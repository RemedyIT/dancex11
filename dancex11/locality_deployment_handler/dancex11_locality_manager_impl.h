// -*- C++ -*-
/**
 * @file    dancex11_locality_manager_impl.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_LOCALITYMANAGER_IMPL_H_
#define DAnCEX11_LOCALITYMANAGER_IMPL_H_

#pragma once

#include "dancex11/core/dancex11_localitymanagerS.h"
#include "dancex11/core/dancex11_deploymentinterceptorsC.h"
#include "dancex11/core/dancex11_shutdown_controlC.h"
#include "dancex11_locality_deployment_handler_export.h"
#include "dancex11/scheduler/deployment_scheduler.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11/scheduler/events/plugin_event_scheduler.h"

#include <map>
#include <vector>
#include <list>
#include <string>

namespace DAnCEX11
{

  class DANCEX11_LOCALITY_DH_Export LocalityManager_i final
    : public CORBA::servant_traits<DAnCEX11::LocalityManager>::base_type
  {
  public:
    LocalityManager_i (const std::string &uuid,
      std::shared_ptr<Plugin_Manager> plugins,
      IDL::traits<PortableServer::POA>::ref_type poa,
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh);

    virtual ~LocalityManager_i ();

    void
    configure (const Deployment::Properties &prop) override;

    Deployment::Properties
    configuration () override;

    IDL::traits<Deployment::ApplicationManager>::ref_type
    preparePlan (const Deployment::DeploymentPlan &plan,
      IDL::traits<Deployment::ResourceCommitmentManager>::ref_type resourceCommitment) override;

    void shutdown () override;

    IDL::traits<Deployment::Application>::ref_type
    startLaunch (const Deployment::Properties &configProperty,
      Deployment::Connections &providedReference) override;

    void
    finishLaunch (const Deployment::Connections &providedReferences,
      bool start) override;

    void
    start () override;

    void
    stop () override;

    void
    destroyApplication (IDL::traits<Deployment::Application>::ref_type app) override;

    void
    destroyManager (IDL::traits<Deployment::ApplicationManager>::ref_type manager) override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    LocalityManager_i () = delete;
    LocalityManager_i (const LocalityManager_i&) = delete;
    LocalityManager_i (LocalityManager_i&&) = delete;
    LocalityManager_i& operator= (const LocalityManager_i& x) = delete;
    LocalityManager_i& operator= (LocalityManager_i&& x) = delete;
    //@}
  private:
    void
    install_instances ();

    void
    collect_references (Deployment::Connections &providedReference);

    void
    disconnect_connections ();

    std::string
    determine_instance_type (const Deployment::PlanConnectionDescription &conn,
      const Deployment::Connection &providedReference);

    std::string uuid_;

    IDL::traits<PortableServer::POA>::ref_type poa_;

    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh_;

    typedef std::list< uint32_t > INSTANCE_LIST;

    typedef std::pair< std::string, INSTANCE_LIST > HANDLER_TABLE_PAIR;
    typedef std::map< std::string, INSTANCE_LIST > HANDLER_TABLE;

    HANDLER_TABLE instance_handlers_;

    Plugin_Manager::INSTALL_ORDER handler_order_;

    Deployment::DeploymentPlan plan_;

    Plugin_Event_Scheduler_T<Deployment_Scheduler> scheduler_;

    uint32_t spawn_delay_;
  };

}
#endif /* DAnCEX11_LOCALITYMANAGER_IMPL_H_ */
