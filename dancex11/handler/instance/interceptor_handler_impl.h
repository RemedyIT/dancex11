// -*- C++ -*-
/**
 * @file    interceptor_handler_impl.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @brief   Installation handler implementation for spawning LocalityManagers.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DAnCEX11_INTERCEPTOR_HANDLER_H
#define DAnCEX11_INTERCEPTOR_HANDLER_H

#pragma once

#include "dancex11/core/dancex11_deploymenthandlersC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11_deployment_handler_export.h"

namespace DAnCEX11
{
  class DAnCE_Deployment_Handler_Export Interceptor_Handler_Impl final
    : public DAnCEX11::InstanceDeploymentHandler
  {
  public:
    Interceptor_Handler_Impl (Plugin_Manager&);

    virtual ~Interceptor_Handler_Impl ();

    virtual std::string
    instance_type () override;

    virtual CORBA::StringSeq
    dependencies () override;

    virtual void
    install_instance (const Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      CORBA::Any& instance_reference) override;

    virtual void
    activate_instance (const Deployment::DeploymentPlan & ,
      uint32_t ,
      const CORBA::Any &) override {};

    virtual void
    passivate_instance (const Deployment::DeploymentPlan & ,
      uint32_t ,
      const CORBA::Any &) override {};

    virtual void
    remove_instance (const Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      const CORBA::Any & instance_reference) override;

    virtual void
    provide_endpoint_reference (const Deployment::DeploymentPlan &,
      uint32_t,
      CORBA::Any&) override {};

    virtual void
    connect_instance (const Deployment::DeploymentPlan &plan,
      uint32_t connectionRef,
      const CORBA::Any & provided_reference) override;

    virtual void
    disconnect_instance (const Deployment::DeploymentPlan &plan,
      uint32_t connectionRef) override;

    virtual void
    instance_configured (const Deployment::DeploymentPlan &,
      uint32_t) override {};

    virtual void
    configure (const Deployment::Properties &) override;

    virtual void
    close () override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Interceptor_Handler_Impl (const Interceptor_Handler_Impl&) = delete;
    Interceptor_Handler_Impl (Interceptor_Handler_Impl&&) = delete;
    Interceptor_Handler_Impl& operator= (const Interceptor_Handler_Impl& x) = delete;
    Interceptor_Handler_Impl& operator= (Interceptor_Handler_Impl&& x) = delete;
    //@}

  private:
    Plugin_Manager& plugins_;
  };
}

#endif /* DAnCE_INTERCEPTOR_HANDLER_H */
