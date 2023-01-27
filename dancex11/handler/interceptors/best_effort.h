/**
 * @file    best_effort.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @brief   Interceptors which implement a Best Effort error handling mechanism.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_INTERCEPTORS_BEST_EFFORT_H
#define DAnCEX11_INTERCEPTORS_BEST_EFFORT_H

#pragma once

#include "dancex11/core/dancex11_deploymentinterceptors_base_impl.h"
#include "dance_error_interceptors_export.h"

namespace DAnCEX11
{
  class DAnCE_Error_Interceptors_Export Best_Effort final
    : public DAnCEX11::DeploymentInterceptor_Base
  {
  public:
    Best_Effort () = default;

    ~Best_Effort () override = default;

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
    Best_Effort (const Best_Effort&) = delete;
    Best_Effort (Best_Effort&&) = delete;
    Best_Effort& operator= (const Best_Effort& x) = delete;
    Best_Effort& operator= (Best_Effort&& x) = delete;
    //@}
  };
}

extern "C"
{
  void
  DAnCE_Error_Interceptors_Export create_DAnCEX11_Best_Effort (
    IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type& plugin);
}

#endif /* DAnCEX11_INTERCEPTORS_BEST_EFFORT_H */
