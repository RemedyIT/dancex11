/**
 * @file    deployment_default_wait_strategy.cpp
 * @author  Martin Corino
 *
 * @brief   Default deployment event completion wait strategy.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "deployment_default_wait_strategy.h"
#include "deployment_completion.h"

#include "dancex11/logger/log.h"

#include "ace/Countdown_Time.h"

namespace DAnCEX11
{
  template <typename RESULT>
  bool
  Default_Completion_Wait_Strategy<RESULT>::wait_on_completion (ACE_Time_Value *tv)
  {
    if (!this->sched_.multithreaded ())
    {
      return this->single_threaded_wait_on_completion (tv);
    }
    else
    {
      bool const retval = this->counter_.wait_on_all_completed (tv);

      if (!retval)
      {
        DANCEX11_LOG_ERROR ("Default_Completion_Wait_Strategy::"
                              "wait_on_completion - " <<
                            "Timed out waiting on event completion\n");
        return false;
      }

      DANCEX11_LOG_DEBUG ("Default_Completion_Wait_Strategy::"
                            "wait_on_completion - " <<
                          "All events completed");
      return true;
    }
  }

  template <typename RESULT>
  bool
  Default_Completion_Wait_Strategy<RESULT>::single_threaded_wait_on_completion (ACE_Time_Value *tv)
  {
    ACE_Time_Value timer = tv ? tv->to_relative_time () : ACE_Time_Value::zero;
    ACE_Countdown_Time countdown (std::addressof(timer));

    while (this->sched_.work_pending ())
    {
      if (tv && timer == ACE_Time_Value::zero)
      {
        DANCEX11_LOG_ERROR ("Default_Completion_Wait_Strategy::"
                               "single_threaded_wait_on_completion - " <<
                            "Timed out waiting on event completion\n");
        return false; // timeout
      }

      countdown.start ();
      this->sched_.perform_work ();
      countdown.stop ();
    }
    DANCEX11_LOG_DEBUG ("Default_Completion_Wait_Strategy::"
                          "single_threaded_wait_on_completion - " <<
                        "All events completed");
    return true;
  }

}
