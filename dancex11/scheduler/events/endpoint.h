/**
 * @file    endpoint.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_ENDPOINT_H
#define DAnCEX11_EVENT_ENDPOINT_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Endpoint_Reference
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'endpoint references'
   */
  class Deployment_Scheduler_Export Endpoint_Reference final
    : public Action_Base
  {
  public:
    Endpoint_Reference (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t connectionRef,
        const std::string &inst_type,
        Event_Future holder);

    virtual ~Endpoint_Reference () = default;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Endpoint_Reference () = delete;
    Endpoint_Reference (const Endpoint_Reference&) = delete;
    Endpoint_Reference (Endpoint_Reference&&) = delete;
    Endpoint_Reference& operator= (const Endpoint_Reference& x) = delete;
    Endpoint_Reference& operator= (Endpoint_Reference&& x) = delete;
    //@}
  protected:
    void invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    void invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type) override;

    void invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    void create_unexpected_exception (const std::string &,
      const std::string &) override;

    void create_valid_result (Event_Result &) override;

  private:
    Deployment::DeploymentPlan &plan_;
    uint32_t connectionRef_;
    CORBA::Any ref_;
  };
}

#endif /* DAnCEX11_EVENT_ENDPOINT_H */
