/**
 * @file    remove.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_REMOVE_H
#define DAnCEX11_EVENT_REMOVE_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Remove_Instance
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'removing instances'
   */
  class Deployment_Scheduler_Export Remove_Instance final
    : public Action_Base
  {
  public:
    Remove_Instance (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t instanceRef,
        const std::string &instance_type,
        Event_Future holder);

    virtual ~Remove_Instance () = default;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Remove_Instance () = delete;
    Remove_Instance (const Remove_Instance&) = delete;
    Remove_Instance (Remove_Instance&&) = delete;
    Remove_Instance& operator= (const Remove_Instance& x) = delete;
    Remove_Instance& operator= (Remove_Instance&& x) = delete;
    //@}
protected:
    void invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    void invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type) override;

    void invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    void create_unexpected_exception (const std::string &,
      const std::string &) override;

    void create_valid_result (Event_Result &) override;

  private:
    Deployment::DeploymentPlan & plan_;
    uint32_t instanceRef_;
  };
}

#endif /* DAnCEX11_EVENT_REMOVE_H */
