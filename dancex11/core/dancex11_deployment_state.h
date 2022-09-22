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

#include <vector>
#include <string>

namespace DAnCEX11
{
  class StateGuard;

  class DANCEX11_STATE_Export State final
  {
  public:
    ~State ();

    void initialize (std::vector<std::string>&& args);

    void add_service_directive(std::string&& svcdir);

    bool has_orb () const
    { return (bool)this->orb_; }

    void close ();

    IDL::traits<CORBA::ORB>::ref_type orb ()
    { return this->get_orb (); }

    IDL::traits<PortableServer::POA>::ref_type root_poa ()
    { return this->get_root_poa (); }

    static State* instance ();

  private:
    IDL::traits<CORBA::ORB>::ref_type get_orb ();
    IDL::traits<PortableServer::POA>::ref_type get_root_poa ();

    friend class StateGuard;

    State ();

    std::vector<std::string> args_;
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
        State::instance_ = std::addressof(this->state_);
    }
    inline ~StateGuard ()
    {
      DANCEX11_LOG_TRACE ("StateGuard::~StateGuard");

      if (State::instance_ == std::addressof(this->state_))
        State::instance_ = nullptr;
    }

  private:
    State state_;
  };

}

#define DANCEX11_DEFINE_DEPLOYMENT_STATE(name) \
  DAnCEX11::StateGuard __dancex11_ ## name ## _guard

#endif /* DANCEX11_DEPLOYMENT_STATE_H_ */
