/**
 * @file    dancex11_locality_deployment_handler.h
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 Locality DeploymentManagerHandler implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_LOCALITY_DEPLOYMENT_HANDLER_H_
#define DANCEX11_LOCALITY_DEPLOYMENT_HANDLER_H_

#pragma once

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/core/dancex11_deploymentmanagerhandlerC.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11_locality_deployment_handler_export.h"

#include "tao/x11/portable_server/portableserver.h"

namespace DAnCEX11
{
  class DANCEX11_LOCALITY_DH_Export LocalityDeploymentHandler final
  : public DeploymentManagerHandler
  {
  public:
    LocalityDeploymentHandler (std::string uuid,
                               std::string = {});
    ~LocalityDeploymentHandler ();

    std::string handler_type() override;

    void
    configure (
        const std::string& config,
        const ::Deployment::Properties& prop,
        IDL::traits<::DAnCEX11::ShutdownHandler>::ref_type sh) override;

    IDL::traits<::Deployment::Deployment_Manager>::ref_type
    activate_manager (
        const std::string& name) override;

    IDL::traits<::DAnCEX11::DeploymentLaunchManager>::ref_type
    activate_launcher (
        const std::string& name) override;

    void shutdown () override;

    static const std::string handler_type_;
    static const std::string config_file_;

  private:
    void create_poas ();

    std::string uuid_;
    std::string activator_;

    IDL::traits<PortableServer::POA>::ref_type root_poa_;
    IDL::traits<PortableServer::POA>::ref_type mng_poa_;

    IDL::traits<::DAnCEX11::ShutdownHandler>::ref_type shutdown_handler_;

    std::shared_ptr<Plugin_Manager> plugins_;

    Deployment::DeploymentPlan launch_plan_;

    PortableServer::ObjectId servant_id_;
  };
}

#endif /* DANCEX11_NODE_DEPLOYMENT_HANDLER_H_ */
