/**
 * @file    remove.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#include "remove.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Remove_Instance::Remove_Instance (
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
  Remove_Instance::invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type)
  {
    DANCEX11_LOG_TRACE ("Remove_Instance::invoke_pre_interceptor");
    //no-op
  }

  void
  Remove_Instance::invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler)
  {
    DANCEX11_LOG_TRACE ("Remove_Instance::invoke");

    DANCEX11_LOG_DEBUG ("Remove_Instance::invoke - Invoking remove_instance on handler for type <" <<
                        this->instance_type_ << ">");
    CORBA::Any any;
    handler->remove_instance (this->plan_,
                              this->instanceRef_,
                              any);
    DANCEX11_LOG_DEBUG ("Remove_Instance::invoke - remove_instance completed");
  }

  void
  Remove_Instance::invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Remove_Instance::invoke_post_interceptor");

    di->post_remove (this->plan_,
                    this->instanceRef_,
                    this->instance_excep_);
  }

  void
  Remove_Instance::create_unexpected_exception (const std::string &name,
    const std::string &reason)
  {
    DANCEX11_LOG_TRACE ("Remove_Instance::create_unexpected_exception");

    ::Deployment::StopError ex_tmp (name, reason);
    this->instance_excep_ =
      DAnCEX11::Utility::create_any_from_exception (ex_tmp);
  }

  void
  Remove_Instance::create_valid_result (Event_Result &)
  {
    DANCEX11_LOG_TRACE ("Remove_Instance::create_valid_result");
    // no-op
  }
}
