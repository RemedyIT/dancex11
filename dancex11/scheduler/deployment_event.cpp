/**
 * @file    deployment_event.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "deployment_event.h"

namespace DAnCEX11
{
  Event_Result::Event_Result (const std::string &id,
    bool exception)
    : id_ (std::move(id))
    , exception_ (exception)
  {
  }

  Event_Result::Event_Result (const std::string &id,
    bool exception,
    const CORBA::Any &any)
    : id_ (std::move(id))
    , exception_ (exception)
    , contents_ (std::move(any))
  {
  }

  Deployment_Event::Deployment_Event (Event_Future holder,
    const std::string &name,
    const std::string &inst_type)
    : holder_ (holder)
    , name_ (std::move(name))
    , instance_type_ (std::move(inst_type))
  {
  }

  Event_Future
  Deployment_Event::get_future ()
  {
    return holder_;
  }
}
