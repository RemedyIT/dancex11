/**
 * @file    dancex11_locality_deployment_handler.cpp
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 Locality DeploymentManagerHandler implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_locality_deployment_handler.h"
#include "dancex11_locality_manager_impl.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_deploymentmanagerhandlerS.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/handler/instance/plugin_conf.h"
#include "dancex11/logger/log.h"

#include "tao/x11/portable_server/portableserver_functions.h"

#include "ace/UUID.h"
#include "ace/OS_NS_sys_stat.h"

namespace DAnCEX11
{
  /*
   * Locality DLM implementation
   */

  class Locality_Launcher
      : public CORBA::servant_traits<DeploymentLaunchManager>::base_type
  {
  public:
    Locality_Launcher (
        Deployment::DeploymentPlan&& plan,
        IDL::traits<PortableServer::POA>::ref_type poa,
        std::string name,
        std::string uuid,
        std::shared_ptr<Plugin_Manager> plugins,
        IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
      : DeploymentLaunchManager (),
        plan_ (std::move (plan)),
        poa_ (std::move(poa)),
        name_ (std::move(name)),
        uuid_ (std::move(uuid)),
        plugins_ (std::move(plugins)),
        sh_ (std::move(sh))
    {}
    virtual ~Locality_Launcher () = default;

    Deployment::DeploymentPlan
    plan() override;

    void plan (const Deployment::DeploymentPlan& _v) override;

    void launch () override;

    void shutdown () override;

  private:
    bool find_locality_instance (
        Deployment::MonolithicDeploymentDescription*&,
        Deployment::InstanceDeploymentDescription*&);

    Deployment::DeploymentPlan plan_;
    IDL::traits<PortableServer::POA>::ref_type poa_;
    std::string name_;
    std::string uuid_;
    std::shared_ptr<Plugin_Manager> plugins_;
    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh_;
    CORBA::servant_reference<DAnCEX11::LocalityManager_i> lm_;
    IDL::traits< ::Deployment::ApplicationManager>::ref_type am_;
    IDL::traits< ::Deployment::Application>::ref_type app_;
  };

  Deployment::DeploymentPlan
  Locality_Launcher::plan()
  {
    return this->plan_;
  }

  void
  Locality_Launcher::plan (
      const ::Deployment::DeploymentPlan& _v)
  {
    this->plan_ = _v;
  }

  bool
  Locality_Launcher::find_locality_instance (
      Deployment::MonolithicDeploymentDescription*& lm_mdd,
      Deployment::InstanceDeploymentDescription*& lm_idd)
  {
    for (Deployment::InstanceDeploymentDescription& idd :
            this->plan_.instance ())
    {
      Deployment::MonolithicDeploymentDescription& mdd =
          this->plan_.implementation ()[idd.implementationRef ()];

      std::string impl_type;
      if (Utility::get_property_value (DAnCEX11::IMPL_TYPE,
                                       mdd.execParameter (),
                                       impl_type)
          &&
          impl_type == DAnCEX11::DANCE_LOCALITYMANAGER)
      {
        lm_mdd = &mdd;
        lm_idd = &idd;
        return true;
      }
    }
    return false;
  }

  void
  Locality_Launcher::launch ()
  {
    DANCEX11_LOG_TRACE ("Locality_Launcher::launch");

    DANCEX11_LOG_DEBUG ("Locality_Launcher::launch - " <<
                        "creating LocalityManager instance");

    // create Deployment::NodeManager instance
    this->lm_ =
        CORBA::make_reference<DAnCEX11::LocalityManager_i> (
              this->uuid_,
              this->plugins_,
              this->poa_,
              nullptr);

    Deployment::Properties prop;
    // see if we have an LM instance defined with which to configure
    Deployment::MonolithicDeploymentDescription* lm_mdd {};
    Deployment::InstanceDeploymentDescription* lm_idd {};
    if (this->find_locality_instance (lm_mdd, lm_idd))
    {
      // collect defined properties
      Utility::append_properties (prop, lm_mdd->execParameter ());
      Utility::append_properties (prop, lm_idd->configProperty ());
    }

    // configure locality manager
    this->lm_->configure (prop);

    DANCEX11_LOG_DEBUG ("Locality_Launcher::launch - "
                        "locality manager preparing plan [" <<
                        this->plan_.UUID () << "]");

    // have the node manager prepare the plan
    this->am_ =
        this->lm_->preparePlan (this->plan_, nullptr);

    DANCEX11_LOG_DEBUG ("Locality_Launcher::launch - "
                        "locality application manager starting plan [" <<
                        this->plan_.UUID () << "] launch");

    // have the locality application manager start the plan launching
    ::Deployment::Properties cfgProps;
    ::Deployment::Connections providedRefs;
    this->app_ = this->am_->startLaunch (cfgProps, providedRefs);

    DANCEX11_LOG_DEBUG ("Locality_Launcher::launch - "
                        "locality application finishing plan [" <<
                        this->plan_.UUID () << "] launch");

    // have the locality application finish the launch
    this->app_->finishLaunch (providedRefs, false);

    DANCEX11_LOG_DEBUG ("Locality_Launcher::launch - "
                        "locality application starting plan [" <<
                        this->plan_.UUID () << "]");

    // have the locality application start the application
    this->app_->start ();

    DANCEX11_LOG_DEBUG ("Locality_Launcher::launch - "
                        "locality application started plan [" <<
                        this->plan_.UUID () << "]");
  }

  void
  Locality_Launcher::shutdown ()
  {
    DANCEX11_LOG_TRACE ("Locality_Launcher::shutdown");

    DANCEX11_LOG_DEBUG ("Locality_Launcher::shutdown - "
                        "stopping locality application...");

    this->app_->stop ();

    DANCEX11_LOG_DEBUG ("Locality_Launcher::shutdown - "
                        "destroying locality application...");

    IDL::traits< ::Deployment::Application>::ref_type app = this->app_;
    this->app_.reset ();
    this->am_->destroyApplication (app);

    DANCEX11_LOG_DEBUG ("Locality_Launcher::shutdown - "
                        "destroying locality application manager...");

    IDL::traits< ::Deployment::ApplicationManager>::ref_type am = this->am_;
    this->am_.reset ();
    this->lm_->destroyManager (am);

    this->lm_.reset ();

    // call shutdown handler
    if (this->sh_)
      this->sh_->shutdown ();

    DANCEX11_LOG_DEBUG ("Locality_Launcher::shutdown - "
                        "shutting down ORB...");

    // shutdown ORB
    DAnCEX11::State::instance ()->orb ()->shutdown (false);

    DANCEX11_LOG_DEBUG ("Locality_Launcher::shutdown - "
                        "shut down ORB");
  }

  /*
   * Locality Deployment Handler implementation
   */

  const std::string LocalityDeploymentHandler::handler_type_ { "LocalityManager" };
  const std::string LocalityDeploymentHandler::config_file_ { "localitymanager.config" };

  LocalityDeploymentHandler::LocalityDeploymentHandler (
      std::string uuid,
      std::string activator)
    : uuid_ (uuid),
      activator_ (activator)
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::LocalityDeploymentHandler");

    ACE_Utils::UUID_GENERATOR::instance ()->init ();
  }

  LocalityDeploymentHandler::~LocalityDeploymentHandler ()
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::~LocalityDeploymentHandler");
  }

  std::string
  LocalityDeploymentHandler::handler_type()
  {
    return handler_type_;
  }

  void
  LocalityDeploymentHandler::configure (
      const std::string& config,
      const ::Deployment::Properties& prop,
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::configure");

    // set up deployment POA
    this->create_poas ();

    std::string cfg_file = config;
    // search for default config if none specified
    if (cfg_file.empty ())
    {
      ACE_stat  st;
      // first look if the default file is available in
      // the current working directory
      if (ACE_OS::stat (config_file_.c_str (), &st) == 0
#if defined (S_ISREG)
          &&
          S_ISREG (st.st_mode)
#endif
         )
      {
        cfg_file = config_file_;
      }
      else
      {
        // check $DANCEX11_ROOT/bin/<config_file_>
        // try to resolve DANCEX11_ROOT
        const char* dancex11_env = std::getenv ("DANCEX11_ROOT");
        if (dancex11_env)
          {
            std::string filename (dancex11_env);
            filename += ACE_DIRECTORY_SEPARATOR_STR_A;
            filename += "bin";
            filename += ACE_DIRECTORY_SEPARATOR_STR_A;
            filename += config_file_;
            if (ACE_OS::stat (filename.c_str (), &st) == 0
#if defined (S_ISREG)
          &&
          S_ISREG (st.st_mode)
#endif
         )
            {
              cfg_file = filename;
            }
          }
      }
      // if we were not able to find an accessible file
      // just set the default name and see if the config
      // loader has some brilliant way of finding it
      if (cfg_file.empty ())
      {
        cfg_file = config_file_;
      }
    }

    // load configuration
    Deployment::DeploymentPlan plugin_plan;
    Config_Loader cl;
    if (!cl.load_deployment_config (cfg_file, plugin_plan, this->launch_plan_))
    {
      DANCEX11_LOG_PANIC ("LocalityDeploymentHandler::configure - "
                          "Failed to load configuration from [" <<
                          cfg_file << "]");
      throw Deployment::StartError ("LocalityDeploymentHandler",
                                    std::string ("LocalityDeploymentHandler::configure - "
                                    "Failed to load configuration from [") +
                                    cfg_file + "]");
    }

    // generate a UUID for this locality if not provided
    if (this->uuid_.empty ())
    {
      DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::configure - "
                          "generating UUID");
      ACE_Utils::UUID uuid;
      ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID (uuid);
      this->uuid_ = uuid.to_string ()->c_str ();
    }

    // deploy plugins
    std::shared_ptr<Plugin_Manager> plugins = std::make_shared<Plugin_Manager> ();
    // pass on the properties passed by the DM
    plugins->set_configuration (prop);
    Plugin_Configurator pc;
    pc.load_from_plan (*plugins, plugin_plan);

    // keep configured plugin manager
    this->plugins_ = std::move (plugins);

    // register shutdown handler
    this->shutdown_handler_ = sh;
  }

  IDL::traits< ::Deployment::Deployment_Manager>::ref_type
  LocalityDeploymentHandler::activate_manager (
      const std::string& name)
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::activate_manager");

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                        "creating LocalityManager instance");

    // create Deployment::LocalityManager instance
    CORBA::servant_reference<DAnCEX11::LocalityManager_i> lm =
        CORBA::make_reference<DAnCEX11::LocalityManager_i> (
              this->uuid_,
              this->plugins_,
              this->root_poa_,
              this->shutdown_handler_);

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                        "LocalityManager servant instance allocated.");

    // Registering servant in poa
    PortableServer::ObjectId const oid =
        PortableServer::string_to_ObjectId (name);
    this->mng_poa_->activate_object_with_id (oid, lm);

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                        "LocalityManager servant instance activated as [" <<
                        name << "]");

    this->servant_id_ = std::move (oid);

    IDL::traits<CORBA::Object>::ref_type lm_obj =
        this->mng_poa_->id_to_reference (this->servant_id_);
    IDL::traits<DAnCEX11::LocalityManager>::ref_type lm_ref =
      IDL::traits<DAnCEX11::LocalityManager>::narrow (lm_obj);

    // in case we're passed a reference to an activator let's call it
    if (!this->activator_.empty ())
    {
      DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                          "Resolving Activator");
      IDL::traits<CORBA::Object>::ref_type obj =
        DAnCEX11::State::instance ()->orb ()->string_to_object (this->activator_);
      IDL::traits<LocalityManagerActivator>::ref_type sa =
        IDL::traits<LocalityManagerActivator>::narrow (obj);

      if (!sa)
      {
        DANCEX11_LOG_ERROR ("LocalityDeploymentHandler::activate_manager - " <<
                            "Failed to resolve Activator <" <<
                            this->activator_ << ">");
        throw Deployment::StartError (
            "LocalityDeploymentHandler",
            "Failed to resolve Locality Activator");
      }

      // Make callback.
      DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                          "Making callback on my Activator");

      try
      {
        // Callback to NodeApplication to get configuration
        Deployment::Properties config;
        sa->locality_manager_callback (lm_ref, this->uuid_, config);

        DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                            "Configuration received, got " << config.size () <<
                            " values");

        lm->configure (config);

        DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_manager - " <<
                            "Configuration complete for locality manager <" <<
                            this->uuid_ << ">");

        sa->configuration_complete (this->uuid_);
      }
      catch (const CORBA::BAD_PARAM &)
      {
        DANCEX11_LOG_ERROR ("LocalityDeploymentHandler::activate_manager - " <<
                            "The Activator reference provided pointed to the " <<
                            "wrong Activator");
        throw Deployment::StartError (
            "LocalityDeploymentHandler",
            "Bad Locality Activator reference");
      }
      catch (const CORBA::Exception &ex)
      {
        std::ostringstream err;
        err << ex;
        DANCEX11_LOG_ERROR ("LocalityDeploymentHandler::activate_manager - " <<
                            "Caught CORBA Exception while " <<
                            "calling Activator <" << err.str () << ">");
        throw Deployment::StartError ("LocalityDeploymentHandler", err.str ());
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("LocalityDeploymentHandler::activate_manager - " <<
                            "Caught exception while calling Activator");
        throw Deployment::StartError (
            "LocalityDeploymentHandler",
            "Caught exception while calling Locality Activator");
      }
    }

    // Return locality manager reference
    return lm_ref;
  }

  IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::ref_type
  LocalityDeploymentHandler::activate_launcher (
      const std::string& name)
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::activate_launcher");

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_launcher - " <<
                        "creating LocalityLauncher instance");

    // create Deployment::NodeManager instance
    CORBA::servant_reference<Locality_Launcher> ll =
        CORBA::make_reference<Locality_Launcher> (
              std::move (this->launch_plan_),
              this->root_poa_,
              name,
              this->uuid_,
              this->plugins_,
              this->shutdown_handler_);

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_launcher - " <<
                        "LocalityLauncher servant instance allocated.");

    // Registering servant in poa
    PortableServer::ObjectId const oid =
        PortableServer::string_to_ObjectId (name);
    this->mng_poa_->activate_object_with_id (oid, ll);

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_launcher - " <<
                        "LocalityLauncher servant instance activated as [" <<
                        name << "]");

    this->servant_id_ = std::move (oid);

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_launcher - " <<
                        "retrieving LocalityLauncher servant reference");

    // Return node launcher reference
    IDL::traits<CORBA::Object>::ref_type ll_obj = this->mng_poa_->id_to_reference (this->servant_id_);

    DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::activate_launcher - " <<
                        "narrowing LocalityLauncher servant reference");

    return IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::narrow (ll_obj);
  }

  void
  LocalityDeploymentHandler::shutdown ()
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::shutdown");

    // deactivate servant
    if (!this->servant_id_.empty ())
    {
      DANCEX11_LOG_DEBUG ("LocalityDeploymentHandler::shutdown - " <<
                          "deactivating servant instance");

      try
      {
        this->mng_poa_->deactivate_object (this->servant_id_);
      }
      catch (const CORBA::BAD_INV_ORDER&)
      {
        // Ok, ORB or POA already shutdown. Just ignore it
      }
    }

    this->launch_plan_ = {};

    this->plugins_.reset ();
  }

  void
  LocalityDeploymentHandler::create_poas ()
  {
    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::create_poas");

    IDL::traits<CORBA::ORB>::ref_type orb =
        DAnCEX11::State::instance ()->orb ();
    this->root_poa_ = DAnCEX11::State::instance ()->root_poa ();

    if (!orb || !this->root_poa_)
    {
      DANCEX11_LOG_PANIC ("LocalityDeploymentHandler::create_poas - "
                          "Deployment state uninitialized. "
                          "Missing ORB/RootPOA");

      throw Deployment::StartError ("LocalityDeploymentHandler",
                                    "Deployment state uninitialized. "
                                    "Missing ORB/RootPOA");
    }

    DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::create_poas - " \
                        "Obtaining the POAManager");
    IDL::traits<PortableServer::POAManager>::ref_type mgr = this->root_poa_->the_POAManager ();

    CORBA::PolicyList policies (2);

    try
      {
        DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::create_poas - " \
                            "Creating the \"Managers\" POA.");

        policies[0] = this->root_poa_->create_id_assignment_policy (PortableServer::IdAssignmentPolicyValue::USER_ID);
        policies[1] = this->root_poa_->create_lifespan_policy (PortableServer::LifespanPolicyValue::PERSISTENT);
        this->mng_poa_ = this->root_poa_->create_POA ("Managers",
                                          mgr,
                                          policies);
      }
    catch (const PortableServer::POA::AdapterAlreadyExists &)
      {
        DANCEX11_LOG_TRACE ("LocalityDeploymentHandler::create_poas - " \
                            "Using existing \"Managers\" POA");
        this->mng_poa_ = this->root_poa_->find_POA ("Managers", 0);
      }

    // Destroy the Policy objects.
    for (IDL::traits<CORBA::Policy>::ref_type _pol : policies)
    {
      if (_pol) _pol->destroy ();
    }
  }

}
