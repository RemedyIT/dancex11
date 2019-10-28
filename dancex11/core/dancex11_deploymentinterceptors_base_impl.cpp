// -*- C++ -*-
/**
 * @file    dancex11_deploymentinterceptors_base_impl.cpp
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "dancex11_deploymentinterceptors_base_impl.h"

namespace DAnCEX11
{
  DeploymentInterceptor_Base::DeploymentInterceptor_Base ()
  {
  }

  DeploymentInterceptor_Base::~DeploymentInterceptor_Base ()
  {
  }

  void
  DeploymentInterceptor_Base::configure (const ::Deployment::Properties &)
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::preprocess_plan (Deployment::DeploymentPlan &)
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::pre_install (Deployment::DeploymentPlan &,
    uint32_t)
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::post_install (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any & ,
    const CORBA::Any & )
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::post_endpoint_reference (const Deployment::DeploymentPlan & ,
    uint32_t ,
    const CORBA::Any & ,
    const CORBA::Any & )
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::pre_connect (Deployment::DeploymentPlan & ,
    uint32_t ,
    CORBA::Any & )
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::post_connect (const Deployment::DeploymentPlan & ,
    uint32_t ,
    const CORBA::Any & )
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::pre_disconnect (Deployment::DeploymentPlan &,
    uint32_t)
  {
    // Add your implementation here
  }

  void
  DeploymentInterceptor_Base::post_disconnect (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
    // Add your implementation here
  }

  void DeploymentInterceptor_Base::post_configured (const Deployment::DeploymentPlan & ,
    uint32_t ,
    const CORBA::Any &)
  {
    // Add your implementation here
  }

  void DeploymentInterceptor_Base::post_activate (const Deployment::DeploymentPlan & ,
    uint32_t ,
    const CORBA::Any & )
  {
    // Add your implementation here
  }

  void DeploymentInterceptor_Base::post_passivate (const Deployment::DeploymentPlan & ,
    uint32_t ,
    const CORBA::Any &)
  {
    // Add your implementation here
  }

  void DeploymentInterceptor_Base::post_remove (const Deployment::DeploymentPlan & ,
    uint32_t ,
    const CORBA::Any & )
  {
    // Add your implementation here
  }

  void DeploymentInterceptor_Base::unexpected_event (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &,
    const std::string&)
  {
  }

  void DeploymentInterceptor_Base::close ()
  {
  }
}
