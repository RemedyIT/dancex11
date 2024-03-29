/**
 * @file    deployment_event.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_DEPLOYMENT_EVENT_H
#define DAnCEX11_DEPLOYMENT_EVENT_H

#include "ace/Future.h"
#include "ace/Method_Request.h"
#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/core/dancex11_propertiesC.h"

#include <list>

namespace DAnCEX11
{
  struct Deployment_Scheduler_Export Event_Result
  {
    Event_Result () = default;
    Event_Result (const std::string &id,
                  bool exception);

    Event_Result (const std::string &id,
                  bool exception,
                  const CORBA::Any &any);

    std::string id_;
    bool exception_ {true};
    CORBA::Any contents_;
  };

  using Event_Future = ACE_Future<Event_Result>;

  using Event_List = std::list<Event_Future>;

  class Deployment_Scheduler_Export Deployment_Event
    : public virtual ACE_Method_Request
  {
  public:
    Deployment_Event (Event_Future holder,
      const std::string &name,
      const std::string &instance_type);

    Event_Future get_future ();

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Deployment_Event () = delete;
    Deployment_Event (const Deployment_Event&) = delete;
    Deployment_Event (Deployment_Event&&) = delete;
    Deployment_Event& operator= (const Deployment_Event& x) = delete;
    Deployment_Event& operator= (Deployment_Event&& x) = delete;
    //@}
  protected:
    Event_Future holder_;
    std::string const name_;
    std::string const instance_type_;
  };
}

#endif /* DAnCEX11_DEPLOYMENT_EVENT_H */
