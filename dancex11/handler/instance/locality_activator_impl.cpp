// -*- C++ -*-
/**
 * @file locality_activator_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "locality_activator_impl.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11/core/dancex11_dmh_loader.h"

#include "ace/UUID.h"
#include "ace/CORBA_macros.h"
#include "ace/OS_NS_sys_stat.h"
#include "ace/Service_Config.h"
#include "ace/Dynamic_Service.h"
#include "ace/Countdown_Time.h"

#include <sstream>
#include <thread>

namespace DAnCEX11
{
#if defined (ACE_WIN32)
  DAnCE_LocalityActivator_i::Watchdog::Watchdog (ACE_Process_Manager &procman)
    : procman_ (procman)
  {
  }

  int
  DAnCE_LocalityActivator_i::Watchdog::svc ()
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Watchdog::svc");
    while (!this->stop_)
    {
      if (this->procman_.managed () > 0)
      {
        this->procman_.wait (0, ACE_Time_Value (0, 25000));
      }
      else
      {
        std::this_thread::sleep_for (std::chrono::milliseconds (25));
      }
    }
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Watchdog::svc - exit");
    return 0;
  }

  bool
  DAnCE_LocalityActivator_i::Watchdog::start ()
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Watchdog::start");
    return this->activate () == 0;
  }

  void
  DAnCE_LocalityActivator_i::Watchdog::stop ()
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Watchdog::stop");
    this->stop_ = true;
    this->wait ();
  }
#endif

  DAnCE_LocalityActivator_i::Server_Info::~Server_Info ()
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Server_Info::~Server_Info");
  }

  DAnCE_LocalityActivator_i::DAnCE_LocalityActivator_i (
      Deployment::Properties props,
      bool multithreaded,
      IDL::traits< CORBA::ORB>::ref_type orb,
      IDL::traits< PortableServer::POA>::ref_type poa)
    :
#if defined (ACE_WIN32)
      process_watcher_ (process_manager_),
#endif
      multithreaded_ (multithreaded),
      orb_ (std::move(orb)),
      poa_ (std::move(poa)),
      properties_ (std::move (props)),
      mutex_ (),
      condition_ (mutex_)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::DAnCE_LocalityActivator_i");
    ACE_Utils::UUID_GENERATOR::instance ()->init ();


    DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::DAnCE_LocalityActivator_i - " <<
                       "Configured with " << this->properties_.size () << " properties");

#if defined (ACE_WIN32)
    // start the watchdog
    if (!this->process_watcher_.start ())
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::DAnCE_LocalityActivator_i - " <<
                          "failed to start locality process watchdog");
    }
    // initialize the process manager without reactor
    this->process_manager_.open (ACE_Process_Manager::DEFAULT_SIZE);
#else
    // initialize the process manager with the ORBs reactor
    // so the eXit handlers get triggered when needed
    this->process_manager_.open (ACE_Process_Manager::DEFAULT_SIZE,
                                 this->orb_->reactor ());
#endif
  }

  DAnCE_LocalityActivator_i::~DAnCE_LocalityActivator_i()
  {
#if defined (ACE_WIN32)
    this->process_watcher_.stop ();
#endif
  }

  void
  DAnCE_LocalityActivator_i::locality_manager_callback (
    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type serverref,
    const std::string & server_UUID,
    ::Deployment::Properties & config)
  {
    DANCEX11_LOG_TRACE("DAnCE_LocalityActivator_i::locality_manager_callback");

    Server_Info *info = 0;

    {
      ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                           guard,
                           this->container_mutex_,
                           CORBA::NO_RESOURCES ());
      for (SERVER_INFOS::iterator i  (this->server_infos_.begin ());
           i != this->server_infos_.end (); ++i)
      {
        DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                            "Comparing " << (*i)->uuid_ << " with " << server_UUID);
        if ((*i)->uuid_ == server_UUID)
        {
          info = (*i).get ();
          break;
        }
      }
    }

    if (!info)
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                          "Received callback from LocalityManager " << server_UUID <<
                          ", which doesn't belong to me.");
      throw CORBA::BAD_PARAM ();
    }

    if (info->status_ == Server_Info::ACTIVE)
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                          "Received callback from LocalityManager " << server_UUID <<
                          ", which has already been configured.");
      throw CORBA::BAD_INV_ORDER ();
    }

    if (info->ref_)
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                          "Received callback from LocalityManager " << server_UUID <<
                          ", which has already called back.");
      throw CORBA::BAD_INV_ORDER ();
    }

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                        "Received callback from LocalityManager " << server_UUID);

    info->ref_ = serverref;

    Utility::build_property_sequence (config, info->cmap_);

    // @@TODO: May want to print out configvalues here.
    DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::locality_manager_callback - "
                       "Generated " << config.size () <<
                       " Properties for LocalityManager " << server_UUID);
  }

  void
  DAnCE_LocalityActivator_i::configuration_complete (const std::string & server_UUID)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::configuration_complete");

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::configuration_complete - "
                        "Received configuration_complete from LocalityManager " << server_UUID);

    try
    {
      Server_Info *info = 0;

      {
        ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                             guard,
                             this->container_mutex_,
                             CORBA::NO_RESOURCES ());
        for (SERVER_INFOS::iterator i  (this->server_infos_.begin ());
             i != this->server_infos_.end (); ++i)
        {
          DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                              "Comparing " << (*i)->uuid_ << " with " << server_UUID);
          if ((*i)->uuid_ == server_UUID)
          {
            info = (*i).get ();
            break;
          }
        }
      }

      if (!info)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                            "Received configuration_complete from LocalityManager " << server_UUID <<
                            ", which doesn't belong to me.");
        throw CORBA::BAD_PARAM ();
      }

      if (info->status_ == Server_Info::ACTIVE)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                            "Received configuration_complete from LocalityManager " << server_UUID <<
                            ", which has already been completed.");
        throw CORBA::BAD_INV_ORDER ();
      }

      if (!info->ref_)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::locality_manager_callback - " <<
                            "Received configuration_complete from LocalityManager " << server_UUID <<
                            ", which has not called back.");
        throw CORBA::BAD_INV_ORDER ();
      }

      ACE_GUARD_THROW_EX (ACE_SYNCH_MUTEX,
                          guard,
                          info->mutex_,
                          CORBA::NO_RESOURCES ());
      info->status_ = Server_Info::ACTIVE;
      info->condition_.signal ();
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::configuration_complete - " \
                          "Caught unknown exception while processing " \
                          "configuration_complete");
      throw;
    }
  }

  IDL::traits < ::DAnCEX11::LocalityManager>::ref_type
  DAnCE_LocalityActivator_i::create_locality_manager (const ::Deployment::DeploymentPlan &plan,
                                                      uint32_t instanceRef,
                                                      const ::Deployment::Properties & config)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::create_locality_manager");

    Safe_Server_Info server = std::make_shared <Server_Info> (plan,
                                                              instanceRef);

    DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::create_locality_manager - " <<
                       "Received " << config.size () << " config properties");

    DAnCEX11::Utility::build_property_map (server->cmap_, this->properties_);
    DAnCEX11::Utility::build_property_map (server->cmap_, config);

    ::DAnCEX11::Utility::PROPERTY_MAP::iterator itprop =
        server->cmap_.find (DAnCEX11::LOCALITY_ACTIVATION_MODE);
    if (itprop != server->cmap_.end ())
    {
      if (itprop->second >>= server->activation_mode_)
      {
        DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::create_locality_manager - "
                            "Using provided LM activation mode [" <<
                            server->activation_mode_ << "]");
      }
      else
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::create_locality_manager - " <<
                            "Failed to extract provided activation mode " <<
                            "from property '" << DAnCEX11::LOCALITY_ACTIVATION_MODE <<
                            "', falling back to default mode [" <<
                            server->activation_mode_ << "]");
      }
    }

    // register locality information
    {
      ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                           guard,
                           this->container_mutex_,
                           CORBA::NO_RESOURCES ());
      server_infos_.insert (server);
    }

    try
    {
      switch (server->activation_mode_)
      {
        case DAnCEX11::LOCALITY_AM_SPAWN:
          return this->activate_child_locality (server);

        case DAnCEX11::LOCALITY_AM_LOCAL:
          return this->activate_local_locality (server);

        case DAnCEX11::LOCALITY_AM_REMOTE:
          return this->activate_remote_locality (server);
      }
    }
    catch (...)
    {
      // in case of startup errors remove the registered locality since
      // there will never come a regular remove request

      ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                           guard,
                           this->container_mutex_,
                           CORBA::NO_RESOURCES ());

      SERVER_INFOS::iterator i;
      for (i = this->server_infos_.begin ();
           i != this->server_infos_.end ();
           ++i)
      {
        if ((*i) && (*i)->plan_UUID_ == server->plan_UUID_)
          {
            this->server_infos_.erase (i);
            break;
          }
      }

      throw; // rethrow caught exception
    }

    return {};
  }

  IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
  DAnCE_LocalityActivator_i::activate_remote_locality (Safe_Server_Info ssi)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_remote_locality");

    std::string lm_ior;
    // check for availability of explicit LM IOR
    ::DAnCEX11::Utility::PROPERTY_MAP::iterator itprop = ssi->cmap_.find (DAnCEX11::LOCALITY_IOR);
    if (itprop != ssi->cmap_.end ())
    {
      if (itprop->second >>= lm_ior)
      {
        DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_remote_locality - "
                            "Using provided LM IOR [" << lm_ior << "]");
      }
      else
      {
        DANCEX11_LOG_PANIC ("DAnCE_LocalityActivator_i::activate_remote_locality - " <<
                            "Failed to extract provided LM IOR " <<
                            "from property '" << DAnCEX11::LOCALITY_IOR << "'");
        throw Deployment::StartError ("Locality_Activator",
                                      "Failed to retrieve remote LM reference");
      }
    }
    else
    {
      uint16_t listen_port {};
      // check for explicit listen port
      if ((itprop = ssi->cmap_.find (DAnCEX11::LOCALITY_PORT)) != ssi->cmap_.end ())
      {
        itprop->second >>= listen_port;

        DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_remote_locality - " <<
                            "Using provided locality listen port " << listen_port);
      }

      if (listen_port == 0)
      {
        DANCEX11_LOG_PANIC ("DAnCE_LocalityActivator_i::activate_remote_locality - " <<
                            "Unable to determine remote LM IOR, listen_port is 0");
        throw Deployment::StartError ("Locality_Activator",
                                      "Unable to determine remote LM IOR");
      }

      std::string listen_addr {};
      // check for explicit listen address
      if ((itprop = ssi->cmap_.find (DAnCEX11::LOCALITY_ADDRESS)) != ssi->cmap_.end ())
      {
        itprop->second >>= listen_addr;

        DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_remote_locality - " <<
                            "Using provided locality listen address " << listen_addr);
      }

      if (listen_addr.empty ())
      {
        bool numeric_addr {};
        // check for numeric addresses spec
        if ((itprop = ssi->cmap_.find (DAnCEX11::DM_NUMERIC_ADDRESSES)) != ssi->cmap_.end ())
        {
          itprop->second >>= numeric_addr;

          DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_remote_locality - " <<
                              (numeric_addr ? "" : "NOT") <<
                              "using numeric IP addresses");
        }

        listen_addr = numeric_addr ? "127.0.0.1" : "localhost";
      }

      // construct a corbaloc IOR for LM (assume default service name)
      std::ostringstream os;
      os << "corbaloc:iiop:" << listen_addr << ":" << listen_port << "/" << "DeploymentManager.LocalityManager";
      lm_ior = os.str ();
    }

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_remote_locality - "
                        "Resolving LM IOR [" << lm_ior << "]");

    // resolve LM reference
    IDL::traits<CORBA::Object>::ref_type obj =
        this->orb_->string_to_object (lm_ior);
    ssi->ref_ = IDL::traits< ::DAnCEX11::LocalityManager>::narrow (obj);

    if (!ssi->ref_)
    {
      DANCEX11_LOG_PANIC ("DAnCE_LocalityActivator_i::activate_remote_locality - " <<
                          "Unable to resolve remote LM reference");
      throw Deployment::StartError ("Locality_Activator",
                                    "Unable to resolve remote LM reference");
    }

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_remote_locality - "
                        "Configuring LM ");

    // configure LM instance
    Deployment::Properties lm_prop;
    Utility::build_property_sequence (lm_prop, ssi->cmap_);
    ssi->ref_->configure (lm_prop);

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_remote_locality - "
                        "LM instance configured and active");

    ssi->status_ = Server_Info::ACTIVE;

    return ssi->ref_;
  }

  IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
  DAnCE_LocalityActivator_i::activate_local_locality (Safe_Server_Info ssi)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_local_locality");

    ::DAnCEX11::Utility::PROPERTY_MAP::iterator itprop;

    // determine deployment handler to load
    std::string handler = "dancex11_locality_dm_handler";
    if ((itprop = ssi->cmap_.find (DAnCEX11::LOCALITY_DM_HANDLER)) != ssi->cmap_.end ())
    {
      itprop->second >>= handler;
      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_local_locality - " <<
                          "Using provided locality DM handler: " <<
                          handler);
    }
    else
    {
      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_local_locality - " <<
                          "Using default locality DM handler: " <<
                          handler);
    }

    // determine config file to pass to deployment handler
    std::string cfg_file;
    if ((itprop = ssi->cmap_.find (DAnCEX11::LOCALITY_CONFIGFILE)) != ssi->cmap_.end ())
    {
      itprop->second >>= cfg_file;

      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_local_locality - " <<
                          "Using provided locality configuration file " << cfg_file);
    }

    const std::string svcobj_name = "DANCEX11_INPROCESS_LOCALITY_MANAGER";

    // load handler
    // create an ACE Service Config directive for the DMH loader
    std::ostringstream os;
    os << "dynamic " << svcobj_name << " Service_Object * " <<
          handler << ":_make_DAnCEX11_DeploymentHandler_Impl() \"\"";
    std::string svcfg_txt = os.str ();

    if (ACE_Service_Config::process_directive (svcfg_txt.c_str ()) != 0)
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::activate_local_locality - " <<
                          "Error(s) while processing DMH loader "
                          "Service Config directive :\n\t" <<
                          svcfg_txt);
      throw Deployment::StartError ("Locality_Activator",
                                    "Unable to load inprocess LocalityManager");
    }

    DAnCEX11::DMHandler_Loader *dmh_loader {};
    try {
      DANCEX11_LOG_DEBUG ( "DAnCE_LocalityActivator_i::activate_local_locality - "
                           "Resolving DMH loader <" << svcobj_name << ">");
      dmh_loader =
        ACE_Dynamic_Service<DAnCEX11::DMHandler_Loader>::instance (svcobj_name.c_str ());
    }
    catch (...) {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::activate_local_locality - "
                          "Unknown exception caught while resolving the "
                          "DMH loader <" << svcobj_name << ">");
      throw Deployment::StartError ("Locality_Activator",
                                    "Unable to load inprocess LocalityManager");
    }

    ssi->dmh_ =
        dmh_loader->create_handler ();

    // get LM properties
    Deployment::Properties lm_prop;
    Utility::build_property_sequence (lm_prop, ssi->cmap_);

    // configure DMH
    ssi->dmh_->configure (cfg_file, lm_prop, nullptr);

    std::string svc_name = ssi->plan_UUID_ + ".LocalityManager";

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_local_locality - " <<
                        "Activating locality manager service : " <<
                        svc_name);

    // deployment management service requested; activate standard DM
    IDL::traits<Deployment::Deployment_Manager>::ref_type ddm =
        ssi->dmh_->activate_manager (svc_name);

    // narrow
    ssi->ref_ = IDL::traits< ::DAnCEX11::LocalityManager>::narrow (ddm);

    if (!ssi->ref_)
    {
      DANCEX11_LOG_PANIC ("DAnCE_LocalityActivator_i::activate_local_locality - " <<
                          "Unable to narrow local LM reference");
      throw Deployment::StartError ("Locality_Activator",
                                    "Unable to narrow local LM reference");
    }

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_local_locality - "
                        "Configuring LM ");

    // configure LM instance
    ssi->ref_->configure (lm_prop);

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_local_locality - "
                        "LM instance configured and active");

    ssi->status_ = Server_Info::ACTIVE;

    return ssi->ref_;
  }

  IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
  DAnCE_LocalityActivator_i::activate_child_locality (Safe_Server_Info ssi)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_child_locality");

    std::string cmd_options = this->construct_command_line (*ssi);

    DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::activate_child_locality - " <<
                       "LocalityManager arguments: " << cmd_options);

    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::activate_child_locality - " <<
                        "Attempting to spawn LocalityManager with UUID " <<
                        ssi->uuid_);

    // Now we need to get a copy of the one that was inserted...
    pid_t const pid =
        this->spawn_locality_manager (std::make_unique<Server_Child_Handler> (ssi),
                                      cmd_options);

    ACE_Time_Value timeout (30);

    ::DAnCEX11::Utility::PROPERTY_MAP::iterator itprop = ssi->cmap_.find (DAnCEX11::LOCALITY_TIMEOUT);
    if (itprop != ssi->cmap_.end ())
    {
      uint32_t t;
      if (itprop->second >>= t)
      {
        DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::activate_child_locality - "
                            "Using provided non-default server timeout of " << t);
        timeout = ACE_Time_Value (t);
      }
      else
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::activate_child_locality - " <<
                            "Failed to extract provided non-default server timeout " <<
                            "from property '" << DAnCEX11::LOCALITY_TIMEOUT <<
                            "', falling back to default timeout of " << timeout.sec ());
      }
    }

    if (this->multithreaded_)
      this->multi_threaded_wait_for_callback (*ssi, timeout);
    else
      this->single_threaded_wait_for_callback (*ssi, timeout);

    ssi->pid_ = pid; // register pid of successfully started lm process

    DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::activate_child_locality - "
                       "LocalityManager " << ssi->uuid_ <<
                       " successfully spawned and configured!");

    return ssi->ref_;
  }

  std::string
  DAnCE_LocalityActivator_i::construct_command_line (Server_Info &server)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line");

    // Get my object reference
    IDL::traits<CORBA::Object>::ref_type obj = this->poa_->servant_to_reference (this->_lock ());
    std::string ior = this->orb_->object_to_string (obj);

    ::DAnCEX11::Utility::PROPERTY_MAP::iterator itprop;

    if ((itprop = server.cmap_.find (DAnCEX11::LOCALITY_UUID)) != server.cmap_.end ())
    {
      // Nodeapplication has requested a custom uuid
      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - "
                          "Using provided UUID");
      std::string uuid;
      itprop->second >>= uuid;
      server.uuid_ = uuid;
    }
    else
    {
      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - "
                          "Using generated UUID");
      ACE_Utils::UUID uuid;
      ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID (uuid);
      server.uuid_ = uuid.to_string ()->c_str ();
    }

    std::string args {};

    if ((itprop = server.cmap_.find (DAnCEX11::LOCALITY_ARGUMENTS)) != server.cmap_.end ())
    {
      itprop->second >>= args;

      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using provided server arguments " << args);
    }

    std::string cfg_file {};

    if ((itprop = server.cmap_.find (DAnCEX11::LOCALITY_CONFIGFILE)) != server.cmap_.end ())
    {
      itprop->second >>= cfg_file;

      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using provided locality configuration file " << cfg_file);
    }

    uint16_t listen_port {};

    if ((itprop = server.cmap_.find (DAnCEX11::LOCALITY_PORT)) != server.cmap_.end ())
    {
      itprop->second >>= listen_port;

      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using provided locality listen port " << listen_port);
    }

    std::string listen_addr {};

    if ((itprop = server.cmap_.find (DAnCEX11::LOCALITY_ADDRESS)) != server.cmap_.end ())
    {
      itprop->second >>= listen_addr;

      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using provided locality listen address " << listen_addr);
    }

    bool numeric_addr {};

    if ((itprop = server.cmap_.find (DAnCEX11::DM_NUMERIC_ADDRESSES)) != server.cmap_.end ())
    {
      itprop->second >>= numeric_addr;

      DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          (numeric_addr ? "" : "NOT") <<
                          "using numeric IP addresses");
    }

    DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::construct_command_line - "
                       "using arguments [" << args << "] "
                       "and config file [" << cfg_file << "]");

    std::string handler = "dancex11_locality_dm_handler";

    if ((itprop = server.cmap_.find (DAnCEX11::LOCALITY_DM_HANDLER)) != server.cmap_.end ())
    {
      itprop->second >>= handler;
      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using provided locality DM handler: " <<
                          handler);
    }
    else
    {
      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using default locality DM handler: " <<
                          handler);
    }

    std::string domain_nc;

    if ((itprop = server.cmap_.find (DAnCEX11::DOMAIN_NC)) != server.cmap_.end ())
    {
      itprop->second >>= domain_nc;
      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::construct_command_line - " <<
                          "Using provided domain NC: " <<
                          domain_nc);
    }

    // in case no explicit listen address is specified we'll default to locahost
    if (listen_addr.empty ())
    {
      listen_addr = numeric_addr ? "127.0.0.1" : "localhost";
    }

    // Build our command line to launch the component server
    std::ostringstream cmd_os;
    cmd_os << args;
    if (!cfg_file.empty ())
    {
      cmd_os << " -c " << cfg_file;
    }
    if (listen_port > 0)
    {
      cmd_os << " -p " << listen_port;
    }
    if (!listen_addr.empty ())
    {
      cmd_os << " -a " << listen_addr;
    }
    if (numeric_addr)
    {
      cmd_os << " -N";
    }
    // skip if explicitly configured as empty value
    if (!handler.empty ())
    {
      // add handler arg
      cmd_os << " --handler " << handler;
    }
    if (!domain_nc.empty ())
    {
      // add domain NC arg
      cmd_os << " --deployment-nc " << domain_nc;
    }
    // add UUID and IOR as handler args
    cmd_os << " -- -u "
           << server.uuid_
           << " -r "
           << ior;

    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::construct_command_line - LM args " << cmd_os.str ());

    return cmd_os.str ();
  }

  pid_t
  DAnCE_LocalityActivator_i::spawn_locality_manager (std::unique_ptr<Server_Child_Handler> exit_handler,
                                                     const std::string &cmd_line)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::spawn_locality_manager");

    const Server_Info &si = exit_handler->server_info ();

    ::DAnCEX11::Utility::PROPERTY_MAP::const_iterator itprop;

    std::string path = "dancex11_deployment_manager";

    if ((itprop = si.cmap_.find (DAnCEX11::LOCALITY_EXECUTABLE)) != si.cmap_.end ())
    {
      itprop->second >>= path;
      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::spawn_locality_manager - " <<
                          "Using provided locality manager executable: " <<
                          path);
    }
    else
    {
#if defined (ACE_WIN32)
      // On Windows by default spawn the same deployment manager executable
      // as used for this process (makes sure to use the same Release/Debug
      // environment which may cause problems otherwise; at least with MSVC).
      ACE_TCHAR szPath[MAX_PATH];

      if( !::GetModuleFileName(0, szPath, MAX_PATH) )
      {
          DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::spawn_locality_manager - " <<
                              "Unable to determine deployment executable from current process [" <<
                              GetLastError() << "]");
          throw Deployment::StartError ("locality_manager",
                                        "Unable to determine deployment executable from current process");
      }
      path = ACE_TEXT_ALWAYS_CHAR (szPath);

#else
      // check for existence of $DANCEX11_ROOT/bin/dancex11_deployment_manager
      // otherwise assume executable to be locatable in PATH
      // try to resolve DANCEX11_ROOT
      const char* dancex11_env = std::getenv ("DANCEX11_ROOT");
      if (dancex11_env)
      {
        ACE_stat  st;
        std::string filename (dancex11_env);
        filename += ACE_DIRECTORY_SEPARATOR_STR_A;
        filename += "bin";
        filename += ACE_DIRECTORY_SEPARATOR_STR_A;
        filename += path;
        filename += ACE_PLATFORM_EXE_SUFFIX_A;
        if (ACE_OS::stat (filename.c_str (), &st) == 0
#if defined (S_ISREG)
          &&
          S_ISREG (st.st_mode)
#endif
         )
        {
          path = filename;
        }
      }
#endif

      DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::spawn_locality_manager - " <<
                          "Using default locality manager executable: " <<
                          path);
    }

    ACE_Process_Options options (true,
      path.size () + cmd_line.size () + 15);
    if (options.command_line ("%s %s",
                              path.c_str (),
                              cmd_line.c_str ()) != 0)
    {
      DANCEX11_LOG_ERROR ("Failed to create commandline");
      throw Deployment::StartError ("locality_manager",
                                    "Failed to create command line for server");
    }

    options.avoid_zombies (0);

    // check for customized PATH addition
    if ((itprop = si.cmap_.find (DANCE_LM_PATH)) != si.cmap_.end ())
    {
      itprop->second >>= path;
      if (!path.empty ())
        {
          const char * p_env = std::getenv ("PATH");
          std::string newpath (p_env ? p_env : "");
          newpath += ACE_TEXT_ALWAYS_CHAR (ACE_LD_SEARCH_PATH_SEPARATOR_STR);
          newpath += path;
          options.setenv (ACE_TEXT("PATH"), ACE_TEXT_CHAR_TO_TCHAR (newpath.c_str ()));

          DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::spawn_locality_manager - " <<
                             "configured customized PATH environment: " <<
                             newpath);
        }
    }

    // check for customized LD search path addition
    if ((itprop = si.cmap_.find (DANCE_LM_LIBPATH)) != si.cmap_.end ())
    {
      itprop->second >>= path;
      if (!path.empty ())
      {
        const char * p_env = std::getenv (ACE_TEXT_ALWAYS_CHAR (ACE_LD_SEARCH_PATH));
        std::string newpath (p_env ? p_env : "");
        newpath += ACE_TEXT_ALWAYS_CHAR (ACE_LD_SEARCH_PATH_SEPARATOR_STR);
        newpath += path;
        options.setenv (ACE_LD_SEARCH_PATH, ACE_TEXT_CHAR_TO_TCHAR (newpath.c_str ()));

        DANCEX11_LOG_INFO ("DAnCE_LocalityActivator_i::spawn_locality_manager - " <<
                           "configured customized " << ACE_LD_SEARCH_PATH <<
                           " environment: " << newpath);
      }
    }

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::spawn_locality_manager - " <<
                        "Spawning process, command line is " <<
                        options.command_line_buf ());

    pid_t const pid = this->process_manager_.spawn (options,
                                                    exit_handler.get ());

    if (pid == ACE_INVALID_PID)
    {
      DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::spawn_locality_manager - "
                          "Failed to spawn a LocalityManager process");
      throw Deployment::StartError ("locality_manager",
                                    "Failed to spawn process");
    }
    else
    {
      // release as in this case the handler is registered and will be deleted later
      exit_handler.release ();
    }

    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::spawn_locality_manager - "
                        "Process successfully spawned with pid " << pid);
    return pid;
  }

  void
  DAnCE_LocalityActivator_i::
  single_threaded_wait_for_callback (const Server_Info &si,
                                     ACE_Time_Value &timeout)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::single_threaded_wait_for_callback");

    ACE_Countdown_Time countdown (std::addressof(timeout));

    // Below code is broken for thread-per-connection concurrency model,
    // since the main thread is running ORB event loop and will spawn
    // a different thread to handle the call <register_node_application>,
    // the <perform_work> operation will not be invoked and finally
    // a timeout will occur. For a similar reason, it won't work
    // for thread-pool concurrency model.
    while (true)
    {
      if (si.status_ != Server_Info::ACTIVE && si.status_ != Server_Info::TERMINATED)
      {
        ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                             guard,
                             this->mutex_,
                             CORBA::NO_RESOURCES ());
        // The next guy to acquire the mutex may have already
        // been activated by the previous leader's perform_work,
        // so let's check to make sure that only non-activated
        // folks are hanging on perform_work.
        if (si.status_ != Server_Info::ACTIVE && si.status_ != Server_Info::TERMINATED)
        {
          countdown.start ();

          ACE_Time_Value max_wait (0, 100000); // 100 msec
          this->orb_->perform_work (max_wait);

          countdown.stop ();
        }
      }

      if (si.status_ == Server_Info::TERMINATED)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::single_threaded_wait_for_callback - " <<
                            "Startup failed for LocalityManager " << si.uuid_ <<
                            "; process exited before activation.");
        throw ::Deployment::StartError ("locality_manager",
                                        "Failed to startup LocalityManager");
      }

      if (si.status_ == Server_Info::ACTIVE)
      {
        break;
      }

      if (timeout == ACE_Time_Value::zero)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::single_threaded_wait_for_callback - " <<
                            "Timed out while waiting for LocalityManager " << si.uuid_ <<
                            " to call back.");
        throw ::Deployment::StartError ("locality_manager",
                                        "Timed out waiting for LocalityManager");
      }
    }
  }

  void
  DAnCE_LocalityActivator_i::
    multi_threaded_wait_for_callback (Server_Info &si,
                                      ACE_Time_Value &timeout)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::multi_threaded_wait_for_callback");

    // convert timeout to absolute time for condition wait()
    timeout = timeout.to_absolute_time ();

    // Wait for a conditional variable
    ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                         guard,
                         si.mutex_,
                         CORBA::NO_RESOURCES ());

    while (si.status_ != Server_Info::ACTIVE)
      {
        if (si.condition_.wait (std::addressof(timeout)) == -1)
          {
            DANCEX11_LOG_CRITICAL ("DAnCE_LocalityActivator_i::multi_threaded_wait_for_callback - " <<
                                "Timed out while waiting for LocalityManager " << si.uuid_ <<
                                " to call back.");
            throw Deployment::StartError ("locality_manager",
                                          "timed out waiting for callback");
          }

        if (si.status_ == Server_Info::TERMINATED)
          {
            DANCEX11_LOG_CRITICAL ("DAnCE_LocalityActivator_i::multi_threaded_wait_for_callback - "
                                   "Startup failed for LocalityManager " << si.uuid_<<
                                   "; process exited before activation.");
            throw ::Deployment::StartError ("locality_manager",
                                            "Failed to startup LocalityManager");
          }
      }
  }

  void
  DAnCE_LocalityActivator_i::remove_locality_manager (
      const ::Deployment::DeploymentPlan &plan)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::remove_locality_manager");

    Safe_Server_Info info;

    {
      ACE_GUARD_THROW_EX ( ACE_SYNCH_MUTEX,
                           guard,
                           this->container_mutex_,
                           CORBA::NO_RESOURCES ());

      SERVER_INFOS::iterator i;
      for (i = this->server_infos_.begin ();
           i != this->server_infos_.end ();
           ++i)
        {
          if ((*i) && (*i)->plan_UUID_ == plan.UUID ())
            {
              info = *i;
              this->server_infos_.erase (i);
              break;
            }
        }
    }

    if (!info)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::remove_locality_manager - " \
                            "Failed to find equivalent LocalityManager under my management.");
        throw ::Deployment::StopError ("locality manager",
                                       "unable to stop locality manager");
      }

    if (info->status_ == Server_Info::TERMINATED)
      {
        DANCEX11_LOG_WARNING ("DAnCE_LocalityActivator_i::remove_locality_manager - " <<
                              "Locality Manager with UUID <" << info->uuid_ <<
                              "> already terminated");
        return;
      }

    if (info->activation_mode_ == DAnCEX11::LOCALITY_AM_LOCAL)
    {
      try
      {
        info->status_ = Server_Info::TERMINATED;
        info->dmh_->shutdown ();
      }
      catch (const CORBA::Exception &)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::remove_locality_manager - "
                            "Caught CORBA exception from LocalityManager handler for "
                            << info->uuid_ << " at shutdown.");
      }

    }
    else
    {
      try
        {
          DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::remove_locality_manager - "
                              "Calling shutdown on LocalityManager " << info->uuid_);
          info->status_ = Server_Info::TERMINATE_REQUESTED;
          info->ref_->shutdown ();
        }
      catch (const ::Deployment::StopError &)
        {
          DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::remove_locality_manager - "
                              "Received RemoveFailure exception from LocalityManager " << info->uuid_);
        }
    }

    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::remove_locality_manager - "
                        "LocalityManager " << info->uuid_ <<
                        " successfully shut down.");
  }

  DAnCE_LocalityActivator_i::Server_Child_Handler::Server_Child_Handler (
      Safe_Server_Info  si)
    : server_info_ (si)
    {}

  DAnCE_LocalityActivator_i::Server_Child_Handler::~Server_Child_Handler ()
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Server_Child_Handler::~Server_Child_Handler");
  }

  int DAnCE_LocalityActivator_i::Server_Child_Handler::handle_close (
      ACE_HANDLE, ACE_Reactor_Mask)
  {
    DANCEX11_LOG_TRACE ("DAnCE_LocalityActivator_i::Server_Child_Handler::handle_close");

    // X11_FUZZ: disable check_new_delete
    delete this;  // clean us up
    // X11_FUZZ: enable check_new_delete
    return 0;
  }

  int DAnCE_LocalityActivator_i::Server_Child_Handler::handle_exit (
      ACE_Process *proc)
  {
    DANCEX11_LOG_DEBUG ("DAnCE_LocalityActivator_i::Server_Child_Handler::handle_exit"
                        " - Locality Manager UUID " << this->server_info_->uuid_ <<
                        ", pid=" << int (proc->getpid ())
                        << ": " << int (proc->exit_code ()) );

    // this method is guaranteed to be called synchronously
    // so we can safely call anything we like

    // Check if the termination was requested, log an error if not.
    if (this->server_info_->status_  != Server_Info::TERMINATE_REQUESTED)
      {
        DANCEX11_LOG_ERROR ("DAnCE_LocalityActivator_i::Server_Child_Handler::handle_exit - "
                            "Error: Unexpected locality shutdown.");

        // This is either a failure to start or a runtime crash
        // On startup this is handled by the installation handler and the post_install()
        // interception point. In case of runtime crash we should have some means of
        // reporting this state to whoever is controlling this process.
        // TODO : MCO - Fault Management
      }

    // flag this process as exited
    this->server_info_->status_ = Server_Info::TERMINATED;

    // signal possibly waiting startup thread
    this->server_info_->condition_.signal ();

    return 0;
  }

}


