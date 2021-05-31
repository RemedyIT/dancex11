// -*- C++ -*-
/**
 * @file dancex11_ndh_manager_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 * @brief  Implementation of Deployment::NodeManager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_ndh_manager_impl.h"
#include "dancex11/logger/log.h"

#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/handler/instance/plugin_conf.h"

namespace DAnCEX11
{
  NodeManager_Impl::NodeManager_Impl(
      IDL::traits<PortableServer::POA>::ref_type poa,
      std::string name,
      std::string domain_nc,
      std::shared_ptr<Plugin_Manager> plugins,
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
    : poa_ (std::move(poa)),
      sh_ (std::move(sh)),
      name_ (std::move(name)),
      domain_nc_ (std::move(domain_nc)),
      plugins_ (std::move(plugins))
  {
    DANCEX11_LOG_TRACE ( "NodeManager_Impl::NodeManager_Impl");

    DANCEX11_LOG_DEBUG ("NodeManager_Impl::NodeManager_Impl has been created");
  }

  NodeManager_Impl::~NodeManager_Impl()
  {
    DANCEX11_LOG_TRACE ("NodeManager_Impl::~NodeManager_Impl()");
    for (TManagers::iterator iter = this->managers_.begin();
         iter != this->managers_.end();
         ++iter)
    {
      try
      {
        PortableServer::ObjectId id = this->poa_->servant_to_id (iter->second);
        DANCEX11_LOG_TRACE ("NodeManager_impl::~NodeManager_impl - Deactivating NodeApplicationManager " <<
                            iter->first);
        this->poa_->deactivate_object (id);
        DANCEX11_LOG_TRACE ("NodeManager_impl::~NodeManager_impl - deleting NodeApplicationManager");
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NodeManager_impl::~NodeManager_impl - Caught exception while removing " <<
                            "NodeApplicationManager " << iter->first);
      }
    }
  }

  void
  NodeManager_Impl::joinDomain (const ::Deployment::Domain & /*theDomain*/,
                                IDL::traits< ::Deployment::TargetManager>::ref_type /*manager*/,
                                IDL::traits< ::Deployment::Logger>::ref_type /*log*/,
                                int32_t /*updateInterval*/)
  {
    DANCEX11_LOG_TRACE ("NodeManager_Impl::joinDomain");
    DANCEX11_LOG_WARNING ("NodeManager_Impl::joinDomain - joinDomain not implemented");
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  NodeManager_Impl::leaveDomain ()
  {
    DANCEX11_LOG_TRACE ("NodeManager_Impl::leaveDomain");
    DANCEX11_LOG_WARNING ("NodeManager_Impl::leaveDomain - leave not implemented");
    throw CORBA::NO_IMPLEMENT ();
  }

  IDL::traits< ::Deployment::ApplicationManager>::ref_type
  NodeManager_Impl::preparePlan (
      const ::Deployment::DeploymentPlan & plan,
      IDL::traits< ::Deployment::ResourceCommitmentManager>::ref_type /*resourceCommitment*/)
  {
    DANCEX11_LOG_TRACE ( "NodeManager_Impl::preparePlan");

    DANCEX11_LOG_DEBUG ("NodeManager_impl::preparePlan - "
                        "started for node [" << this->name_ <<
                        "] and plan [" << plan.UUID () << "]");

#if !defined(NDEBUG)
    DANCEX11_LOG_TRACE ("NodeManager_impl::preparePlan - " <<
                        IDL::traits< ::Deployment::DeploymentPlan>::write (plan));
#endif

    // resourceCommitment will be used on next development stage
    if (this->managers_.find (plan.UUID ()) != this->managers_.end ())
    {
      // What should we do here if we already have application for this plan?
      // Probably it is mistake because we should previously call destroyApplication
      // before performe relaunching of application
      DANCEX11_LOG_CRITICAL ("NodeManager_impl::preparePlan - " <<
                             "ApplicationManager for UUID " << plan.UUID () <<
                             " already exists");
      throw ::Deployment::PlanError(plan.UUID (),
                                    "Plan with same UUID already exists");
    }
    DANCEX11_LOG_DEBUG ("NodeManager_impl::preparePlan - creating NodeApplicationManager...");
    CORBA::servant_reference<NodeApplicationManager_Impl> manager =
        CORBA::make_reference<NodeApplicationManager_Impl>
                (this->poa_,
                 this->name_,
                 this->domain_nc_,
                 this->plugins_);
    if (!manager)
      throw CORBA::NO_MEMORY();

    DANCEX11_LOG_DEBUG ("NodeManager_impl::preparePlan - " <<
                        "invoking preparePlan on NodeApplicationManager...");
    manager->preparePlan (plan);

    DANCEX11_LOG_DEBUG ("NodeManager_impl::preparePlan - activating NodeApplicationManager...");
    PortableServer::ObjectId const id = this->poa_->activate_object (manager);
    IDL::traits<CORBA::Object>::ref_type nam = this->poa_->id_to_reference (id);

    // There is an idea to check if plan.UUID really exists
    this->managers_[plan.UUID ()] = manager;

    // narrow should return a nil reference if it fails.
    DANCEX11_LOG_DEBUG ("NodeManager_impl::preparePlan - "
                        "NodeApplicationManager for plan " << plan.UUID () << " completed");
    return IDL::traits< ::Deployment::NodeApplicationManager>::narrow (nam);
  }

  void
  NodeManager_Impl::destroyManager (IDL::traits< ::Deployment::ApplicationManager>::ref_type appManager)
  {
    DANCEX11_LOG_TRACE ("NodeManager_Impl::destroyManager");

    for (TManagers::iterator iter = this->managers_.begin();
         iter != this->managers_.end();
         ++iter)
    {
      if (appManager->_is_equivalent (this->poa_->servant_to_reference (iter->second)))
      {
        PortableServer::ObjectId id = this->poa_->reference_to_id (appManager);
        this->poa_->deactivate_object (id);
        DANCEX11_LOG_TRACE ("NodeManager_impl::destroyManager - erasing NodeApplicationManager");
        this->managers_.erase (iter);
        DANCEX11_LOG_TRACE ("NodeManager_impl::destroyManager - finished");
        return;
      }
    }

    DANCEX11_LOG_ERROR ("NodeManager_impl::destroyManager - "
                        "corresponding NodeApplicationManager cannot be found");
    throw ::Deployment::InvalidReference();
  }

  ::Deployment::Resources
  NodeManager_Impl::getDynamicResources ()
  {
    DANCEX11_LOG_TRACE ( "NodeManager_Impl::getDynamicResources ()");
    DANCEX11_LOG_WARNING ("NodeManager_Impl::getDynamicResources - " <<
                          "getDynamicResources not implemented");
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  NodeManager_Impl::shutdown ()
  {
    DANCEX11_LOG_TRACE ("NodeManager_impl::shutdown");

    // call shutdown handler
    if (this->sh_)
      this->sh_->shutdown ();

    if (DAnCEX11::State::instance ()->orb ())
    {
      DAnCEX11::State::instance ()->orb ()->shutdown (false);
    }
  }

} //DAnCEX11

