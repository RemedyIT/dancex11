/**
 * @file    configured.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_CONFIGURED_H
#define DAnCEX11_EVENT_CONFIGURED_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Instance_Configured
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'instance configured'
   */
  class Deployment_Scheduler_Export Instance_Configured final
    : public Action_Base
  {
  public:
    Instance_Configured (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t instanceRef,
        const std::string &inst_type,
        Event_Future holder);

    virtual ~Instance_Configured ()= default;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Instance_Configured () = delete;
    Instance_Configured (const Instance_Configured&) = delete;
    Instance_Configured (Instance_Configured&&) = delete;
    Instance_Configured& operator= (const Instance_Configured& x) = delete;
    Instance_Configured& operator= (Instance_Configured&& x) = delete;
    //@}
  protected:
    virtual void
    invoke_pre_interceptor (IDL::traits<DeploymentInterceptor>::ref_type) override;

    virtual void
    invoke (IDL::traits<InstanceDeploymentHandler>::ref_type) override;

    virtual void
    invoke_post_interceptor (IDL::traits<DeploymentInterceptor>::ref_type) override;

    virtual void
    create_unexpected_exception (const std::string &,
      const std::string &) override;

    virtual void
    create_valid_result (Event_Result &) override;

  private:
    Deployment::DeploymentPlan &plan_;
    uint32_t instanceRef_;
  };
}

#endif /* DAnCEX11_EVENT_CONFIGURED_H */
