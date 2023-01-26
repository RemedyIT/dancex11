/**
 * @file    standard_error.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @brief   Interceptors which implement a sandard (first failure) error handling mechanism.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_INTERCEPTORS_STANDARD_ERROR_H
#define DAnCEX11_INTERCEPTORS_STANDARD_ERROR_H

#pragma once

#include "dancex11/core/dancex11_deploymentinterceptors_base_impl.h"
#include "dance_error_interceptors_export.h"

namespace DAnCEX11
{
  class DAnCE_Error_Interceptors_Export Standard_Error final
    : public DAnCEX11::DeploymentInterceptor_Base
  {
  public:
    Standard_Error () = default;

    ~Standard_Error () override = default;

    void
    post_install (const Deployment::DeploymentPlan &plan,
      uint32_t index,
      const CORBA::Any &reference,
      const CORBA::Any &exception_thrown) override;

    void
    post_connect (const Deployment::DeploymentPlan &plan,
      uint32_t connectionRef,
      const CORBA::Any &exceptionThrown) override;

    void
    post_configured (const Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      const CORBA::Any &exception_thrown) override;

    void
    post_activate (const Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      const CORBA::Any &exception_thrown) override;

    void
    post_passivate (const Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      const CORBA::Any &exception_thrown) override;

    void
    post_remove (const Deployment::DeploymentPlan &plan,
      uint32_t instanceRef,
      const CORBA::Any &exception_thrown) override;

    void
    configure (const Deployment::Properties&) override;
  private:
    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Standard_Error (const Standard_Error&) = delete;
    Standard_Error (Standard_Error&&) = delete;
    Standard_Error& operator= (const Standard_Error& x) = delete;
    Standard_Error& operator= (Standard_Error&& x) = delete;
    //@}
  };
}

extern "C"
{
  void
  DAnCE_Error_Interceptors_Export create_DAnCEX11_Standard_Error (
    IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type& plugin);
}

#endif /* DAnCEX11_INTERCEPTORS_STANDARD_ERROR_H */
