/**
 * @file    passivate.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "passivate.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Passivate_Instance::Passivate_Instance (
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
  Passivate_Instance::invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type)
  {
    DANCEX11_LOG_TRACE ("Passivate_Instance::invoke_pre_interceptor");
    //no-op
  }

  void
  Passivate_Instance::invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler)
  {
    DANCEX11_LOG_TRACE ("Passivate_Instance::invoke");

    DANCEX11_LOG_DEBUG ("Passivate_Instance::invoke - " <<
                        "Invoking passivate_instance on handler for type <" << this->instance_type_ << ">");
    CORBA::Any any;
    handler->passivate_instance (this->plan_,
                                 this->instanceRef_,
                                 any);
    DANCEX11_LOG_DEBUG ("Passivate_Instance::invoke - passivate_instance completed");
  }

  void
  Passivate_Instance::invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Passivate_Instance::invoke_post_interceptor");

    di->post_passivate (this->plan_,
                        this->instanceRef_,
                        this->instance_excep_);
  }

  void
  Passivate_Instance::create_unexpected_exception (const std::string &name,
                                                   const std::string &reason)
  {
    DANCEX11_LOG_TRACE ("Passivate_Instance::create_unexpected_exception");

    ::Deployment::StopError ex_tmp (name, reason);
    this->instance_excep_ =
      DAnCEX11::Utility::create_any_from_exception (ex_tmp);
  }

  void
  Passivate_Instance::create_valid_result (Event_Result &)
  {
    DANCEX11_LOG_TRACE ("Passivate_Instance::create_valid_result");
    // no-op
  }
}
