/**
 * @file    action_base.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_ACTION_BASE_H
#define DAnCEX11_ACTION_BASE_H

#include "dancex11/scheduler/deployment_event.h"
#include "dancex11/scheduler/events/plugin_manager.h"

namespace DAnCEX11
{
  /**
   * @class Action_Base
   * @brief Base class for all scheduled events in the LM.
   */
  class Deployment_Scheduler_Export Action_Base
    : public Deployment_Event
  {
  public:
    Action_Base (
        Plugin_Manager& plugins,
        Event_Future holder,
        const std::string &name,
        const std::string &instance_type);

    virtual ~Action_Base () = default;

    virtual int
    call () override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Action_Base () = delete;
    Action_Base (const Action_Base&) = delete;
    Action_Base (Action_Base&&) = delete;
    Action_Base& operator= (const Action_Base& x) = delete;
    Action_Base& operator= (Action_Base&& x) = delete;
    //@}
  protected:
    virtual void
    invoke_pre_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) = 0;

    virtual void
    invoke (IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type) = 0;

    virtual void
    invoke_post_interceptor (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type) = 0;

    virtual void
    create_unexpected_exception (const std::string &,
      const std::string &) = 0;

    virtual void
    create_valid_result (Event_Result &) = 0;

    Plugin_Manager& plugins_;
    CORBA::Any instance_excep_;
  };
}

#endif /* DAnCEX11_ACTION_BASE_H */
