/**
 * @file    passivate.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_PASSIVATE_H
#define DAnCEX11_EVENT_PASSIVATE_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Passivate_Instance
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'passivating instances'
   */
  class Deployment_Scheduler_Export Passivate_Instance final
    : public Action_Base
  {
  public:
    Passivate_Instance (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t instanceRef,
        const std::string &inst_type,
        Event_Future holder);

    virtual ~Passivate_Instance () = default;

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

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Passivate_Instance () = delete;
    Passivate_Instance (const Passivate_Instance&) = delete;
    Passivate_Instance (Passivate_Instance&&) = delete;
    Passivate_Instance& operator= (const Passivate_Instance& x) = delete;
    Passivate_Instance& operator= (Passivate_Instance&& x) = delete;
    //@}
  private:
    Deployment::DeploymentPlan &plan_;
    uint32_t instanceRef_;
  };
}

#endif /* DAnCEX11_EVENT_PASSIVATE_H */
