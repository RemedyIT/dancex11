/**
 * @file    start.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_START_H
#define DAnCEX11_EVENT_START_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Start_Instance
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'starting instance'
   */
  class Deployment_Scheduler_Export Start_Instance final
    : public Action_Base
  {
  public:
    Start_Instance (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t instanceRef,
        const std::string &inst_type,
        Event_Future holder);

    virtual ~Start_Instance () = default;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Start_Instance () = delete;
    Start_Instance (const Start_Instance&) = delete;
    Start_Instance (Start_Instance&&) = delete;
    Start_Instance& operator= (const Start_Instance& x) = delete;
    Start_Instance& operator= (Start_Instance&& x) = delete;
    //@}
  protected:
    virtual void
    invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    virtual void
    invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type) override;

    virtual void
    invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

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

#endif /* DAnCEX11_EVENT_START_H */
