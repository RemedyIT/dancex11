//--*C++*--
/**
 * @file   dancex11_execution_manager_impl.cpp
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  Implementation of Execution manager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_execution_manager_impl.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  ExecutionManager_Impl::ExecutionManager_Impl (
      IDL::traits<PortableServer::POA>::ref_type poa,
      std::string name,
      std::string nc,
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
    : poa_ (std::move(poa)),
      sh_ (std::move(sh)),
      name_ (std::move(name)),
      locator_ (std::move(nc))
  {
    DANCEX11_LOG_TRACE ( "ExecutionManager_Impl::ExecutionManager_Impl");
  }

  ExecutionManager_Impl::~ExecutionManager_Impl()
  {
    DANCEX11_LOG_TRACE ( "ExecutionManager_Impl::~ExecutionManager_Impl");
    for (TDomainManagers::iterator iter = this->managers_.begin();
         iter != this->managers_.end();
         ++iter)
    {
      try
      {
        PortableServer::ObjectId const id = this->poa_->servant_to_id (iter->second);
        DANCEX11_LOG_TRACE ("ExecutionManager_impl::~ExecutionManager_impl - Deactivating DomainApplicationManager " <<
                            iter->first);
        this->poa_->deactivate_object (id);
        DANCEX11_LOG_TRACE ("ExecutionManager_impl::~ExecutionManager_impl - Deleting DomainApplicationManager");
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("ExecutionManager_impl::~ExecutionManager_impl - Caught exception while removing " <<
                            "DomainApplicationManager " << iter->first);
      }
    }
  }

  IDL::traits< ::Deployment::ApplicationManager>::ref_type
  ExecutionManager_Impl::preparePlan (const ::Deployment::DeploymentPlan& plan,
                                      IDL::traits< ::Deployment::ResourceCommitmentManager>::ref_type /*resourceCommitment*/)
  {
    DANCEX11_LOG_TRACE ( "ExecutionManager_Impl::preparePlan");
    // Check if plan is already deployed.
    if (this->managers_.find (plan.UUID ()) != this->managers_.end ())
    {
      DANCEX11_LOG_ERROR ("ExecutionManager_Impl::preparePlan - "
                          "DomainApplicationManager with specified UUID already exists\n");
      // Should we return on this situation reference on existed DomainApplicationManager or
      // we should throw PlanError exception?
      DANCEX11_LOG_CRITICAL ("ExecutionManager_impl::preparePlan - " <<
                              "DomainApplicationManager for UUID " << plan.UUID () <<
                              " already exists");
      throw ::Deployment::PlanError(plan.UUID (),
                                    "Plan with same UUID already exists");
    }

    try
    {
      CORBA::servant_reference<DomainApplicationManager_Impl> dam =
            CORBA::make_reference<DomainApplicationManager_Impl> (
                              this->poa_,
                              plan,
                              this->locator_);

      if (!dam)
        throw CORBA::NO_MEMORY ();

      DANCEX11_LOG_DEBUG ("ExecutionManager_Impl::preparePlan - "
                          "Domain Application Manager was successfully created.");
      dam->preparePlan ();

      DANCEX11_LOG_DEBUG ("ExecutionManager_Impl::preparePlan - "
                          "Domain Application Manager preparePlan has been called.");

      PortableServer::ObjectId const id = this->poa_->activate_object (dam);
      IDL::traits<CORBA::Object>::ref_type manager = this->poa_->id_to_reference (id);
      // There is an idea to check if plan.UUID really exists
      this->managers_[plan.UUID ()] = dam;

      // narrow should return a nil reference if it fails.
      DANCEX11_LOG_DEBUG ("ExecutionManager_Impl::preparePlan - "
                          "DomainApplicationManager for plan " <<
                          plan.UUID () << " completed");

      DANCEX11_LOG_DEBUG ("ExecutionManager_Impl::preparePlan - finished");
      return IDL::traits< ::Deployment::DomainApplicationManager>::narrow (manager);
    }
    catch (const CORBA::Exception& /*ex*/)
    {
      DANCEX11_LOG_ERROR ("ExecutionManager_Impl::preparePlan - "
                         "Propagating CORBA exception caught here");
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("ExecutionManager_Impl::preparePlan - "
                         "Propagating exception caught here");
    }
    return {};
  }

  ::Deployment::DomainApplicationManagers
   ExecutionManager_Impl::getManagers ()
  {
    DANCEX11_LOG_DEBUG ("ExecutionManager_Impl::getManagers");
    ::Deployment::DomainApplicationManagers managers;

    for (std::pair<std::string, CORBA::servant_reference<DomainApplicationManager_Impl>> iter : this->managers_)
    {
      IDL::traits<CORBA::Object>::ref_type manager = this->poa_->servant_to_reference(iter.second);
      managers.push_back(IDL::traits< ::Deployment::DomainApplicationManager>::narrow (manager));
    }
    return managers;
  }

  void
  ExecutionManager_Impl::destroyManager(IDL::traits< ::Deployment::ApplicationManager>::ref_type manager)
  {
    DANCEX11_LOG_TRACE ("ExecutionManager_Impl::destroyManager");

    for (TDomainManagers::iterator iter = this->managers_.begin();
          iter != this->managers_.end();
          ++iter)
    {
      if (manager->_is_equivalent (this->poa_->servant_to_reference (iter->second)))
      {
        DANCEX11_LOG_DEBUG ("ExecutionManager_impl::destroyManager - " <<
                            "deactivating DomainApplicationManager");
        PortableServer::ObjectId const id = this->poa_->reference_to_id (manager);
        this->poa_->deactivate_object (id);

        DANCEX11_LOG_DEBUG ("ExecutionManager_impl::destroyManager - " <<
                            "calling destroyManager on app manager");

        iter->second->destroyManager ();

        DANCEX11_LOG_DEBUG ("ExecutionManager_impl::destroyManager - " <<
                            "erasing DomainApplicationManager");
        this->managers_.erase (iter);
        DANCEX11_LOG_DEBUG ("ExecutionManager_impl::destroyManager - " <<
                            "finished");
        return;
      }
    }

    DANCEX11_LOG_ERROR ("ExecutionManager_impl::destroyManager - "
                        "correponding DomainApplicationManager cannot be found");
    throw ::Deployment::InvalidReference();
  }

  void
  ExecutionManager_Impl::add_node_manager (const std::string& name, const std::string& ior)
  {
    DANCEX11_LOG_TRACE ("ExecutionManager_Impl::add_node_manager");
    this->locator_.store_ior (name, ior);
  }

  void
  ExecutionManager_Impl::load_cdd (const std::string &filename, const std::string& fmt)
  {
    DANCEX11_LOG_TRACE ("ExecutionManager_Impl::load_cdd");
    if (!this->locator_.process_cdd (filename, fmt))
    {
      DANCEX11_LOG_ERROR ("ExecutionManager_impl::load_cdd - "
                          "Error processing cdd");

      throw Deployment::StartError ("ExecutionManager_Impl", "Error loading cdd file");
    }
  }

  void
  ExecutionManager_Impl::shutdown ()
  {
    DANCEX11_LOG_TRACE ("ExecutionManager_Impl::shutdown");

    // call shutdown handler
    if (this->sh_)
    {
      this->sh_->shutdown ();
    }

    if (DAnCEX11::State::instance ()->orb ())
    {
      DAnCEX11::State::instance ()->orb ()->shutdown (false);
    }
  }
} // DAnCEX11
