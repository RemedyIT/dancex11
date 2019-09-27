// -*- C++ -*-
/**
 * @file   dancex11_ndh_application_manager_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 * @brief  Implementation of Deployment::NodeApplicationManager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11/logger/log.h"

#include "dancex11_ndh_application_manager_impl.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_deploymentinterceptorsC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/scheduler/deployment_completion.h"
#include "dancex11/scheduler/events/install.h"
#include "dancex11/scheduler/events/remove.h"

#include <list>
#include <map>
#include <sstream>

using namespace DAnCEX11::Utility;

namespace DAnCEX11
{

NodeApplicationManager_Impl::NodeApplicationManager_Impl (
    IDL::traits<PortableServer::POA>::ref_type poa,
    std::string name,
    std::string domain_nc,
    std::shared_ptr<Plugin_Manager> plugins)
    : poa_ (std::move(poa)),
      application_ (nullptr),
      node_name_ (std::move(name)),
      domain_nc_ (std::move(domain_nc)),
      scheduler_ (std::move(plugins))
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_Impl::NodeApplicationManager_Impl");

  // Spawn thread pool
  // @Todo:  We can probably move this up into the NodeManager and
  // share the thread pool among several node applications.
  this->scheduler_.activate_scheduler (0);

  DANCEX11_LOG_TRACE ("NodeApplicationManager_Impl::NodeApplicationManager_Impl - " <<
                      "NodeApplicationManager_Impl created");
}

NodeApplicationManager_Impl::~NodeApplicationManager_Impl()
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_Impl::~NodeApplicationManager_Impl");

  this->scheduler_.terminate_scheduler ();

  try
  {
    if (this->application_)
    {
      IDL::traits<CORBA::Object>::ref_type app =
        this->poa_->servant_to_reference (this->application_);
      PortableServer::ObjectId id = this->poa_->reference_to_id (app);
      this->poa_->deactivate_object (id);
      this->application_.reset ();
    }
  }
  catch (...)
  {
    DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::~NodeApplicationManager_Impl - " <<
                        "Caught exception in NodeApplicationManager destructor");
  }
}

void
NodeApplicationManager_Impl::preparePlan (const Deployment::DeploymentPlan& plan)
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_Impl::preparePlan");

  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::preparePlan - " <<
                      "Performing locality split on plan " << plan.UUID ());

  this->split_plan_.split_plan (plan);

  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::preparePlan - " <<
                      "Plan " << plan.UUID () << " successfully split into " <<
                      this->split_plan_.plans ().size () << " localities.");

  // only attempt to install artifacts if there is a handler loaded
  if (this->scheduler_.plugins ().fetch_installation_handler (
                                    DAnCEX11::DANCE_ARTIFACT))
  {
    this->installArtifacts ();
  }

  // update lm instance deployment configurations
  LocalitySplitter::TSubPlanConstIterator plans_end = this->split_plan_.plans ().end ();
  for (LocalitySplitter::TSubPlanIterator i = this->split_plan_.plans ().begin ();
       i != plans_end;
       ++i)
  {
    ::Deployment::DeploymentPlan &sub_plan =
      i->second;

    const LocalitySplitter::TSubPlanKey &sub_plan_key =
      i->first;

    // the locality splitter makes sure every sub plan contains a Locality Manager
    // instance (creating default if necessary) and identifies it in the key
    uint32_t loc_manager_instance = sub_plan_key.locality_manager_instance ();

    // get deployment descriptors
    const ::Deployment::InstanceDeploymentDescription &lm_idd =
        sub_plan.instance ()[loc_manager_instance];
    ::Deployment::MonolithicDeploymentDescription &lm_mdd =
        sub_plan.implementation ()[lm_idd.implementationRef ()];

    // pass on any configured domain NC as execParameter
    // this way if the domain NC has been explicitly configured as a configProperty
    // for the locality instance in the plan that one will override this setting ultimately
    DAnCEX11::Utility::update_property_value (DAnCEX11::DOMAIN_NC,
                                              lm_mdd.execParameter (),
                                              this->domain_nc_);
  }

  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::preparePlan - " <<
                      "Finished preparing plan " << plan.UUID () << ".");
}

void
NodeApplicationManager_Impl::installArtifacts ()
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::installArtifacts - " <<
                      "triggering artifact installation events");

  uint32_t planix (0);
  std::list < Event_Future > prepared_instances;
  std::map < std::string, ::Deployment::MonolithicDeploymentDescription* > instance_localities;
  Deployment_Completion completion (*this->scheduler_);

  // schedule artifact installation events for each instance of each sub plan
  LocalitySplitter::TSubPlanConstIterator plans_end = this->split_plan_.plans ().end ();
  for (LocalitySplitter::TSubPlanIterator i = this->split_plan_.plans ().begin ();
       i != plans_end;
       ++i)
    {
      ::Deployment::DeploymentPlan &sub_plan =
        i->second;

      const LocalitySplitter::TSubPlanKey &sub_plan_key =
        i->first;

      DANCEX11_LOG_DEBUG ("NodeApplicationManager_impl::installArtifacts - " <<
                          "Considering sub-plan " << planix << ":" << sub_plan.UUID () <<
                          " with " << sub_plan.instance ().size () << " instances");


      DANCEX11_LOG_DEBUG ("NodeApplicationManager_impl::installArtifacts - " <<
                          "calling preprocessing interceptors");

      Plugin_Manager::INTERCEPTORS interceptors =
          this->scheduler_.plugins ().fetch_interceptors ();

      for (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di : interceptors)
      {
        if (di)
        {
          di->preprocess_plan (sub_plan);
        }
      }

      // the locality splitter makes sure every sub plan contains a Locality Manager
      // instance (creating default if necessary) and identifies it in the key

      uint32_t loc_manager_instance = sub_plan_key.locality_manager_instance ();
      const ::Deployment::InstanceDeploymentDescription &lm_idd =
          sub_plan.instance ()[loc_manager_instance];

      // check for LM activation mode
      uint32_t lm_am {};
      if (!Utility::get_property_value (DAnCEX11::LOCALITY_ACTIVATION_MODE,
                                        lm_idd.configProperty (), lm_am))
      {
        // try implementation
        const ::Deployment::MonolithicDeploymentDescription &lm_mdd =
            sub_plan.implementation ()[lm_idd.implementationRef ()];
        Utility::get_property_value (DAnCEX11::LOCALITY_ACTIVATION_MODE,
                                     lm_mdd.execParameter (), lm_am);
      }

      // do not trigger installation events for remote LM instances
      if (lm_am == DAnCEX11::LOCALITY_AM_REMOTE)
      {
        DANCEX11_LOG_INFO ("NodeApplicationManager_impl::installArtifacts - " <<
                           "Skipping sub-plan " << planix << ":"
                           << sub_plan.UUID () <<
                           " with REMOTE LM instance");
      }
      else
      {
        for (uint32_t instanceRef = 0;
              instanceRef < sub_plan.instance ().size () ;
                ++instanceRef)
        {
          Event_Future result;
          completion.accept (result);

          this->scheduler_.schedule_event<Install_Instance> (
              sub_plan,
              instanceRef,
              DAnCEX11::DANCE_ARTIFACT,
              result);

          instance_localities[sub_plan.instance ()[instanceRef].name ()] =
              &sub_plan.implementation ()[lm_idd.implementationRef ()];

          prepared_instances.push_back (result);
        }
      }

      ++planix;
    }

  // TODO - MCO: installation timeout should be configurable
  bool finished {};
  uint32_t timeouts {};
  while (!finished)
  {
    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (30));

    finished = completion.wait_on_completion (std::addressof(tv));

    Event_List completed_events;
    completion.completed_events (completed_events);

    // check if we made progress; we only record timeouts if no progress was made
    if (completed_events.empty () && !finished)
    {
      if (++timeouts > 9)
      {
        DANCEX11_LOG_ERROR ("NodeApplicationManager_impl::installArtifacts - " <<
                            "Timed out while waiting on completion of "
                            "artifact installations");

        throw Deployment::StartError ("NodeApplicationManager",
                                      "Timed out while waiting on completion of "
                                      "artifact installations");
      }
    }

    tv = ACE_Time_Value::zero;

    // process completed installation events
    for (std::list < Event_Future >::iterator i = completed_events.begin ();
         i != completed_events.end ();
         ++i)
    {
      Event_Result event;
      if (i->get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("NodeApplicationManager_impl::installArtifacts - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_)
      {
        DAnCEX11::Utility::throw_exception_from_any (event.contents_);
      }

      std::vector<std::string> install_paths;
      if (event.contents_.impl () &&
          (event.contents_  >>= install_paths) &&
          !install_paths.empty ())
      {
        DANCEX11_LOG_DEBUG ("NodeApplicationManager_impl::installArtifacts - " <<
                            "Successfully installed artifacts for instance " <<
                            event.id_ << ": " <<
                            IDL::traits< std::vector<std::string> >::write (install_paths));

        if (instance_localities.find (event.id_) != instance_localities.end ())
        {
          // get deployment description of LM for this instance
          ::Deployment::MonolithicDeploymentDescription &lm_mdd =
              *instance_localities[event.id_];

          // update path and libpath properties of sub plan locality manager
          std::string search_path;
          for (std::vector<std::string>::iterator it = install_paths.begin ();
               it != install_paths.end ();
               ++it)
          {
            if (!search_path.empty ())
              search_path += ACE_TEXT_ALWAYS_CHAR (ACE_LD_SEARCH_PATH_SEPARATOR_STR);
            search_path += *it;
          }

          // update PATH
          std::string new_path;
          std::string prop_val;
          if (DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_LM_PATH,
                                                  lm_mdd.execParameter (),
                                                  prop_val) && !prop_val.empty ())
          {
            std::swap (new_path, prop_val);
            new_path += ACE_TEXT_ALWAYS_CHAR (ACE_LD_SEARCH_PATH_SEPARATOR_STR);
            new_path += search_path;
          }
          else
          {
            std::swap (new_path, prop_val);
          }
          DAnCEX11::Utility::update_property_value (DAnCEX11::DANCE_LM_PATH,
                                                    lm_mdd.execParameter (),
                                                    new_path);

          // On windows these two search paths are the same
#if !defined (ACE_WIN32)
          // update LIBPATH
          new_path.clear ();
          prop_val.clear ();
          if (DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_LM_LIBPATH,
                                                  lm_mdd.execParameter (),
                                                  prop_val) && !prop_val.empty ())
            {
              std::swap (new_path, prop_val);
              new_path += ACE_TEXT_ALWAYS_CHAR (ACE_LD_SEARCH_PATH_SEPARATOR_STR);
              new_path += search_path;
            }
          else
            {
              std::swap (new_path, prop_val);
            }
          DAnCEX11::Utility::update_property_value (DAnCEX11::DANCE_LM_LIBPATH,
                                                 lm_mdd.execParameter (),
                                                   new_path);
#endif
        }
        else
        {
          DANCEX11_LOG_ERROR ("NodeApplicationManager_impl::installArtifacts - " <<
                              "cannot find locality for instance: " << event.id_);
        }
      }
    }
  }
}

IDL::traits<Deployment::Application>::ref_type
NodeApplicationManager_Impl::startLaunch (const Deployment::Properties &prop,
                                          Deployment::Connections & providedReference)
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_Impl::startLaunch");

  // Creating NodeApplication object
  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::startLaunch - " <<
                      "Initializing NodeApplication");
  this->application_ =
      CORBA::make_reference<NodeApplication_Impl> (
            this->poa_,
            this->node_name_,
            this->scheduler_);
  if (!this->application_)
    throw CORBA::NO_MEMORY ();

  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::startLaunch - " <<
                      "Instructing NodeApplication to prepare locality managers.");

  this->application_->prepare_instances (this->split_plan_.plans ());

  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::startLaunch - " <<
                      "Instructing NodeApplication to start launch localities.");

  try
  {
    this->application_->start_launch_instances (prop, providedReference);
  }
  catch (const CORBA::Exception&)
  {
    DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::startLaunch - " <<
                        "Caught exception from NodeApplication while "
                        "starting to launch localities. Shutting down "
                        "NodeApplication.");

    try
    {
      this->application_->shutdown_instances ();
      this->application_.reset ();
    }
    catch(...)
    {
      DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::startLaunch - " <<
                          "Caught exception while shutting down NodeApplication "
                          "after start failure.");
    }

    throw;
  }

  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::startLaunch - " <<
                      "Activating NodeApplication servant");

  PortableServer::ObjectId const as_id =
    this->poa_->activate_object (this->application_);

  IDL::traits<CORBA::Object>::ref_type as_obj = this->poa_->id_to_reference (as_id);
  IDL::traits<Deployment::Application>::ref_type app = IDL::traits<Deployment::Application>::narrow (as_obj);

  if (!app)
  {
    DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::startLaunch - " <<
                        "NodeApplication servant failed to activate");
    throw ::Deployment::StartError ("NodeApplication",
                                    "Activation failure");
  }
  return app;
}

void
NodeApplicationManager_Impl::destroyApplication (IDL::traits<Deployment::Application>::ref_type application)
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_Impl::destroyApplication");

  try
  {
    if (!application->_is_equivalent (this->poa_->servant_to_reference (this->application_)))
    {
      DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::destroyApplication - " <<
                          "application is equivalent to current application");
      throw ::Deployment::StopError("NodeApplicationManager",
                                    "Wrong application passed to destroyApplication");
    }

    this->application_->remove_instances ();

    PortableServer::ObjectId id = this->poa_->reference_to_id (application);
    this->poa_->deactivate_object (id);

    this->application_.reset ();
  }
  catch (const CORBA::SystemException &)
  {
    throw;
  }
  catch (const Deployment::StopError &)
  {
    throw;
  }
  catch (const CORBA::UserException &e)
  {
    DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::destroyApplication failed " <<
                        "with UserException " << e);
    std::ostringstream os;
    os << e;
    throw Deployment::StopError("NodeApplicationManager", os.str ());
  }
  catch (...)
  {
    DANCEX11_LOG_ERROR ("NodeApplicationManager_Impl::destroyApplication failed " <<
                        "with unknown exception.");
    throw Deployment::StopError("NodeApplicatoinManager", "Unknown C++ exception in destroyApplication");
  }

  // only attempt to remove artifacts if there is a handler loaded
  if (this->scheduler_.plugins ().fetch_installation_handler (
                                    DAnCEX11::DANCE_ARTIFACT))
  {
    this->removeArtifacts ();
  }
}

void
NodeApplicationManager_Impl::removeArtifacts ()
{
  DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::removeArtifacts - " <<
                      "triggering artifact removal events");

  uint32_t planix (0);
  std::list < Event_Future > prepared_instances;
  Deployment_Completion completion (*this->scheduler_);

  // schedule artifact removal events for each instance of each sub plan
  LocalitySplitter::TSubPlanConstIterator plans_end = this->split_plan_.plans ().end ();
  for (LocalitySplitter::TSubPlanIterator i = this->split_plan_.plans ().begin ();
       i != plans_end;
       ++i)
    {
      ::Deployment::DeploymentPlan &sub_plan =
        i->second;

      const LocalitySplitter::TSubPlanKey &sub_plan_key =
        i->first;

      DANCEX11_LOG_TRACE ("NodeApplicationManager_impl::removeArtifacts - " <<
                          "Considering sub-plan " << planix << ":" << sub_plan.UUID () <<
                          " with " << sub_plan.instance ().size () << " instances");

      // the locality splitter makes sure every sub plan contains a Locality Manager
      // instance (creating default if necessary) and identifies it in the key

      uint32_t loc_manager_instance = sub_plan_key.locality_manager_instance ();
      const ::Deployment::InstanceDeploymentDescription &lm_idd =
          sub_plan.instance ()[loc_manager_instance];

      // check for LM activation mode
      uint32_t lm_am {};
      if (!Utility::get_property_value (DAnCEX11::LOCALITY_ACTIVATION_MODE,
                                        lm_idd.configProperty (), lm_am))
      {
        // try implementation
        const ::Deployment::MonolithicDeploymentDescription &lm_mdd =
            sub_plan.implementation ()[lm_idd.implementationRef ()];
        Utility::get_property_value (DAnCEX11::LOCALITY_ACTIVATION_MODE,
                                     lm_mdd.execParameter (), lm_am);
      }

      // do not trigger removal events for remote LM instances
      if (lm_am == DAnCEX11::LOCALITY_AM_REMOTE)
      {
        DANCEX11_LOG_INFO ("NodeApplicationManager_impl::removeArtifacts - " <<
                           "Skipping sub-plan " << planix << ":"
                           << sub_plan.UUID () <<
                           " with REMOTE LM instance");
      }
      else
      {
        for (uint32_t instanceRef = 0;
              instanceRef < sub_plan.instance ().size () ;
                ++instanceRef)
        {
          Event_Future result;
          completion.accept (result);

          this->scheduler_.schedule_event<Remove_Instance> (
              sub_plan,
              instanceRef,
              DAnCEX11::DANCE_ARTIFACT,
              result);

          prepared_instances.push_back (result);
        }
      }

      ++planix;
    }

  // TODO - MCO: (de-)installation timeout should be configurable
  bool finished {};
  uint32_t timeouts {};
  while (!finished)
  {
    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (30));

    finished = completion.wait_on_completion (std::addressof(tv));

    Event_List completed_events;
    completion.completed_events (completed_events);

    // check if we made progress; we only record timeouts if no progress was made
    if (completed_events.empty () && !finished)
    {
      if (++timeouts > 9)
      {
        DANCEX11_LOG_ERROR ("NodeApplicationManager_impl::removeArtifacts - " <<
                            "Timed out while waiting on completion of "
                            "artifact removals");

        throw Deployment::StopError ("NodeApplicationManager",
                                      "Timed out while waiting on completion of "
                                      "artifact removals");
      }
    }

    tv = ACE_Time_Value::zero;

    // process completed installation events
    for (std::list < Event_Future >::iterator i = completed_events.begin ();
         i != completed_events.end ();
         ++i)
    {
      Event_Result event;
      if (i->get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("NodeApplicationManager_impl::removeArtifacts - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_)
      {
        DAnCEX11::Utility::throw_exception_from_any (event.contents_);
      }
    }
  }
}

} /* namespace DAnCEX11 */

