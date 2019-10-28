/**
 * @file    connect.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "connect.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Connect_Instance::Connect_Instance (
      Plugin_Manager& plugins,
      Deployment::DeploymentPlan & plan,
      uint32_t connectionRef,
      const CORBA::Any &provided_ref,
      const std::string& inst_type,
      Event_Future holder)
    : Action_Base (plugins,
                   holder,
                   plan.connection ()[connectionRef].name (),
                   inst_type)
    , plan_ (plan)
    , connectionRef_ (connectionRef)
    , provided_ref_ (provided_ref)
  {
  }

  void
  Connect_Instance::invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Connect_Instance::invoke_pre_interceptor");

    di->pre_connect (this->plan_,
                    this->connectionRef_,
                    this->provided_ref_);
  }

  void
  Connect_Instance::invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler)
  {
    DANCEX11_LOG_TRACE ("Connect_Instance::invoke");

    DANCEX11_LOG_DEBUG ("Connect_Instance::invoke - Invoking connect_instance on handler");

    handler->connect_instance (this->plan_,
                               this->connectionRef_,
                               this->provided_ref_);

    DANCEX11_LOG_DEBUG ("Connect_Instance::invoke - connect_instance successful completed");
  }

  void
  Connect_Instance::invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Connect_Instance::invoke_post_interceptor");

    di->post_connect (this->plan_,
                      this->connectionRef_,
                      this->instance_excep_);
  }

  void
  Connect_Instance::create_unexpected_exception (const std::string &name,
    const std::string &reason)
  {
    DANCEX11_LOG_TRACE ("Connect_Instance::create_unexpected_exception");

    ::Deployment::InvalidConnection ex_tmp (name, reason);
    this->instance_excep_ =
      DAnCEX11::Utility::create_any_from_exception (ex_tmp);
  }

  void
  Connect_Instance::create_valid_result (Event_Result &)
  {
    DANCEX11_LOG_TRACE ("Connect_Instance::create_valid_result");
    // no-op
  }
}
