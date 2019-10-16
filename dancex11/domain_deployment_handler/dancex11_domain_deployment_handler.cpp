/**
 * @file    dancex11_domain_deployment_handler.cpp
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 Domain DeploymentManagerHandler implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_domain_deployment_handler.h"
#include "dancex11_execution_manager_impl.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_deploymentmanagerhandlerS.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/handler/instance/plugin_conf.h"
#include "dancex11/logger/log.h"

#include "tao/x11/portable_server/portableserver_functions.h"

#include "ace/OS_NS_sys_stat.h"

namespace DAnCEX11
{
  /*
   * Domain DLM implementation
   */

  class Domain_Launcher
      : public CORBA::servant_traits<DeploymentLaunchManager>::base_type
  {
  public:
    Domain_Launcher (
        Deployment::DeploymentPlan&& plan,
        std::vector<std::string> nodes,
        std::string cdd,
        std::string cddfmt,
        IDL::traits<PortableServer::POA>::ref_type poa,
        std::string name,
        std::string nc,
        std::shared_ptr<Plugin_Manager> plugins,
        IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
      : DeploymentLaunchManager (),
        plan_ (std::move (plan)),
        nodes_ (std::move (nodes)),
        cdd_ (std::move (cdd)),
        cddfmt_ (std::move (cddfmt)),
        poa_ (std::move(poa)),
        name_ (std::move(name)),
        nc_ (std::move(nc)),
        plugins_ (std::move(plugins)),
        sh_ (std::move(sh))
    {}
    virtual ~Domain_Launcher () = default;

    virtual
    Deployment::DeploymentPlan
    plan() override;

    virtual
    void
    plan (const Deployment::DeploymentPlan& _v) override;

    virtual
    void
    launch () override;

    virtual
    void
    shutdown () override;

  private:
    Deployment::DeploymentPlan plan_;
    std::vector<std::string> nodes_;
    std::string cdd_;
    std::string cddfmt_;
    IDL::traits<PortableServer::POA>::ref_type poa_;
    std::string name_;
    std::string nc_;
    std::shared_ptr<Plugin_Manager> plugins_;
    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh_;
    CORBA::servant_reference<DAnCEX11::ExecutionManager_Impl> em_;
    IDL::traits< ::Deployment::ApplicationManager>::ref_type am_;
    IDL::traits< ::Deployment::Application>::ref_type app_;
  };

  Deployment::DeploymentPlan
  Domain_Launcher::plan()
  {
    return this->plan_;
  }

  void
  Domain_Launcher::plan (
      const ::Deployment::DeploymentPlan& _v)
  {
    this->plan_ = _v;
  }

  void
  Domain_Launcher::launch ()
  {
    DANCEX11_LOG_TRACE ("Domain_Launcher::launch");

    DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - " <<
                        "creating DomainManager instance");

    // create Deployment::DomainManager instance
    this->em_ =
        CORBA::make_reference<DAnCEX11::ExecutionManager_Impl> (
              this->poa_,
              "ExecutionManager.Domain_Launcher",
              nc_,
              nullptr);

    // configure node mappings
    if (!this->cdd_.empty ())
    {
      this->em_->load_cdd (this->cdd_);
    }
    for (const std::string& nodemap : this->nodes_)
    {
      size_t const pos = nodemap.find ('=');
      if (std::string::npos == pos)
      {
        DANCEX11_LOG_ERROR ("Domain_Launcher::launch - " <<
                            "Domain Deployment handler received -n NODE without IOR");
        continue;
      }

      std::string node_name;
      std::string nm_ior;

      node_name = nodemap.substr (0, pos);
      nm_ior = nodemap.substr (pos + 1);

      DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - " <<
                          "Adding node \"" << node_name << "\" with IOR [" <<
                          nm_ior << "] to EM's map.");
      this->em_->add_node_manager (node_name, nm_ior);
    }

    DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                        "node manager preparing plan [" <<
                        this->plan_.UUID () << "]");

    // have the node manager prepare the plan
    this->am_ =
        this->em_->preparePlan (this->plan_, nullptr);

    DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                        "domain application manager starting plan [" <<
                        this->plan_.UUID () << "] launch");

    try
    {
      // have the domain application manager start the plan launching
      ::Deployment::Properties cfgProps;
      ::Deployment::Connections providedRefs;
      this->app_ = this->am_->startLaunch (cfgProps, providedRefs);

      DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                          "domain application finishing plan [" <<
                          this->plan_.UUID () << "] launch");

      // have the node application finish the launch
      this->app_->finishLaunch (providedRefs, false);

      DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                          "domain application starting plan [" <<
                          this->plan_.UUID () << "]");

      // have the node application start the application
      this->app_->start ();

      DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                          "domain application started plan [" <<
                          this->plan_.UUID () << "]");
    }
    catch (const CORBA::UserException&)
    {
      // attempt controlled shutdown
      DANCEX11_LOG_ERROR ("Domain_Launcher::launch - "
                          "Caught deployment exception while starting plan [" <<
                          this->plan_.UUID () << "]");
      try
      {
        if (this->app_)
        {
          DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                              "stopping domain application...");

          this->app_->stop ();

          DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                              "destroying domain application...");

          IDL::traits< ::Deployment::Application>::ref_type app = this->app_;
          this->app_.reset ();
          this->am_->destroyApplication (app);
        }
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("Domain_Launcher::launch - "
                            "Caught exception while attempting "
                            "controlled shutdown of failed plan launch - "
                            "destroyApplication on ApplicationManager.");
      }

      try
      {
        DANCEX11_LOG_DEBUG ("Domain_Launcher::launch - "
                            "destroying domain application manager...");

        IDL::traits< ::Deployment::ApplicationManager>::ref_type am = this->am_;
        this->am_.reset ();
        this->em_->destroyManager (am);

        this->em_.reset ();
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("Domain_Launcher::launch - "
                            "Caught exception while attempting "
                            "controlled shutdown of failed plan launch.");
      }

      // To prevent issues in the future.
      this->app_ = nullptr;
      this->em_ = nullptr;
      this->am_ = nullptr;

      throw;
    }
  }

  void
  Domain_Launcher::shutdown ()
  {
    DANCEX11_LOG_TRACE ("Domain_Launcher::shutdown");

    if (this->app_)
    {
      DANCEX11_LOG_DEBUG ("Domain_Launcher::shutdown - "
                          "stopping domain application...");

      this->app_->stop ();

      DANCEX11_LOG_DEBUG ("Domain_Launcher::shutdown - "
                          "destroying domain application...");

      IDL::traits< ::Deployment::Application>::ref_type app = this->app_;
      this->app_.reset ();
      this->am_->destroyApplication (app);
    }

    if (this->am_)
    {
      DANCEX11_LOG_DEBUG ("Domain_Launcher::shutdown - "
                          "destroying domain application manager...");

      IDL::traits< ::Deployment::ApplicationManager>::ref_type am = this->am_;
      this->am_.reset ();
      this->em_->destroyManager (am);
    }

    this->em_.reset ();

    // call shutdown handler
    if (this->sh_)
      this->sh_->shutdown ();

    DANCEX11_LOG_DEBUG ("Domain_Launcher::shutdown - "
                        "shutting down ORB...");

    // shutdown ORB
    DAnCEX11::State::instance ()->orb ()->shutdown (false);

    DANCEX11_LOG_DEBUG ("Domain_Launcher::shutdown - "
                        "shut down ORB");
  }

  /*
   * Domain Deployment Handler implementation
   */

  const std::string DomainDeploymentHandler::handler_type_ { "DomainManager" };
  const std::string DomainDeploymentHandler::config_file_ { "domainmanager.config" };

  DomainDeploymentHandler::DomainDeploymentHandler (
      std::vector<std::string> nodes,
      std::string cdd,
      std::string cddfmt)
    : nodes_ (std::move (nodes)),
      cdd_ (std::move (cdd)),
      cddfmt_ (std::move (cddfmt))
  {}

  std::string
  DomainDeploymentHandler::handler_type()
  {
    return handler_type_;
  }

  void
  DomainDeploymentHandler::configure (
      const std::string& config,
      const Deployment::Properties& prop,
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
  {
    DANCEX11_LOG_TRACE ("DomainDeploymentHandler::configure");

    // set up deployment POA
    this->create_poas ();

    // check for DOMAIN_NC property
    if (Utility::get_property_value (DAnCEX11::DOMAIN_NC, prop, this->domain_nc_))
    {
      DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::configure - " <<
                          "using provided domain naming context: [" <<
                          this->domain_nc_ << "]");
    }

    std::string cfg_file = config;
    // search for default config if none specified
    if (cfg_file.empty ())
    {
      ACE_stat st;
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
      DANCEX11_LOG_PANIC ("DomainDeploymentHandler::configure - "
                          "Failed to load configuration from [" <<
                          cfg_file << "]");
      throw Deployment::StartError ("DomainDeploymentHandler",
                                    std::string ("DomainDeploymentHandler::configure - "
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
  DomainDeploymentHandler::activate_manager (
      const std::string& name)
  {
    DANCEX11_LOG_TRACE ("DomainDeploymentHandler::activate_manager");

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_manager - " <<
                        "creating DomainManager instance");

    // create Deployment::DomainManager instance
    CORBA::servant_reference<DAnCEX11::ExecutionManager_Impl> em =
        CORBA::make_reference<DAnCEX11::ExecutionManager_Impl> (
              this->root_poa_,
              name,
              this->domain_nc_,
              this->shutdown_handler_);

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_manager - " <<
                        "ExecutionManager servant instance allocated.");

    // configure node mappings
    if (!this->cdd_.empty ())
    {
      em->load_cdd (this->cdd_, this->cddfmt_);
    }
    for (const std::string& nodemap : this->nodes_)
    {
      size_t const pos = nodemap.find ('=');
      if (std::string::npos == pos)
      {
        DANCEX11_LOG_ERROR ("DomainDeploymentHandler::activate_manager - " <<
                            "Domain Deployment handler received -n NODE without IOR");
        continue;
      }

      std::string node_name;
      std::string nm_ior;

      node_name = nodemap.substr (0, pos);
      nm_ior = nodemap.substr (pos + 1);

      DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_manager - " <<
                          "Adding node \"" << node_name << "\" with IOR [" <<
                          nm_ior << "] to EM's map.");
      em->add_node_manager (node_name, nm_ior);
    }

    // Registering servant in poa
    PortableServer::ObjectId const oid =
        PortableServer::string_to_ObjectId (name);
    this->mng_poa_->activate_object_with_id (oid, em);

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_manager - " <<
                        "ExecutionManager servant instance activated as [" <<
                        name << "]");

    this->servant_id_ = std::move (oid);

    // Return node manager reference
    IDL::traits<CORBA::Object>::ref_type em_obj =
        this->mng_poa_->id_to_reference (this->servant_id_);
    return IDL::traits< ::Deployment::Deployment_Manager>::narrow (em_obj);
  }

  IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::ref_type
  DomainDeploymentHandler::activate_launcher (
      const std::string& name)
  {
    DANCEX11_LOG_TRACE ("DomainDeploymentHandler::activate_launcher");

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_launcher - " <<
                        "creating DomainLauncher instance");

    // create Deployment::DomainManager instance
    CORBA::servant_reference<Domain_Launcher> dl =
        CORBA::make_reference<Domain_Launcher> (
              std::move (this->launch_plan_),
              std::move (this->nodes_),
              std::move (this->cdd_),
              std::move (this->cddfmt_),
              this->root_poa_,
              name,
              this->domain_nc_,
              this->plugins_,
              this->shutdown_handler_);

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_launcher - " <<
                        "DomainLauncher servant instance allocated.");

    // Registering servant in poa
    PortableServer::ObjectId const oid =
        PortableServer::string_to_ObjectId (name);
    this->mng_poa_->activate_object_with_id (oid, dl);

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_launcher - " <<
                        "DomainLauncher servant instance activated as [" <<
                        name << "]");

    this->servant_id_ = std::move (oid);

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_launcher - " <<
                        "retrieving DomainLauncher servant reference");

    // Return node launcher reference
    IDL::traits<CORBA::Object>::ref_type dl_obj =
        this->mng_poa_->id_to_reference (this->servant_id_);

    DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::activate_launcher - " <<
                        "narrowing DomainLauncher servant reference");

    return IDL::traits< ::DAnCEX11::DeploymentLaunchManager>::narrow (dl_obj);
  }

  void
  DomainDeploymentHandler::shutdown ()
  {
    DANCEX11_LOG_TRACE ("DomainDeploymentHandler::shutdown");

    // deactivate servant
    if (!this->servant_id_.empty ())
    {
      DANCEX11_LOG_DEBUG ("DomainDeploymentHandler::shutdown - " <<
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
  DomainDeploymentHandler::create_poas ()
  {
    DANCEX11_LOG_TRACE ("DomainDeploymentHandler::create_poas");

    IDL::traits<CORBA::ORB>::ref_type orb =
        DAnCEX11::State::instance ()->orb ();
    this->root_poa_ = DAnCEX11::State::instance ()->root_poa ();

    if (!orb || !this->root_poa_)
    {
      DANCEX11_LOG_PANIC ("DomainDeploymentHandler::create_poas - "
                          "Deployment state uninitialized. "
                          "Missing ORB/RootPOA");

      throw Deployment::StartError ("DomainDeploymentHandler",
                                    "Deployment state uninitialized. "
                                    "Missing ORB/RootPOA");
    }

    DANCEX11_LOG_TRACE ("DomainDeploymentHandler::create_poas - " \
                        "Obtaining the POAManager");
    IDL::traits<PortableServer::POAManager>::ref_type mgr = this->root_poa_->the_POAManager ();

    CORBA::PolicyList policies (2);

    try
      {
        DANCEX11_LOG_TRACE ("DomainDeploymentHandler::create_poas - " \
                            "Creating the \"Managers\" POA.");

        policies[0] = this->root_poa_->create_id_assignment_policy (PortableServer::IdAssignmentPolicyValue::USER_ID);
        policies[1] = this->root_poa_->create_lifespan_policy (PortableServer::LifespanPolicyValue::PERSISTENT);
        this->mng_poa_ = this->root_poa_->create_POA ("Managers",
                                         mgr,
                                         policies);
      }
    catch (const PortableServer::POA::AdapterAlreadyExists &)
      {
        DANCEX11_LOG_TRACE ("DomainDeploymentHandler::create_poas - " \
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
