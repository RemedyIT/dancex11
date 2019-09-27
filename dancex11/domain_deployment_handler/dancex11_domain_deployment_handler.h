/**
 * @file    dancex11_domain_deployment_handler.h
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 Domain DeploymentManagerHandler implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_DOMAIN_DEPLOYMENT_HANDLER_H_
#define DANCEX11_DOMAIN_DEPLOYMENT_HANDLER_H_

#pragma once

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/core/dancex11_deploymentmanagerhandlerC.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11_domain_deployment_handler_export.h"

#include "tao/x11/portable_server/portableserver.h"

namespace DAnCEX11
{
  class DANCEX11_DOMAIN_DH_Export DomainDeploymentHandler final
  : public DeploymentManagerHandler
  {
  public:
    DomainDeploymentHandler (
        std::vector<std::string> nodes,
        std::string cdd,
        std::string cddfmt);
    ~DomainDeploymentHandler () = default;

    virtual
    std::string
    handler_type() override;

    virtual
    void
    configure (
        const std::string& config,
        const Deployment::Properties& prop,
        IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh) override;

    virtual
    IDL::traits< ::Deployment::Deployment_Manager>::ref_type
    activate_manager (
        const std::string& name) override;

    virtual
    IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::ref_type
    activate_launcher (
        const std::string& name) override;

    virtual
    void
    shutdown () override;

    static const std::string handler_type_;
    static const std::string config_file_;

  private:

    void create_poas ();

    std::vector<std::string> nodes_;
    std::string cdd_;
    std::string cddfmt_;

    IDL::traits<PortableServer::POA>::ref_type root_poa_;
    IDL::traits<PortableServer::POA>::ref_type mng_poa_;

    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type shutdown_handler_;

    std::string domain_nc_;

    std::shared_ptr<Plugin_Manager> plugins_;

    Deployment::DeploymentPlan launch_plan_;

    PortableServer::ObjectId servant_id_;
  };
}

#endif /* DANCEX11_DOMAIN_DEPLOYMENT_HANDLER_H_ */
