// -*- C++ -*-
/**
 * @file   artifact_deployment_handler.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_ARTIFACT_DEPLOYMENT_HANDLER_IMPL_H_
#define DANCEX11_ARTIFACT_DEPLOYMENT_HANDLER_IMPL_H_

#pragma once

#include "dancex11/core/dancex11_deploymenthandlersC.h"
#include "dancex11/core/dancex11_deploymentinterceptors_base_impl.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11_artifact_deployment_handler_export.h"
#include "tools/artifact_installation/artifact_installation_impl.h"

namespace DAnCEX11
{
  class DAnCE_Artifact_Handler_Export Artifact_Deployment_Handler_i
    : public virtual IDL::traits<DAnCEX11::InstanceDeploymentHandler>::base_type
  {
  public:
    // Constructor
    Artifact_Deployment_Handler_i ();

    // Destructor
    virtual ~Artifact_Deployment_Handler_i ();

    virtual
      std::string instance_type ();

    virtual
      CORBA::StringSeq dependencies ();

    virtual
      void install_instance (const ::Deployment::DeploymentPlan & plan,
                             uint32_t instanceRef,
                             CORBA::Any& instance_reference);

    virtual
      void activate_instance (const ::Deployment::DeploymentPlan & ,
                              uint32_t ,
                              const CORBA::Any &) {};
    virtual
      void passivate_instance (const ::Deployment::DeploymentPlan & ,
                               uint32_t ,
                               const CORBA::Any &) {};

    virtual
      void remove_instance (const ::Deployment::DeploymentPlan & plan,
                            uint32_t instanceRef,
                            const CORBA::Any & instance_reference);

    virtual
      void provide_endpoint_reference (const ::Deployment::DeploymentPlan &,
                                       uint32_t,
                                       CORBA::Any&) {};

    virtual
      void connect_instance (const ::Deployment::DeploymentPlan &,
                             uint32_t ,
                             const CORBA::Any & ) {};

    virtual
      void disconnect_instance (const ::Deployment::DeploymentPlan & ,
                                uint32_t ) {};

    virtual
      void instance_configured (const ::Deployment::DeploymentPlan &,
                                uint32_t) {};

    virtual
      void configure(const Deployment::Properties&);

    virtual void close ();

  private:
    static const std::string instance_type_;
    IDL::traits<DAnCEX11::ArtifactInstallation>::ref_type installer_;
  };

  class DAnCE_Artifact_Handler_Export Artifact_Deployment_Initializer final
    : public DAnCEX11::DeploymentInterceptor_Base
  {
  public:
    Artifact_Deployment_Initializer () = default;

    virtual ~Artifact_Deployment_Initializer ();

    virtual void
    preprocess_plan (Deployment::DeploymentPlan & plan) override;
  };
}

extern "C"
{
  void
  DAnCE_Artifact_Handler_Export create_DAnCEX11_Artifact_Deployment_Handler (
    IDL::traits< ::DAnCEX11::InstanceDeploymentHandler>::ref_type& plugin);
  void
  DAnCE_Artifact_Handler_Export create_DAnCEX11_Artifact_Deployment_Initializer (
    IDL::traits< ::DAnCEX11::DeploymentInterceptor>::ref_type& plugin);
}

#endif /* DANCEX11_LOCALITY)DANCEX11_ARTIFACT_DEPLOYMENT_HANDLER_IMPL_H_ */
