/**
 * @file    endpoint.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "endpoint.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Endpoint_Reference::Endpoint_Reference (
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
  Endpoint_Reference::invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type)
  {
    DANCEX11_LOG_TRACE ("Endpoint_Reference::invoke_pre_interceptor");
    // no-op
  }

  void
  Endpoint_Reference::invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler)
  {
    DANCEX11_LOG_TRACE ("Endpoint_Reference::invoke");

    DANCEX11_LOG_DEBUG ("Endpoint_Reference::invoke - Invoking provide_endpoint_reference on handler");

    handler->provide_endpoint_reference (this->plan_,
                                         this->connectionRef_,
                                         this->ref_);
    DANCEX11_LOG_DEBUG ("Endpoint_Reference::invoke - provide_endpoint_reference successful completed");
  }


  void
  Endpoint_Reference::invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di)
  {
    DANCEX11_LOG_TRACE ("Endpoint_Reference::invoke_post_interceptor");

    di->post_endpoint_reference (this->plan_,
                                this->connectionRef_,
                                this->ref_,
                                this->instance_excep_);
  }


  void
  Endpoint_Reference::create_unexpected_exception (const std::string &name,
    const std::string &reason)
  {
    DANCEX11_LOG_TRACE ("Endpoint_Reference::create_unexpected_exception");

    Deployment::StartError ex_tmp (name, reason);
    this->instance_excep_ =
      DAnCEX11::Utility::create_any_from_exception (ex_tmp);
  }

  void
  Endpoint_Reference::create_valid_result (Event_Result &result)
  {
    DANCEX11_LOG_TRACE ("Endpoint_Reference::create_valid_result");
    result.contents_ = this->ref_;
    // no-op
  }
}
