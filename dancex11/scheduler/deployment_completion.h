/**
 * @file    deployment_completion.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_DEPLOYMENT_COMPLETION_H
#define DAnCEX11_DEPLOYMENT_COMPLETION_H

#pragma once

#include "completion_base.h"
#include "dancex11/scheduler/deployment_event.h"
#include "dancex11/scheduler/deployment_scheduler.h"
#include "dancex11/scheduler/deployment_default_wait_strategy.h"

namespace DAnCEX11
{
  /**
   * Deployment_Completion
   */
  typedef Completion_T<Default_Completion_Wait_Strategy<Event_Result>, Event_Result> Deployment_Completion;
}

#endif /* DAnCEX11_DEPLOYMENT_COMPLETION_H */
