/**
 * @file    disconnect.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_DISCONNECT_H
#define DAnCEX11_EVENT_DISCONNECT_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Disconnect_Instance
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'disconnect instance'
   */
  class Deployment_Scheduler_Export Disconnect_Instance final
    : public Action_Base
  {
  public:
    Disconnect_Instance (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t connectionRef,
        const std::string &inst_type,
        Event_Future holder);

    virtual ~Disconnect_Instance () = default;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Disconnect_Instance () = delete;
    Disconnect_Instance (const Disconnect_Instance&) = delete;
    Disconnect_Instance (Disconnect_Instance&&) = delete;
    Disconnect_Instance& operator= (const Disconnect_Instance& x) = delete;
    Disconnect_Instance& operator= (Disconnect_Instance&& x) = delete;
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
    uint32_t connectionRef_;
  };
}

#endif /* DAnCEX11_EVENT_DISCONNECT_H */
