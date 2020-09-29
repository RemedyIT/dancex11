/**
 * @file    deployment_orb_wait_strategy.h
 * @author  Martin Corino
 *
 * @brief   Deployment event completion wait strategy for
 *          async CORBA requests.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DEPLOYMENT_ORB_WAIT_STRATEGY_H_
#define DEPLOYMENT_ORB_WAIT_STRATEGY_H_

#pragma once

#include "dancex11/scheduler/completion_base.h"

namespace DAnCEX11
{
  template <typename RESULT>
  class ORB_Completion_Wait_Strategy final
  {
  public:
    using result_type = RESULT;
    using completion_type = Completion_T<ORB_Completion_Wait_Strategy, result_type>;

    ORB_Completion_Wait_Strategy (
        completion_type &counter,
        bool multithread = false)
      : counter_ (counter),
        multithread_ (multithread)
    {}
    ~ORB_Completion_Wait_Strategy () = default;

    /// Waits for completion of all events, with a timeout
    bool
    wait_on_completion (ACE_Time_Value *tv);

  private:

    bool
    single_threaded_wait_on_completion (ACE_Time_Value *tv);

    completion_type &counter_;
    bool multithread_ {};
  };
}

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "deployment_orb_wait_strategy.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("deployment_orb_wait_strategy.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

#endif /* DEPLOYMENT_ORB_WAIT_STRATEGY_H_ */
