/**
 * @file    dancex11_deployment_state.cpp
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 deployment state singleton
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11_deployment_state.h"

#include "dancex11/logger/log.h"
#include "ciaox11/versionx11.h"

#include <stdexcept>

#if !defined (AXCIOMA_RELEASE_BUILDSTAMP)
# define AXCIOMA_RELEASE_BUILDSTAMP \
  __DATE__ " " __TIME__
#endif

namespace DAnCEX11
{
  State *State::instance_ {};

  State *State::instance ()
  {
    if (State::instance_ == nullptr)
    {
      DANCEX11_LOG_PANIC ("State::instance - " \
                          "DAnCEX11 Deployment State has not been defined.");

      throw std::runtime_error ("State::instance - " \
          "DAnCEX11 Deployment State has not been defined.");
    }
    return State::instance_;
  }

  State::State ()
  {
    DANCEX11_LOG_INFO ("AXCIOMA Version " << (uint32_t) CIAOX11_MAJOR_VERSION << '.' <<
                                             (uint32_t) CIAOX11_MINOR_VERSION << '.' <<
                                             (uint32_t) CIAOX11_MICRO_VERSION  << '.' <<
                                             " Copyright (c) Remedy IT");
    DANCEX11_LOG_INFO ("Buildstamp: " << AXCIOMA_RELEASE_BUILDSTAMP);
  }

  State::~State ()
  {
    DANCEX11_LOG_TRACE ("State::~State");

    this->close ();
  }

  void State::close ()
  {
    DANCEX11_LOG_TRACE ("State::close");

    if (this->orb_)
    {
      DANCEX11_LOG_DEBUG ("State::close - " <<
                          "destroying Deployment ORB");
      this->orb_->destroy ();
      this->orb_.reset ();

      DANCEX11_LOG_DEBUG ("State::close - " <<
                          "Deployment ORB destroyed");
    }
  }

  bool State::initialize (int argc, char* argv[])
  {
    DANCEX11_LOG_TRACE ("State::initialize");

    DANCEX11_LOG_DEBUG ("State::initialize - " <<
                        "creating Deployment ORB");

    this->orb_ = CORBA::ORB_init (argc, argv, "DnCX11_Deployment_ORB");

    if (this->orb_)
    {
      // Get reference to Root POA.
      DANCEX11_LOG_DEBUG ("State::initialize - " <<
                          "Resolving root POA");

      IDL::traits<CORBA::Object>::ref_type obj =
          this->orb_->resolve_initial_references ("RootPOA");

      this->root_poa_ = IDL::traits<PortableServer::POA>::narrow (obj);
      if (!this->root_poa_)
      {
        DANCEX11_LOG_ERROR ("State::initialize - " <<
                            "Narrowing root POA returned nil reference");
      }
      return true;
    }
    else
    {
      DANCEX11_LOG_PANIC ("State::initialize - " <<
                          "failed to initialize Deployment ORB");
      return false;
    }
  }

}
