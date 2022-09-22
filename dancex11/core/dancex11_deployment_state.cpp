/**
 * @file    dancex11_deployment_state.cpp
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 deployment state singleton
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
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

    if (this->root_poa_)
    {
      DANCEX11_LOG_DEBUG ("State::close - " <<
                          "Destroying root POA");

      this->root_poa_->destroy (true, true);
      this->root_poa_.reset ();

      DANCEX11_LOG_DEBUG ("State::close - " <<
                          "Deployment root POA destroyed");
    }

    if (this->orb_)
    {
      DANCEX11_LOG_DEBUG ("State::close - " <<
                          "Destroying deployment ORB");

      this->orb_->destroy ();
      this->orb_.reset ();

      DANCEX11_LOG_DEBUG ("State::close - " <<
                          "Deployment ORB destroyed");
    }
  }

  void State::initialize (std::vector<std::string>&& args)
  {
    DANCEX11_LOG_TRACE ("State::initialize");

    this->args_ = std::move (args);
  }

  void State::add_service_directive(std::string&& svcdir)
  {
    DANCEX11_LOG_TRACE ("State::add_service_directive");

    DANCEX11_LOG_DEBUG ("State::add_service_directive - " <<
                        svcdir);

    this->args_.push_back ("-ORBSvcConfDirective");
    this->args_.push_back (std::move (svcdir));
  }

  IDL::traits<CORBA::ORB>::ref_type State::get_orb ()
  {
    DANCEX11_LOG_TRACE ("State::get_orb");

    if (!this->orb_)
    {
      DANCEX11_LOG_DEBUG ("State::get_orb - " <<
                          "creating Deployment ORB");

      int argc = ACE_Utils::truncate_cast<int> (this->args_.size ());
      std::unique_ptr<const char*[]> orb_argv = std::make_unique<const char*[]> (argc);
      int narg = 0;
      for (const std::string& a : this->args_)
      {
        orb_argv.get ()[narg++] = a.c_str ();
      }
      this->orb_ = CORBA::ORB_init (argc,
                                    const_cast<char**> (orb_argv.get ()),
                                    "DnCX11_Deployment_ORB");
      if (!this->orb_)
      {
        DANCEX11_LOG_PANIC ("State::get_orb - " <<
                            "failed to initialize Deployment ORB");
      }
    }
    return this->orb_;
  }

  IDL::traits<PortableServer::POA>::ref_type State::get_root_poa ()
  {
    DANCEX11_LOG_TRACE ("State::get_root_poa");

    if (!this->root_poa_)
    {
      IDL::traits<CORBA::ORB>::ref_type orb = this->get_orb ();
      if (orb)
      {
        // Get reference to Root POA.
        DANCEX11_LOG_DEBUG ("State::get_root_poa - " <<
                            "Resolving root POA");

        IDL::traits<CORBA::Object>::ref_type obj =
            orb->resolve_initial_references ("RootPOA");

        this->root_poa_ = IDL::traits<PortableServer::POA>::narrow (obj);
        if (!this->root_poa_)
        {
          DANCEX11_LOG_ERROR ("State::get_root_poa - " <<
                              "Narrowing root POA returned nil reference");
        }
      }
    }
    return this->root_poa_;
  }
}
