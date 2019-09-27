// -*- C++ -*-
/**
 * @file    dancex11_deploymentinterceptors_base_impl.h
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
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

    virtual ~DeploymentInterceptor_Base ();

    virtual void
    configure (const Deployment::Properties & config) override;

    virtual void
    preprocess_plan (Deployment::DeploymentPlan & plan) override;

    virtual void
    pre_install (Deployment::DeploymentPlan & plan,
      uint32_t instanceRef) override;

    virtual void
    post_install (const Deployment::DeploymentPlan & plan,
      uint32_t index,
      const CORBA::Any & reference,
      const CORBA::Any & exception_thrown) override;

    virtual void
    post_endpoint_reference (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & endpoint_reference,
      const CORBA::Any & exception_thrown) override;

    virtual void
    pre_connect (Deployment::DeploymentPlan & plan,
      uint32_t connection_index,
      CORBA::Any & provided_reference) override;

    virtual void
    post_connect (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & exceptionThrown) override;

    virtual void
    pre_disconnect (Deployment::DeploymentPlan & plan,
      uint32_t connection_index) override;

    virtual void
    post_disconnect (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & exceptionThrown) override;

    virtual void
    post_configured (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    virtual void
    post_activate (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    virtual void
    post_passivate (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    virtual void
    post_remove (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown) override;

    virtual void
    unexpected_event (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & exception_thrown,
      const std::string& error) override;

    virtual void close () override;
  };
}


#endif /* DANCEX11_DEPLOYMENTINTERCEPTORSI_H_ */
