/**
 * @file    completion_base.h
 * @author  Martin Corino
 *
 * @brief   Base completion handler template.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef COMPLETION_BASE_H_
#define COMPLETION_BASE_H_

#pragma once

#include /**/ "ace/config-all.h"
#include "ace/Synch_Traits.h"
#include "ace/Null_Mutex.h"
#include "ace/Thread_Mutex.h"
#include "ace/CORBA_macros.h"

#include "ace/Future.h"
#include "ace/Method_Request.h"
#include "completion_counter_base.h"
#include "tao/x11/corba.h"

#include <list>

namespace DAnCEX11
{
  /**
   * @class Completion_T
   */
  template <typename WAIT_STRATEGY, typename RESULT>
  class Completion_T
    : public virtual ACE_Future_Observer< RESULT >
    , public virtual Completion_Counter_Base< ACE_SYNCH_MUTEX >
  {
  public:
    typedef RESULT result_type;
    typedef ACE_Future< RESULT > future_type;

    typedef std::list < future_type > future_list;

    template <typename ...Args>
    Completion_T (Args&& ...args)
      : Completion_Counter_Base< ACE_SYNCH_MUTEX > (0, 0)
      , mutex_ ()
      , condition_ (this->mutex_)
      , wait_strategy_ (*this, std::forward<Args> (args)...)
    {
    }

    virtual ~Completion_T () = default;

    void
    update (const future_type &future) override
    {
      ACE_GUARD_THROW_EX (ACE_SYNCH_MUTEX,
                          guard,
                          this->mutex_,
                          CORBA::NO_RESOURCES ());

      this->completed_events_.push_back (future);
      this->decrement_exec_count ();
    }

    void
    accept (future_type &future)
    {
      future.attach (this);
      this->increment_exec_count ();
    }

    /// Waits for completion of all events, with a timeout
    bool
    wait_on_completion (ACE_Time_Value *tv)
    {
      return this->wait_strategy_.wait_on_completion (tv);
    }

    void
    completed_events (future_list &event_list)
    {
      ACE_GUARD_THROW_EX (ACE_SYNCH_MUTEX,
                          guard,
                          this->mutex_,
                          CORBA::NO_RESOURCES ());

      event_list.swap (this->completed_events_);
      this->completed_events_.clear ();
    }

    WAIT_STRATEGY& wait_strategy ()
    {
      return this->wait_strategy_;
    }

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Completion_T (const Completion_T&) = delete;
    Completion_T (Completion_T&&) = delete;
    Completion_T& operator= (const Completion_T& x) = delete;
    Completion_T& operator= (Completion_T&& x) = delete;
    //@}
  protected:
    void
    on_all_completed () override
    {
      this->condition_.broadcast ();
    }

    void
    on_all_completed_with_failure () override
    {
      this->condition_.broadcast ();
    }

    bool
    wait_on_all_completed (ACE_Time_Value *tv)
    {
      ACE_GUARD_RETURN (ACE_SYNCH_MUTEX,
                        guard,
                        this->mutex_,
                        false);

      return this->all_completed () || (this->condition_.wait (tv) != -1);
    }

    friend WAIT_STRATEGY;

  private:
    ACE_SYNCH_MUTEX mutex_;
    ACE_Condition< ACE_SYNCH_MUTEX > condition_;

    WAIT_STRATEGY wait_strategy_;

    future_list completed_events_;
  };
}

#endif /* COMPLETION_BASE_H_ */
