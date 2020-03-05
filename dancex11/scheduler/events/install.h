/**
 * @file    install.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_EVENT_INSTALL_H
#define DAnCEX11_EVENT_INSTALL_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/events/action_base.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  /**
   * @class Install_Instance
   * @brief Invokes the interceptor points on the interceptors
   * Manages the interceptors for 'installing instances'
   */
  class Deployment_Scheduler_Export Install_Instance final
    : public Action_Base
  {
  public:
    Install_Instance (
        Plugin_Manager& plugins,
        Deployment::DeploymentPlan &plan,
        uint32_t instanceRef,
        const std::string &instance_type,
        Event_Future holder);

    virtual ~Install_Instance () = default;

  protected:
    void invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    void invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type) override;

    void invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) override;

    void create_unexpected_exception (const std::string &,
      const std::string &) override;

    void create_valid_result (Event_Result &) override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Install_Instance () = delete;
    Install_Instance (const Install_Instance&) = delete;
    Install_Instance (Install_Instance&&) = delete;
    Install_Instance& operator= (const Install_Instance& x) = delete;
    Install_Instance& operator= (Install_Instance&& x) = delete;
    //@}
  private:
    Deployment::DeploymentPlan &plan_;
    uint32_t instanceRef_;
    CORBA::Any instance_;
  };
}

#endif /* DAnCEX11_EVENT_INSTALL_H */
