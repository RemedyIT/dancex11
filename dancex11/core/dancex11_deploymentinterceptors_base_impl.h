// -*- C++ -*-
/**
 * @file    dancex11_deploymentinterceptors_base_impl.h
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_DEPLOYMENTINTERCEPTORSI_H_
#define DANCEX11_DEPLOYMENTINTERCEPTORSI_H_

#pragma once

#include "dancex11/core/dancex11_deploymentinterceptorsC.h"
#include "dancex11_stub_export.h"

namespace DAnCEX11
{
  /**
   * @class DAnCEX11_DeploymentInterceptor_Base
   * @brief Base, no-op interceptor implementation.
   */
  class DANCEX11_STUB_Export DeploymentInterceptor_Base
    : public IDL::traits<DAnCEX11::DeploymentInterceptor>::base_type
  {
  public:
    DeploymentInterceptor_Base ();

    ~DeploymentInterceptor_Base () override;

    void configure (const Deployment::Properties & config) override;

    void preprocess_plan (Deployment::DeploymentPlan & plan) override;

    void
    pre_install (Deployment::DeploymentPlan & plan,
      uint32_t instanceRef) override;

    void
    post_install (const Deployment::DeploymentPlan & plan,
      uint32_t index,
      const CORBA::Any & reference,
      const CORBA::Any & exception_thrown) override;

    void
    post_endpoint_reference (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & endpoint_reference,
      const CORBA::Any & exception_thrown) override;

    void
    pre_connect (Deployment::DeploymentPlan & plan,
      uint32_t connection_index,
      CORBA::Any & provided_reference) override;

    void
    post_connect (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & exceptionThrown) override;

    void
    pre_disconnect (Deployment::DeploymentPlan & plan,
      uint32_t connection_index) override;

    void
    post_disconnect (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & exceptionThrown) override;

    void
    post_configured (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    void
    post_activate (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    void
    post_passivate (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    void
    post_remove (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    void
    unexpected_event (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown,
      const std::string& error) override;

    void close () override;
  };
}


#endif /* DANCEX11_DEPLOYMENTINTERCEPTORSI_H_ */
