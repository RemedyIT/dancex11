/**
 * @file    dancex11_node_deployment_handler.cpp
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 Node DeploymentManagerHandler implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_node_deployment_handler.h"
#include "dancex11_ndh_manager_impl.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_deploymentmanagerhandlerS.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/handler/instance/plugin_conf.h"
#include "dancex11/logger/log.h"

#include "tao/x11/portable_server/portableserver_functions.h"

#include "ace/OS_NS_sys_stat.h"

namespace DAnCEX11
{
  /*
   * Node DLM implementation
   */

  class Node_Launcher
      : public CORBA::servant_traits<DeploymentLaunchManager>::base_type
  {
  public:
    Node_Launcher (
        Deployment::DeploymentPlan&& plan,
        IDL::traits<PortableServer::POA>::ref_type poa,
        std::string name,
        std::string domain_nc,
        std::shared_ptr<Plugin_Manager> plugins,
        IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
      : DeploymentLaunchManager (),
        plan_ (std::move (plan)),
        poa_ (std::move(poa)),
        name_ (std::move(name)),
        domain_nc_ (std::move(domain_nc)),
        plugins_ (std::move(plugins)),
        sh_ (std::move(sh))
    {}
    virtual ~Node_Launcher () = default;

    Deployment::DeploymentPlan plan() override;

    void plan (const Deployment::DeploymentPlan& _v) override;

    void launch () override;

    void shutdown () override;

  private:
    Deployment::DeploymentPlan plan_;
    IDL::traits<PortableServer::POA>::ref_type poa_;
    std::string name_;
    std::string domain_nc_;
    std::shared_ptr<Plugin_Manager> plugins_;
    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh_;
    CORBA::servant_reference<DAnCEX11::NodeManager_Impl> nm_;
    IDL::traits< ::Deployment::NodeApplicationManager>::ref_type nam_;
    IDL::traits< ::Deployment::Application>::ref_type app_;
  };

  Deployment::DeploymentPlan
  Node_Launcher::plan()
  {
    return this->plan_;
  }

  void
  Node_Launcher::plan (
      const ::Deployment::DeploymentPlan& _v)
  {
    this->plan_ = _v;
  }

  void
  Node_Launcher::launch ()
  {
    DANCEX11_LOG_TRACE ("Node_Launcher::launch");

    DANCEX11_LOG_DEBUG ("Node_Launcher::launch - " <<
                        "creating NodeManager instance");

    // create Deployment::NodeManager instance
    this->nm_ =
        CORBA::make_reference<DAnCEX11::NodeManager_Impl> (
              this->poa_,
              "NodeManager.Node_Launcher",
              this->domain_nc_,
              this->plugins_,
              nullptr);


    DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                        "node manager preparing plan [" <<
                        this->plan_.UUID () << "]");

    // have the node manager prepare the plan
    this->nam_ =
        IDL::traits< ::Deployment::NodeApplicationManager>::narrow (this->nm_->preparePlan (this->plan_, nullptr));

    DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                        "node application manager starting plan [" <<
                        this->plan_.UUID () << "] launch");

    try
    {
      // have the node application manager start the plan launching
      ::Deployment::Properties cfgProps;
      ::Deployment::Connections providedRefs;
      this->app_ = this->nam_->startLaunch (cfgProps, providedRefs);

      DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                          "node application finishing plan [" <<
                          this->plan_.UUID () << "] launch");

      // have the node application finish the launch
      this->app_->finishLaunch (providedRefs, false);

      DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                          "node application starting plan [" <<
                          this->plan_.UUID () << "]");

      // have the node application start the application
      this->app_->start ();

      DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                          "node application started plan [" <<
                          this->plan_.UUID () << "]");
    }
    catch (const CORBA::UserException&)
    {
      // attempt controlled shutdown
      DANCEX11_LOG_ERROR ("Node_Launcher::launch - "
                          "Caught deployment exception while starting plan [" <<
                          this->plan_.UUID () << "]");
      try
      {
        if (this->app_)
        {
          DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                              "stopping node application...");

          this->app_->stop ();

          DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                              "destroying node application...");

          IDL::traits< ::Deployment::Application>::ref_type app = this->app_;
          this->app_.reset ();
          this->nam_->destroyApplication (app);
        }

        DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                            "destroying node application manager...");

        IDL::traits< ::Deployment::NodeApplicationManager>::ref_type nam = this->nam_;
        this->nam_.reset ();
        this->nm_->destroyManager (nam);

        this->nm_.reset ();
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("Node_Launcher::launch - "
                            "Caught exception while attempting "
                            "controlled shutdown of failed plan launch.");
      }

      throw;
    }
  }

  void
  Node_Launcher::shutdown ()
  {
    DANCEX11_LOG_TRACE ("Node_Launcher::shutdown");

    if (this->app_)
    {
      DANCEX11_LOG_DEBUG ("Node_Launcher::shutdown - "
                          "stopping node application...");

      this->app_->stop ();

      DANCEX11_LOG_DEBUG ("Node_Launcher::shutdown - "
                          "destroying node application...");

      IDL::traits< ::Deployment::Application>::ref_type app = this->app_;
      this->app_.reset ();
      this->nam_->destroyApplication (app);
    }

    if (this->nam_)
    {
      DANCEX11_LOG_DEBUG ("Node_Launcher::shutdown - "
                          "destroying node application manager...");

      IDL::traits< ::Deployment::NodeApplicationManager>::ref_type nam = this->nam_;
      this->nam_.reset ();
      this->nm_->destroyManager (nam);
    }

    this->nm_.reset ();

    // call shutdown handler
    if (this->sh_)
      this->sh_->shutdown ();

    DANCEX11_LOG_DEBUG ("Node_Launcher::shutdown - "
                        "shutting down ORB...");

    // shutdown ORB
    DAnCEX11::State::instance ()->orb ()->shutdown (false);

    DANCEX11_LOG_DEBUG ("Node_Launcher::shutdown - "
                        "shut down ORB");
  }

  /*
   * Node Deployment Handler implementation
   */

  const std::string NodeDeploymentHandler::handler_type_ { "NodeManager" };
  const std::string NodeDeploymentHandler::config_file_ { "nodemanager.config" };

  std::string
  NodeDeploymentHandler::handler_type()
  {
    return handler_type_;
  }

  void
  NodeDeploymentHandler::configure (
      const std::string& config,
      const ::Deployment::Properties& prop,
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
  {
    DANCEX11_LOG_TRACE ("NodeDeploymentHandler::configure");

    // set up deployment POA
    this->create_poas ();

    // check for DOMAIN_NC property
    if (Utility::get_property_value (DAnCEX11::DOMAIN_NC, prop, this->domain_nc_))
    {
      DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::configure - " <<
                          "using provided domain naming context: [" <<
                          this->domain_nc_ << "]");
    }

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
      DANCEX11_LOG_PANIC ("NodeDeploymentHandler::configure - "
                          "Failed to load configuration from [" <<
                          cfg_file << "]");
      throw Deployment::StartError ("NodeDeploymentHandler",
                                    std::string ("NodeDeploymentHandler::configure - "
                                    "Failed to load configuration from [") +
                                    cfg_file + "]");
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
  NodeDeploymentHandler::activate_manager (
      const std::string& name)
  {
    DANCEX11_LOG_TRACE ("NodeDeploymentHandler::activate_manager");

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_manager - " <<
                        "creating NodeManager instance");

    // create Deployment::NodeManager instance
    CORBA::servant_reference<DAnCEX11::NodeManager_Impl> nm =
        CORBA::make_reference<DAnCEX11::NodeManager_Impl> (
              this->root_poa_,
              name,
              this->domain_nc_,
              this->plugins_,
              this->shutdown_handler_);

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_manager - " <<
                        "NodeManager servant instance allocated.");

    // Registering servant in poa
    PortableServer::ObjectId const oid =
        PortableServer::string_to_ObjectId (name);
    this->mng_poa_->activate_object_with_id (oid, nm);

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_manager - " <<
                        "NodeManager servant instance activated as [" <<
                        name << "]");

    this->servant_id_ = std::move (oid);

    // Return node manager reference
    IDL::traits<CORBA::Object>::ref_type nm_obj =
        this->mng_poa_->id_to_reference (this->servant_id_);
    return IDL::traits< ::Deployment::Deployment_Manager>::narrow (nm_obj);
  }

  IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::ref_type
  NodeDeploymentHandler::activate_launcher (
      const std::string& name)
  {
    DANCEX11_LOG_TRACE ("NodeDeploymentHandler::activate_launcher");

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_launcher - " <<
                        "creating NodeLauncher instance");

    // create Deployment::NodeManager instance
    CORBA::servant_reference<Node_Launcher> nl =
        CORBA::make_reference<Node_Launcher> (
              std::move (this->launch_plan_),
              this->root_poa_,
              name,
              this->domain_nc_,
              this->plugins_,
              this->shutdown_handler_);

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_launcher - " <<
                        "NodeLauncher servant instance allocated.");

    // Registering servant in poa
    PortableServer::ObjectId const oid =
        PortableServer::string_to_ObjectId (name);
    this->mng_poa_->activate_object_with_id (oid, nl);

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_launcher - " <<
                        "NodeLauncher servant instance activated as [" <<
                        name << "]");

    this->servant_id_ = std::move (oid);

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_launcher - " <<
                        "retrieving NodeLauncher servant reference");

    // Return node launcher reference
    IDL::traits<CORBA::Object>::ref_type nl_obj =
        this->mng_poa_->id_to_reference (this->servant_id_);

    DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::activate_launcher - " <<
                        "narrowing NodeLauncher servant reference");

    return IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::narrow (nl_obj);
  }

  void
  NodeDeploymentHandler::shutdown ()
  {
    DANCEX11_LOG_TRACE ("NodeDeploymentHandler::shutdown");

    // deactivate servant
    if (!this->servant_id_.empty ())
    {
      DANCEX11_LOG_DEBUG ("NodeDeploymentHandler::shutdown - " <<
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
  NodeDeploymentHandler::create_poas ()
  {
    DANCEX11_LOG_TRACE ("NodeDeploymentHandler::create_poas");

    IDL::traits<CORBA::ORB>::ref_type orb =
        DAnCEX11::State::instance ()->orb ();
    this->root_poa_ = DAnCEX11::State::instance ()->root_poa ();

    if (!orb || !this->root_poa_)
    {
      DANCEX11_LOG_PANIC ("NodeDeploymentHandler::create_poas - "
                          "Deployment state uninitialized. "
                          "Missing ORB/RootPOA");

      throw Deployment::StartError ("NodeDeploymentHandler",
                                    "Deployment state uninitialized. "
                                    "Missing ORB/RootPOA");
    }

    DANCEX11_LOG_TRACE ("NodeDeploymentHandler::create_poas - " \
                        "Obtaining the POAManager");
    IDL::traits<PortableServer::POAManager>::ref_type mgr = this->root_poa_->the_POAManager ();

    CORBA::PolicyList policies (2);

    try
      {
        DANCEX11_LOG_TRACE ("NodeDeploymentHandler::create_poas - " \
                            "Creating the \"Managers\" POA.");

        policies[0] = this->root_poa_->create_id_assignment_policy (PortableServer::IdAssignmentPolicyValue::USER_ID);
        policies[1] = this->root_poa_->create_lifespan_policy (PortableServer::LifespanPolicyValue::PERSISTENT);
        this->mng_poa_ = this->root_poa_->create_POA ("Managers",
                                         mgr,
                                         policies);
      }
    catch (const PortableServer::POA::AdapterAlreadyExists &)
      {
        DANCEX11_LOG_TRACE ("NodeDeploymentHandler::create_poas - " \
                            "Using existing \"Managers\" POA");
        this->mng_poa_ = this->root_poa_->find_POA ("Managers", false);
      }

    // Destroy the Policy objects.
    for (IDL::traits<CORBA::Policy>::ref_type _pol : policies)
    {
      if (_pol) _pol->destroy ();
    }
  }

}
