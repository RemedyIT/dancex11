// -*- C++ -*-
/**
 * @file   dancex11_ndh_application_manager_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 * @brief  Implementation of Deployment::NodeApplicationManager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_NDH_APPLICATIONMANAGER_IMPL_H_
#define DANCEX11_NDH_APPLICATIONMANAGER_IMPL_H_

#pragma once

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_nodeapplicationmanagerS.h"
#include "dancex11/core/dancex11_artifactinstallationC.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11/scheduler/events/plugin_event_scheduler.h"
#include "dancex11_ndh_application_impl.h"

namespace DAnCEX11
{

  class NodeApplicationManager_Impl :
    public virtual CORBA::servant_traits< ::Deployment::NodeApplicationManager>::base_type
  {
  public:
    NodeApplicationManager_Impl (IDL::traits<PortableServer::POA>::ref_type poa,
                                 std::string name,
                                 std::string domain_nc,
                                 std::shared_ptr<Plugin_Manager> plugins);

    virtual ~NodeApplicationManager_Impl();

    IDL::traits<Deployment::Application>::ref_type
    startLaunch (const Deployment::Properties & configProperty,
                 Deployment::Connections & providedReference) override;

    void destroyApplication (IDL::traits<Deployment::Application>::ref_type) override;

    void preparePlan (const Deployment::DeploymentPlan& plan);

  private:
    void installArtifacts ();
    void removeArtifacts ();

    LocalitySplitter split_plan_;
    IDL::traits<PortableServer::POA>::ref_type poa_;
    CORBA::servant_reference<DAnCEX11::NodeApplication_Impl> application_;
    std::string node_name_;
    std::string domain_nc_;
    Plugin_Event_Scheduler_T<Deployment_Scheduler> scheduler_;
  };

}
#endif /* DANCEX11_NDH_APPLICATIONMANAGER_IMPL_H_ */
