// -*- C++ -*-
/**
 * @file   dancex11_ndh_application_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 * @brief  Implementation of Deployment::NodeApplication
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "dancex11_ndh_application_impl.h"

#include "ace/OS_Memory.h"
#include "ace/CORBA_macros.h"

#include "dancex11/logger/log.h"
#include "dancex11/deployment/deployment_baseC.h"
#include "dancex11/deployment/deployment_applicationC.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/deployment/deployment_applicationmanagerC.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11/scheduler/deployment_completion.h"
#include "dancex11/scheduler/events/install.h"
#include "dancex11/scheduler/events/remove.h"

#include <string>

namespace DAnCEX11
{

NodeApplication_Impl::NodeApplication_Impl (
    IDL::traits<PortableServer::POA>::ref_type poa,
    std::string node_name,
    Plugin_Event_Scheduler_T<Deployment_Scheduler> &scheduler)
  : poa_ (std::move(poa)),
    node_name_ (std::move(node_name)),
    scheduler_ (scheduler),
    spawn_delay_ (30)
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::NodeApplication_Impl");
}

NodeApplication_Impl::~NodeApplication_Impl()
{
  DANCEX11_LOG_TRACE ( "NodeApplication_Impl::~NodeApplication_Impl()");
}

void
NodeApplication_Impl::prepare_instances (const LocalitySplitter::TSubPlans& plans)
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instances");

  uint32_t plan (0);
  std::list < Event_Future > prepared_instances;
  Deployment_Completion completion (*this->scheduler_);

  // for each sub plan
  LocalitySplitter::TSubPlanConstIterator plans_end = plans.end ();
  for (LocalitySplitter::TSubPlanConstIterator i = plans.begin ();
       i != plans_end;
       ++i)
    {
      const ::Deployment::DeploymentPlan &sub_plan =
        i->second;

      const LocalitySplitter::TSubPlanKey &sub_plan_key =
        i->first;

      DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instances - " <<
                          "Considering sub-plan " << plan << ":" << sub_plan.UUID () <<
                          " with " << sub_plan.instance ().size () << " instances");

      // the locality splitter makes sure every sub plan contains a Locality Manager
      // instance (creating default if necessary) and identifies it in the key

      uint32_t loc_manager_instance = sub_plan_key.locality_manager_instance ();
      const ::Deployment::InstanceDeploymentDescription &lm_idd =
          sub_plan.instance ()[loc_manager_instance];

      // check for custom spawn delay
      if (!Utility::get_property_value (DAnCEX11::LOCALITY_TIMEOUT,
                                        lm_idd.configProperty (), this->spawn_delay_))
      {
        // try implementation
        const ::Deployment::MonolithicDeploymentDescription &lm_mdd =
            sub_plan.implementation ()[lm_idd.implementationRef ()];
        Utility::get_property_value (DAnCEX11::LOCALITY_TIMEOUT,
                                     lm_mdd.execParameter (), this->spawn_delay_);
      }

      DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instances - " <<
                          "Found Locality Manager instance " << loc_manager_instance <<
                          ":" << lm_idd.name () << ", deploying");

      this->sub_plans_ [ lm_idd.name () ] = SUB_PLAN (loc_manager_instance,
                                                         sub_plan);

      Event_Future result;
      completion.accept (result);

      this->scheduler_.schedule_event<Install_Instance> (
          this->sub_plans_ [ lm_idd.name () ].second,
          loc_manager_instance,
          DAnCEX11::DANCE_LOCALITYMANAGER,
          result);

      prepared_instances.push_back (result);
      ++plan;
    }

  ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

  if (!completion.wait_on_completion (std::addressof(tv)))
  {
    DANCEX11_LOG_ERROR ("NodeApplication_Impl::prepare_instances - " <<
                        "Timed out while waiting on completion of scheduler");
  }

  tv = ACE_Time_Value::zero;

  plan = 0;
  for (std::list < Event_Future >::iterator i = prepared_instances.begin ();
       i != prepared_instances.end ();
       ++i)
  {
    Event_Result event;
    if (i->get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("NodeApplication_Impl::prepare_instances - " <<
                            "Failed to get future value for current instance");
        continue;
      }

    if (event.exception_)
      {
        DAnCEX11::Utility::throw_exception_from_any (event.contents_);
      }

    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type lm_ref;
    if (event.contents_.impl () &&
        (event.contents_  >>= lm_ref) &&
        lm_ref)
      {
        this->localities_[event.id_] = lm_ref;
        DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instances - " <<
                            "Successfully started Locality " <<
                            event.id_);
      }
    else
      {
        DANCEX11_LOG_ERROR ("NodeApplication_Impl::prepare_instances - " <<
                            "Unable to resolve LocalityManager object reference\n");
        throw ::Deployment::StartError (event.id_,
                                        "Unable to resolve LocalityManager object ref");
      }

    DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instances - "
                        "Invoking preparePlan on locality " <<
                        event.id_);

    this->prepare_instance (event.id_,
                            (this->sub_plans_[event.id_].second));

    DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instances - " <<
                        "Successfully executed preparePlan on locality " <<
                        event.id_);
  }
}

void
NodeApplication_Impl::prepare_instance (const std::string & name,
                                        const Deployment::DeploymentPlan &plan)
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instance");

  IDL::traits< ::Deployment::ApplicationManager>::ref_type app;

  try
  {
    app = this->localities_[name]->preparePlan (plan, nullptr);
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::prepare_instance - " <<
                        "Locality <" << name << "> successfully prepared.");
  }
  catch (const CORBA::Exception &ex)
  {
    DANCEX11_LOG_ERROR ("NodeApplication_Impl::prepare_instance - " <<
                        "Caught unexpected CORBA exception while invoking preparePlan " <<
                        ex);
  }
  // @@ TODO:  Ouch! We're swallowing exceptions here!

  if (!app)
  {
    DANCEX11_LOG_ERROR ("NodeApplication_Impl::prepare_instance - " <<
                        "Error: Didn't get a valid reference back from LM preparePlan " <<
                        "for locality " << name);

    // @@ TODO: Ouch!  What do we need to do here?!?
  }

  // For the time being, we don't need to cache this reference.
  // it's the same as the reference we used to invoke preparePlan.
}

void
NodeApplication_Impl::start_launch_instances (const Deployment::Properties &prop,
                                              Deployment::Connections & providedReference)
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::start_launch_instances");

  for (LOCALITY_MAP::iterator i = this->localities_.begin ();
       i != this->localities_.end (); ++i)
  {
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::start_launch_instances - " <<
                        "StartLaunching locality <" << i->first << ">");

    try
    {
      ::Deployment::Connections instance_references;
      i->second->startLaunch (prop,
                              instance_references);


      for (const ::Deployment::Connection& dcon : instance_references)
      {
        providedReference.push_back (dcon);
      }
    }
    catch (const Deployment::PlanError &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::start_launch_instances - " <<
                          "Caught PlanError Exception " << ex.name () << ":" << ex.reason () );
      throw;
    }
    catch (const Deployment::StartError &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::start_launch_instances - " <<
                          "Caught StartError Exception " << ex.name () << ":" << ex.reason () );
      throw;
    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::start_launch_instances - " <<
                          "Caught CORBA Exception " << ex);
      throw;
    }
  }
}

void
NodeApplication_Impl::finishLaunch (const ::Deployment::Connections & providedReference,
                                    bool start)
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::finishLaunch");


  for (LOCALITY_MAP::iterator i = this->localities_.begin ();
       i != this->localities_.end (); ++i)
  {
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::finishLaunch - " <<
                        "FinishLaunching locality <" << i->first << ">");

    try
    {
      i->second->finishLaunch (providedReference, start);
    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::finishLaunch - " <<
                          "Caught CORBA Exception " << ex);
      throw;
    }
  }
}

void
NodeApplication_Impl::start ()
{
  DANCEX11_LOG_TRACE ( "NodeApplication_Impl::start");

  for (LOCALITY_MAP::iterator i = this->localities_.begin ();
       i != this->localities_.end (); ++i)
  {
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::start - " <<
                        "Starting locality <" << i->first << ">");

    try
    {
      i->second->start ();
    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::start - " <<
                          "Caught CORBA Exception " << ex);
      throw;
    }
  }
}

void
NodeApplication_Impl::stop ()
{
  DANCEX11_LOG_TRACE ( "NodeApplication_Impl::stop");

  for (LOCALITY_MAP::iterator i = this->localities_.begin ();
       i != this->localities_.end (); ++i)
  {
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::stop - " <<
                        "Stopping locality <" << i->first << ">");

    try
    {
      i->second->stop ();
    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::stop - " <<
                          "Caught CORBA Exception " << ex);
      throw;
    }
  }
}

void
NodeApplication_Impl::remove_instances ()
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::remove_instances");

  ::Deployment::StopError final_exception;
  bool flag (false);

  std::list < Event_Future > removed_instances;

  Deployment_Completion completion (*this->scheduler_);

  for (LOCALITY_MAP::iterator i = this->localities_.begin ();
       i != this->localities_.end (); ++i)
  {
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::remove_instances - " <<
                        "Removing locality <" << i->first << ">");

    try
    {
      CORBA::Any ref;
      ref <<= i->second;

      i->second->destroyApplication (0);

      Event_Future result;

      PLAN_MAP::iterator sub_plan;

      if ((sub_plan = this->sub_plans_.find (i->first)) !=
          this->sub_plans_.end ())
      {
        this->scheduler_.schedule_event<Remove_Instance> (
            sub_plan->second.second,
            sub_plan->second.first,
            DANCE_LOCALITYMANAGER,
            result);

        removed_instances.push_back (result);
        completion.accept (result);
      }
      else
      {
        DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - "
                            "Unable to find sub plan for instance <" << i->first << ">");
      }
    }
    catch (const ::Deployment::StopError &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - "
                          "Caught StopError final_exception " << ex.name () << ", " << ex.reason ());
      Utility::test_and_set_exception (flag,
                                       final_exception,
                                       ex.name (),
                                       ex.reason ());

    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - " <<
                          "Caught CORBA Final_Exception " << ex);
      Utility::test_and_set_exception (flag,
                                       final_exception,
                                       "Unknown CORBA Final_Exception",
                                       ex);
    }
  }

  ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

  if (!completion.wait_on_completion (std::addressof(tv)))
  {
    DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - " <<
                        "Timed out while waiting on completion of scheduler");
  }

  tv = ACE_Time_Value::zero;

  for (std::list < Event_Future >::iterator i = removed_instances.begin ();
       i != removed_instances.end ();
       ++i)
  {
    try
    {
      Event_Result event;

      if (i->get (event,
                  &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      using DAnCEX11::Utility::extract_and_throw_exception;

      if (event.exception_ &&
          !(extract_and_throw_exception< ::Deployment::StopError > (event.contents_)))
      {
        DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - " <<
                            "Unexpected exception thrown during removal of " <<
                            "instance <" << event.id_ << ">");

        throw ::Deployment::StopError (event.id_,
                                       "Unknown exception thrown from remove_instance");
      }

    }
    catch (const ::Deployment::StopError &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - " <<
                          "Caught StopError final_exception " << ex.name () << ", " << ex.reason ());
      Utility::test_and_set_exception (flag,
                                       final_exception,
                                       ex.name (),
                                       ex.reason ());

    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::remove_instances - " <<
                          "Caught CORBA Final_Exception " << ex);
      Utility::test_and_set_exception (flag,
                                       final_exception,
                                       "Unknown CORBA Final_Exception",
                                       ex);
    }

  }
  if (flag)
    throw final_exception;
}

void
NodeApplication_Impl::shutdown_instances ()
{
  DANCEX11_LOG_TRACE ("NodeApplication_Impl::shutdown_instances");

  for (LOCALITY_MAP::iterator i = this->localities_.begin ();
       i != this->localities_.end (); ++i)
  {
    DANCEX11_LOG_TRACE ("NodeApplication_Impl::shutdown_instances - " <<
                        "Shutting down locality <" << i->first << ">");

    try
    {
      i->second->shutdown ();

    }
    catch (const CORBA::Exception &ex)
    {
      DANCEX11_LOG_ERROR ("NodeApplication_Impl::shutdown_instances - " <<
                          "Caught CORBA Exception " << ex);
    }
  }
}

} /* namespace DAnCEX11 */
