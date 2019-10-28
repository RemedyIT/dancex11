/**
 * @file    disconnect.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "disconnect.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Disconnect_Instance::Disconnect_Instance (
      Plugin_Manager& plugins,
      Deployment::DeploymentPlan &plan,
      uint32_t connectionRef,
      const std::string &inst_type,
      Event_Future holder)
    : Action_Base (plugins,
                   holder,
                   plan.connection ()[connectionRef].name (),
                   inst_type)
    , plan_ (plan)
    , connectionRef_ (connectionRef)
  {
  }

  void
  Disconnect_Instance::invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Disconnect_Instance::invoke_pre_interceptor");

    di->pre_disconnect (this->plan_, this->connectionRef_);
  }

  void
  Disconnect_Instance::invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler)
  {
    DANCEX11_LOG_TRACE ("Disconnect_Instance::invoke");

    DANCEX11_LOG_DEBUG ("Disconnect_Instance::invoke - Invoking disconnect_instance on handler");

    handler->disconnect_instance (this->plan_, this->connectionRef_);

    DANCEX11_LOG_DEBUG ("Disconnect_Instance::invoke - disconnect_instance successful completed");
  }

  void
  Disconnect_Instance::invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Disconnect_Instance::invoke_post_interceptor");

    di->post_disconnect (this->plan_,
                        this->connectionRef_,
                        this->instance_excep_);
  }

  void
  Disconnect_Instance::create_unexpected_exception (const std::string &name,
    const std::string &reason)
  {
    DANCEX11_LOG_TRACE ("Disconnect_Instance::create_unexpected_exception");

    ::Deployment::InvalidConnection ex_tmp (name, reason);
    this->instance_excep_ =
      DAnCEX11::Utility::create_any_from_exception (ex_tmp);
  }

  void
  Disconnect_Instance::create_valid_result (Event_Result &)
  {
    DANCEX11_LOG_TRACE ("Disconnect_Instance::create_valid_result");
    // no-op
  }
}
