// -*- C++ -*-
/**
 * @file locality_manager_handler_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_LOCALITY_MANAGER_HANDLER_IMPL_H_
#define DANCEX11_LOCALITY_MANAGER_HANDLER_IMPL_H_

#pragma once

#include "dancex11/core/dancex11_deploymenthandlersC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11_locality_handler_export.h"
#include "locality_activator_impl.h"

namespace DAnCEX11
{
  class DAnCE_LocalityActivator_i;

  class DAnCE_Locality_Handler_Export Locality_Handler_i
    : public virtual IDL::traits<DAnCEX11::InstanceDeploymentHandler>::base_type
  {
  public:
    // Constructor
    Locality_Handler_i () = default;

    // Destructor
    ~Locality_Handler_i () override = default;

    std::string instance_type () override;

    CORBA::StringSeq dependencies () override;

    void install_instance (const ::Deployment::DeploymentPlan & plan,
                           uint32_t instanceRef,
                           CORBA::Any& instance_reference) override;

    void activate_instance (const ::Deployment::DeploymentPlan & ,
                            uint32_t ,
                            const CORBA::Any &) override {};
    void passivate_instance (const ::Deployment::DeploymentPlan & ,
                             uint32_t ,
                             const CORBA::Any &) override {};

    void remove_instance (const ::Deployment::DeploymentPlan & plan,
                          uint32_t instanceRef,
                          const CORBA::Any & instance_reference) override;

    void provide_endpoint_reference (const ::Deployment::DeploymentPlan &,
                                     uint32_t,
                                     CORBA::Any&) override {};

    void connect_instance (const ::Deployment::DeploymentPlan & plan,
                           uint32_t connectionRef,
                           const CORBA::Any & provided_reference) override;

    void disconnect_instance (const ::Deployment::DeploymentPlan & plan,
                              uint32_t connectionRef) override;

    void instance_configured (const ::Deployment::DeploymentPlan &,
                              uint32_t) override {};

    void configure(const Deployment::Properties&) override;

    void close () override;

  private:
    static const std::string instance_type_;
    CORBA::servant_reference< DAnCE_LocalityActivator_i> activator_;
  };
}

extern "C"
{
  void
  DAnCE_Locality_Handler_Export create_DAnCEX11_Locality_Handler (
    IDL::traits<::DAnCEX11::InstanceDeploymentHandler>::ref_type& handler);
}

#endif /* DANCEX11_LOCALITY)MANAGER_HANDLER_IMPL_H_ */
