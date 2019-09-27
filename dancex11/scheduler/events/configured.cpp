/**
 * @file    configured.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#include "configured.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Instance_Configured::Instance_Configured (
      Plugin_Manager& plugins,
      Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      const std::string &inst_type,
      Event_Future holder)
    : Action_Base (plugins,
                   holder,
                   plan.instance ()[instanceRef].name (),
                   inst_type)
    , plan_ (plan)
    , instanceRef_ (instanceRef)
  {
  }

  void
  Instance_Configured::invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type)
  {
    DANCEX11_LOG_TRACE ("Instance_Configured::invoke_pre_interceptor");

    //no-op
  }

  void
  Instance_Configured::invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler)
  {
    DANCEX11_LOG_TRACE ("Instance_Configured::invoke");

    DANCEX11_LOG_DEBUG ("Instance_Configured::invoke - Invoking instance_configured " <<
                        "on handler for type <" << this->instance_type_ << ">");
    handler->instance_configured (this->plan_,
                                  this->instanceRef_);
    DANCEX11_LOG_DEBUG ("Instance_Configured::invoke - instance_configured completed");
  }

  void
  Instance_Configured::invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Instance_Configured::invoke_post_interceptor");

    di->post_configured (this->plan_,
                         this->instanceRef_,
                         this->instance_excep_);
  }

  void
  Instance_Configured::create_unexpected_exception (const std::string &name,
    const std::string &reason)
  {
    DANCEX11_LOG_TRACE ("Instance_Configured::create_unexpected_exception");

    Deployment::StartError ex_tmp (name, reason);
    this->instance_excep_ =
      DAnCEX11::Utility::create_any_from_exception (ex_tmp);
  }

  void
  Instance_Configured::create_valid_result (Event_Result &)
  {
    DANCEX11_LOG_TRACE ("Instance_Configured::create_valid_result");
    // no-op
  }
}
