// -*- C++ -*-
/**
 * @file    inst_handler_impl.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_INST_HANDLER_H
#define DAnCEX11_INST_HANDLER_H

#pragma once

#include "dancex11/core/dancex11_deploymenthandlersC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11_deployment_handler_export.h"

namespace DAnCEX11
{
  class DAnCE_Deployment_Handler_Export Inst_Handler_Impl final
    : public DAnCEX11::InstanceDeploymentHandler
  {
  public:
    Inst_Handler_Impl (Plugin_Manager&);

    virtual ~Inst_Handler_Impl () = default;

    virtual std::string
    instance_type () override;

    virtual CORBA::StringSeq
    dependencies () override;

    virtual void
    install_instance (const Deployment::DeploymentPlan & plan,
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
    remove_instance (const Deployment::DeploymentPlan & plan,
      uint32_t instanceRef,
      const CORBA::Any & instance_reference) override;

    virtual void
    provide_endpoint_reference (const Deployment::DeploymentPlan &,
      uint32_t,
      CORBA::Any&) override {};

    virtual void
    connect_instance (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any & provided_reference) override;

    virtual void
    disconnect_instance (const Deployment::DeploymentPlan & plan,
      uint32_t connectionRef) override;

    virtual void
    instance_configured (const Deployment::DeploymentPlan &,
      uint32_t) override {};

    virtual void
    configure(const Deployment::Properties&) override;

    virtual void
    close () override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Inst_Handler_Impl (const Inst_Handler_Impl&) = delete;
    Inst_Handler_Impl (Inst_Handler_Impl&&) = delete;
    Inst_Handler_Impl& operator= (const Inst_Handler_Impl& x) = delete;
    Inst_Handler_Impl& operator= (Inst_Handler_Impl&& x) = delete;
    //@}

  private:
    Plugin_Manager& plugins_;
  };
}

#endif /* DAnCEX11_INST_HANDLER_H */
