/**
 * @file    deployment_scheduler.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "deployment_scheduler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  int
  Deployment_Scheduler::schedule_event (Deployment_Event *event)
  {
    int const retval = this->event_queue_.enqueue (event);

    if (retval == -1)
      {
        DANCEX11_LOG_ERROR ("Deployment_Scheduler::schedule_event - " <<
                            "Error: Unable to schedule event for execution");
      }

    return retval;
  }

  void
  Deployment_Scheduler::terminate_scheduler ()
  {
    while (!this->event_queue_.is_empty ())
      {
        // Remove every method request from the list.
        std::unique_ptr< ACE_Method_Request > de (
          this->event_queue_.dequeue ());
        X11_UNUSED_ARG (de);
      }
    this->event_queue_.queue ()->close ();
  }

  int
  Deployment_Scheduler::svc ()
  {
    // Not started since the scheduler is always
    // started, using 0 threads (see the activate_scheduler
    // method)
    try
      {
        for ( ; ; )
          {
            if (!this->perform_work ())
              {
                break;
              }
          }
      }
    catch (const std::exception&)
      {
        // todo
      }

    return 0;
  }

  bool
  Deployment_Scheduler::perform_work ()
  {
    std::unique_ptr< ACE_Method_Request > de (this->event_queue_.dequeue ());

    if (de != nullptr)
      {
        DANCEX11_LOG_DEBUG ("Deployment_Scheduler::perform_work - " <<
                            "Invoking a deployment event");
        de->call ();
        return true;
      }
    return false;
  }

  void
  Deployment_Scheduler::activate_scheduler (size_t threads)
  {
    this->multithread_ = threads > 0;
    if (this->multithread_)
      this->activate (
          THR_DETACHED,
          ACE_Utils::truncate_cast<int> (threads));
  }

  bool
  Deployment_Scheduler::multithreaded () const
  {
    return this->multithread_;
  }

  bool
  Deployment_Scheduler::work_pending ()
  {
    return !this->event_queue_.is_empty ();
  }
}
