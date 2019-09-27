/**
 * @file    connect.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DAnCEX11_EVENT_CONNECT_H
#define DAnCEX11_EVENT_CONNECT_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Connect_Instance
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'instance connected'
   */
  class Deployment_Scheduler_Export Connect_Instance final
    : public Action_Base
  {
  public:
    Connect_Instance (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t connectionRef,
        const CORBA::Any &provided_ref,
        const std::string &instance_type,
        Event_Future holder);

    virtual ~Connect_Instance () = default;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Connect_Instance () = delete;
    Connect_Instance (const Connect_Instance&) = delete;
    Connect_Instance (Connect_Instance&&) = delete;
    Connect_Instance& operator= (const Connect_Instance& x) = delete;
    Connect_Instance& operator= (Connect_Instance&& x) = delete;
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
    CORBA::Any provided_ref_;
  };
}

#endif /* DAnCEX11_EVENT_CONNECT_H */
