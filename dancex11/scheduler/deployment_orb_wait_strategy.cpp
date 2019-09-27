/**
 * @file    deployment_orb_wait_strategy.cpp
 * @author  Martin Corino
 *
 * @brief   Deployment event completion wait strategy for
 *          async CORBA requests.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DEPLOYMENT_ORB_WAIT_STRATEGY_CPP_
#define DEPLOYMENT_ORB_WAIT_STRATEGY_CPP_

#include "deployment_orb_wait_strategy.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_deployment_state.h"

namespace DAnCEX11
{
  template <typename RESULT>
  bool
  ORB_Completion_Wait_Strategy<RESULT>::wait_on_completion (ACE_Time_Value *tv)
  {
    if (!this->multithread_ )
    {
      return this->single_threaded_wait_on_completion (tv);
    }
    else
    {
      if (!this->counter_.wait_on_all_completed (tv))
      {
        DANCEX11_LOG_ERROR ("ORB_Completion_Wait_Strategy::"
                              "wait_on_completion - " <<
                            "Timed out waiting on event completion\n");
        return false; // timeout
      }

      DANCEX11_LOG_DEBUG ("ORB_Completion_Wait_Strategy::"
                            "wait_on_completion - " <<
                          "All events completed");
      return true;
    }
  }

  template <typename RESULT>
  bool
  ORB_Completion_Wait_Strategy<RESULT>::single_threaded_wait_on_completion (ACE_Time_Value *tv)
  {
    ACE_Time_Value timer = tv ? tv->to_relative_time () : ACE_Time_Value::zero;

    IDL::traits<CORBA::ORB>::ref_type orb = DAnCEX11::State::instance ()->orb ();

    // wait till all outstanding async CORBA operations have finished
    while (!this->counter_.all_completed ())
    {
      if (tv && timer == ACE_Time_Value::zero)
      {
        DANCEX11_LOG_ERROR ("ORB_Completion_Wait_Strategy::"
                              "single_threaded_wait_on_completion - " <<
                            "Timed out waiting on event completion\n");
        return false; // timeout
      }

      if (tv)
        orb->perform_work (timer);
      else
        orb->perform_work ();
    }

    DANCEX11_LOG_DEBUG ("ORB_Completion_Wait_Strategy::"
                          "single_threaded_wait_on_completion - " <<
                        "All events completed");
    return true;
  }

}

#endif /* DEPLOYMENT_ORB_WAIT_STRATEGY_CPP_ */
