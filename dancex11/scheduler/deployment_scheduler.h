/**
 * @file    deployment_scheduler.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_DEPLOYMENT_SCHEDULER_H
#define DAnCEX11_DEPLOYMENT_SCHEDULER_H

#include "ace/Task.h"
#include "ace/Activation_Queue.h"
#include "dancex11/scheduler/deployment_event.h"

namespace DAnCEX11
{
  class Deployment_Scheduler_Export Deployment_Scheduler final
    : public ACE_Task_Base
  {
  public:
    Deployment_Scheduler () = default;
    virtual ~Deployment_Scheduler () = default;

    /// Schedule an event for execution
    int schedule_event (Deployment_Event *event);

    void activate_scheduler (size_t threads);

    void terminate_scheduler ();

    bool multithreaded () const;

    bool work_pending ();

    bool perform_work ();

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Deployment_Scheduler (const Deployment_Scheduler&) = delete;
    Deployment_Scheduler (Deployment_Scheduler&&) = delete;
    Deployment_Scheduler& operator= (const Deployment_Scheduler& x) = delete;
    Deployment_Scheduler& operator= (Deployment_Scheduler&& x) = delete;
    //@}
  protected:
    int svc () override;

  private:
    bool multithread_ {false};

    ACE_Activation_Queue event_queue_;
  };
}

#endif /* DAnCEX11_DEPLOYMENT_SCHEDULER_H */
