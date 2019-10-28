/**
 * @file    dancex11_deployment_state.h
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 deployment state singleton
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DEPLOYMENT_STATE_H_
#define DANCEX11_DEPLOYMENT_STATE_H_

#pragma once

#include "dancex11_state_export.h"
#include "dancex11/logger/log.h"

#include "tao/x11/portable_server/portableserver.h"
#include "tao/x11/orb.h"

namespace DAnCEX11
{
  class StateGuard;

  class DANCEX11_STATE_Export State final
  {
  public:
    ~State ();

    bool initialize (int argc, char* argv[]);

    void close ();

    IDL::traits<CORBA::ORB>::ref_type orb ()
    { return this->orb_; }

    IDL::traits<PortableServer::POA>::ref_type root_poa ()
    { return this->root_poa_; }

    static State* instance ();

  private:
    friend class StateGuard;

    State ();

    IDL::traits<CORBA::ORB>::ref_type orb_;
    IDL::traits<PortableServer::POA>::ref_type root_poa_;

    static State* instance_;
  };

  class DANCEX11_STATE_Export StateGuard final
  {
  public:
    inline StateGuard ()
    {
      DANCEX11_LOG_TRACE ("StateGuard::StateGuard");

      if (State::instance_ == nullptr)
        State::instance_ = &this->state_;
    }
    inline ~StateGuard ()
    {
      DANCEX11_LOG_TRACE ("StateGuard::~StateGuard");

      if (State::instance_ == &this->state_)
        State::instance_ = nullptr;
    }

  private:
    State state_;
  };

}

#define DANCEX11_DEFINE_DEPLOYMENT_STATE(name) \
  DAnCEX11::StateGuard __dancex11_ ## name ## _guard

#endif /* DANCEX11_DEPLOYMENT_STATE_H_ */
