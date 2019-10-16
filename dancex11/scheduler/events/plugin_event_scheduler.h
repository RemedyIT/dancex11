/**
 * @file    plugin_event_scheduler.h
 * @author  Martin Corino
 *
 * @brief   Template class for scheduling events corresponding
 *          to a specific Plugin_Manager instance.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef PLUGIN_EVENT_SCHEDULER_H_
#define PLUGIN_EVENT_SCHEDULER_H_

#pragma once

#include "ace/OS_Memory.h"
#include "ace/CORBA_macros.h"

#include "dancex11/scheduler/deployment_scheduler.h"
#include "dancex11/scheduler/events/plugin_manager.h"

#include <memory>

namespace DAnCEX11
{
  template <typename SCHEDULER>
  class Plugin_Event_Scheduler_T
  {
  public:
    Plugin_Event_Scheduler_T (std::shared_ptr<Plugin_Manager> plugins)
      : plugins_ (std::move(plugins)) {}
    ~Plugin_Event_Scheduler_T () = default;

    /// Schedule an event for execution
    template <typename EVENT, typename ...Args>
    int schedule_event (Args&&...args)
    {
      Deployment_Event* event {};
      ACE_NEW_THROW_EX (event,
                        EVENT (*this->plugins_,
                               std::forward<Args> (args)...),
                        CORBA::NO_MEMORY ());
      return this->scheduler_.schedule_event (event);
    }

    void activate_scheduler (size_t threads)
    { this->scheduler_.activate_scheduler (threads); }

    void terminate_scheduler ()
    { this->scheduler_.terminate_scheduler (); }

    bool multithreaded () const
    { return this->scheduler_.multithreaded (); }

    bool work_pending ()
    { return this->scheduler_.work_pending (); }

    bool perform_work ()
    { return this->scheduler_.perform_work (); }

    SCHEDULER& operator *()
    { return this->scheduler_; }

    Plugin_Manager& plugins ()
    { return *this->plugins_; }

  private:
    SCHEDULER scheduler_;
    std::shared_ptr<Plugin_Manager> plugins_;
  };
}

#endif /* PLUGIN_EVENT_SCHEDULER_H_ */
