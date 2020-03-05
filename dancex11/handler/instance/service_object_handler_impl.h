// -*- C++ -*-
/**
 * @file    service_object_handler_impl.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_SERVICE_OBJECT_HANDLER_H
#define DAnCEX11_SERVICE_OBJECT_HANDLER_H

#pragma once

#include "dancex11/core/dancex11_deploymenthandlersC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11_deployment_handler_export.h"

namespace DAnCEX11
{
  class DAnCE_Deployment_Handler_Export Service_Object_Handler_Impl final
    : public IDL::traits<DAnCEX11::InstanceDeploymentHandler>::base_type
  {
  public:
    Service_Object_Handler_Impl (Plugin_Manager&);

    virtual ~Service_Object_Handler_Impl () = default;

    std::string instance_type () override;

    CORBA::StringSeq dependencies () override;

    void
    install_instance (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      CORBA::Any& instance_reference) override;

    void
    activate_instance (const Deployment::DeploymentPlan & ,
      uint32_t ,
      const CORBA::Any &) override {};

    void
    passivate_instance (const Deployment::DeploymentPlan & ,
      uint32_t ,
      const CORBA::Any &) override {};

    void
    remove_instance (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & instance_reference) override;

    void
    provide_endpoint_reference (const Deployment::DeploymentPlan &,
      uint32_t,
      CORBA::Any&) override {};

    void
    connect_instance (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & provided_reference) override;

    void
    disconnect_instance (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef) override;

    void
    instance_configured (const Deployment::DeploymentPlan &,
      uint32_t) override {};

    void
    configure (const Deployment::Properties&) override;

    void
    close () override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Service_Object_Handler_Impl (const Service_Object_Handler_Impl&) = delete;
    Service_Object_Handler_Impl (Service_Object_Handler_Impl&&) = delete;
    Service_Object_Handler_Impl& operator= (const Service_Object_Handler_Impl& x) = delete;
    Service_Object_Handler_Impl& operator= (Service_Object_Handler_Impl&& x) = delete;
    //@}

  private:
    Plugin_Manager& plugins_;
  };
}

#endif /* DAnCEX11_SERVICE_OBJECT_HANDLER_H */
