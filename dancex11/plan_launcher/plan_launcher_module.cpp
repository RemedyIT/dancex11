// -*- C++ -*-
/**
 * @file   plan_launcher_module.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "ace/Get_Opt.h"
#include "ace/Service_Config.h"
#include "ace/Dynamic_Service.h"
#include "ace/Time_Value.h"
#include "ace/OS_NS_sys_stat.h"

#include "dancex11/logger/log.h"
#include "dancex11/deployment/deployment_executionmanagerC.h"
#include "dancex11/deployment/deployment_nodemanagerC.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/handler/instance/plugin_conf.h"

#include "plan_launcher_module.h"
#include "launcher_impl.h"

#include "orbsvcs/orbsvcs/naming_server/CosNamingC.h"

#include <fstream>
#include <sstream>
#include <thread>

namespace DAnCEX11
{
  namespace PlanLauncher
  {
    bool
    write_IOR (const std::string& ior_file_name, const std::string& ior)
    {
      std::ofstream os (ior_file_name);

      if (os)
      {
        os << ior;
        os.close ();
        return true;
      }
      return false;
    }

    std::string
    read_IOR (const std::string& ior_file_name)
    {
      std::ifstream is (ior_file_name);
      std::string ior;
      if (is)
      {
        is >> ior;
        is.close ();
      }
      return ior;
    }
  }

  const char* PlanLauncher_Module::SOptions::orb_listen_opt_ = "-ORBListenEndpoints";
  const char* PlanLauncher_Module::SOptions::svcname_prefix_ = "DeploymentManager";
  const std::vector<std::string> PlanLauncher_Module::SOptions::svc_kinds { {"DomainManager"}, {"NodeManager"}, {"LocalityManager"} };

  PlanLauncher_Module::SOptions::SOptions()
  {
    const char* dancex11_env = std::getenv ("DANCEX11_ROOT");

    if (!dancex11_env)
    {
      DANCEX11_LOG_WARNING ("PlanLauncher_Module::SOptions::SOptions - no DANCEX11_ROOT set.");
    }

    this->dm_addr_ = "localhost";
  }

  PlanLauncher_Module::PlanLauncher_Module ()
  {
    DANCEX11_LOG_TRACE ("PlanLauncher_Module::PlanLauncher_Module");
  }

  PlanLauncher_Module::~PlanLauncher_Module ()
  {
    DANCEX11_LOG_TRACE ("PlanLauncher_Module::~PlanLauncher_Module");
  }

  const char *
  PlanLauncher_Module::usage ()
  {
    DANCEX11_LOG_TRACE ("PlanLauncher_Module::usage");
    return "\ndancex11_plan_launcher -l PLAN [-f FMT] [-c CFG] [-n NAME] [-r IOR] [-a ADDR] [-p PORT] [-d NC] [-t TIMEOUT] [-m IORFILE] [-i IORFILE]\n"
             " or\n"
             "dancex11_plan_launcher -x [PLAN] [-u UUID] [--force] [-f FMT] [-n NAME] [-r IOR] [-a ADDR] [-p PORT] [-d NC] [-t TIMEOUT] [-m IOR] [-i IOR]\n\n"
      "Plan Launcher Options:\n"
      "\t-l|--launch PLAN\t\t Launch the deployment plan read from PLAN.\n"
      "\t-f|--plan-format FMT\t\t Specify the deployment plan format to read from PLAN.\n"
      "\t-n|--mgr-name NAME\t\t Specify the name for the deployment manager instance\n"
      "\t-r|--mgr-ior IOR\t\t Specify the IOR for the deployment manager instance\n"
      "\t-p|--mgr-port PORT\t\t Specify the port the Deployment Manager listens on.\n"
      "\t-a|--mgr-address ADDR\t\t Specify the network interface address on which the Deployment Manager\n"\
                     "\t\t\t\t\t listens (default 'localhost').\n"
      "\t-d|--deployment-nc NC\t\t Specify naming context for deployment manager instance registration.\n"
      "\t-t|--timeout SEC\t\t Specify number of seconds to wait for valid deployment manager reference.\n"
      "\t-c|--config CFG\t\t\t Provide a deployment configuration file used to initialize the Plan Launcher.\n"
      "\t-x|--teardown [PLAN]\t\t Teardown a previously launched plan.\n"
      "\t   --force\t\t\t Force complete teardown in case of errors.\n"
      "\t-u|--uuid UUID\t\t\t Specify UUID of plan to tear down (required if no PLAN specified).\n"
      "\t-m|--am-ior IOR\t\t\t Specify IOR(-file) of Application Manager to write on launch or resolve for teardown.\n"
      "\t-i|--app-ior IOR\t\t Specify IOR(-file) of Application instance to write on launch or resolve for teardown.\n"
      "\t-h|--help\t\t\t print this help message\n";
  }

  bool
  PlanLauncher_Module::parse_args (int argc, char * argv[])
  {
    DANCEX11_LOG_TRACE ("PlanLauncher_Module::parse_args");

    ACE_Get_Opt get_opts (argc,
                          argv,
                          ACE_TEXT("n:d:c:a:p:l:f:r:t:x::u:m:i:h"),
                          1,
                          0,
                          ACE_Get_Opt::RETURN_IN_ORDER);

    get_opts.long_option (ACE_TEXT("mgr-name"), 'n', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("mgr-ior"), 'r', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("deployment-nc"), 'd', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("config"), 'c', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("mgr-addr"), 'a', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("mgr-port"), 'p', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("launch"), 'l', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("plan-format"), 'f', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("teardown"), 'x', ACE_Get_Opt::ARG_OPTIONAL);
    get_opts.long_option (ACE_TEXT("force"), ACE_Get_Opt::NO_ARG);
    get_opts.long_option (ACE_TEXT("uuid"), 'u', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("timeout"), 't', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("am-ior"), 'm', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("app-ior"), 'i', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("help"), 'h', ACE_Get_Opt::NO_ARG);

    int c {}, plan_ind {-1};
    while ( (c = get_opts ()) != -1)
    {
      switch (c)
      {
      case 'n':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - " <<
                            "Provided Manager name: " <<
                            get_opts.opt_arg ());
        this->options_.dm_name_ = get_opts.opt_arg ();
        break;

      case 'r':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - " <<
                            "Provided deployment manager reference: " <<
                            get_opts.opt_arg ());
        this->options_.dm_ior_ = get_opts.opt_arg ();
        break;

      case 't':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - " <<
                            "Provided resolving timeout: " <<
                            get_opts.opt_arg ());
        this->options_.dm_timeout_ = static_cast<uint32_t> (ACE_OS::atoi (get_opts.opt_arg ()));
        break;

      case 'm':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - " <<
                            "Provided application manager reference/filename: " <<
                            get_opts.opt_arg ());
        this->options_.am_ior_ = get_opts.opt_arg ();
        break;

      case 'i':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - " <<
                            "Provided application reference/filename: " <<
                            get_opts.opt_arg ());
        this->options_.app_ior_ = get_opts.opt_arg ();
        break;

      case 'd':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Binding to provided Deployment Naming Context: " <<
                            get_opts.opt_arg ());
        this->options_.deployment_nc_ = get_opts.opt_arg ();
        break;

      case 'c':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - " <<
                            "Using configuration file for plan launcher: " <<
                            get_opts.opt_arg ());
        this->options_.pl_config_ = ACE_TEXT_ALWAYS_CHAR (get_opts.opt_arg ());
        break;

      case 'a':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Manager listening at address: " <<
                            get_opts.opt_arg ());
        this->options_.dm_addr_ = get_opts.opt_arg ();
        break;

      case 'p':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Manager listening on port: " <<
                            get_opts.opt_arg ());
        this->options_.dm_port_ =
            static_cast<uint16_t> (ACE_OS::atoi (get_opts.opt_arg ()));
        break;

      case 'l':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Launching plan: " << get_opts.opt_arg ());
        this->options_.plan_file_ = get_opts.opt_arg ();
        break;

      case 'f':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Loading plan with format: " <<
                            get_opts.opt_arg ());
        this->options_.plan_fmt_ = get_opts.opt_arg ();
        break;

      case 'x':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Teardown running plan");
        this->options_.teardown_ = true;
        if (get_opts.opt_arg ())
        {
          DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                              "Teardown plan: " << get_opts.opt_arg ());
          this->options_.plan_file_ = get_opts.opt_arg ();
        }
        else
        {
          plan_ind = get_opts.opt_ind ();
        }
        break;

      case 'u':
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                            "Plan UUID: " << get_opts.opt_arg ());
        this->options_.plan_uuid_ = get_opts.opt_arg ();
        break;

      //X11_FUZZ: disable check_cout_cerr
      case 'h':
        std::cerr << this->usage () << std::endl << argv [0] << " -" << char(c) << std::endl;
        DANCEX11_LOG_INFO (argv [0] << " -" << char(c));
        return false;
        break;
      //X11_FUZZ: enable check_cout_cerr

      case 0:
        if (ACE_OS::strcmp (get_opts.long_option (),
                            ACE_TEXT("force")) == 0)
        {
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                              "Force teardown ");
          this->options_.force_ = true;
        }
        else
        {
          DANCEX11_LOG_WARNING ("PlanLauncher_Module::parse_args - "
                                "Ignoring unknown long option: " <<
                                get_opts.long_option ());
        }
        break;

      case 1:
        if ((plan_ind+1) == get_opts.opt_ind ())
        {
          DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                              "Teardown plan: " << get_opts.opt_arg ());
          this->options_.plan_file_ = get_opts.opt_arg ();
        }
        else
        {
          DANCEX11_LOG_DEBUG ("PlanLauncher_Module::parse_args - "
                              "Collecting other option: " << get_opts.opt_arg ());
          this->options_.other_opts_.push_back (get_opts.opt_arg ());
        }
        break;

      case '?':
        {
          DANCEX11_LOG_DEBUG ("DAnCEX11_NodeManager_Module::parse_args - "
                              "Collecting foreign switch: " << argv[get_opts.opt_ind ()]);
          this->options_.other_opts_.push_back (argv[get_opts.opt_ind ()]);
          // skip to next argv entry
          for (int aix = get_opts.opt_ind (); get_opts.opt_ind () == aix ;) get_opts ();
        }
        break;

      default:
        DANCEX11_LOG_WARNING ("PlanLauncher_Module::parse_args - ignoring incorrect option: "
                              << (get_opts.opt_arg () ? get_opts.opt_arg () : ""));
      }
    }

    return true;
  }

  bool
  PlanLauncher_Module::init (int argc,
                             char *argv[])
  {
    try
    {
      if (!this->parse_args (argc, argv))
      {
        return false;
      }

      DANCEX11_LOG_TRACE ("PlanLauncher_Module::init - " <<
                          "Initializing ORB arguments.");

      // setup ORB arguments
      int orb_argc {};
      std::unique_ptr<const char*[]> orb_argv = this->create_orb_args (orb_argc, argv[0]);

      DANCEX11_LOG_TRACE ("PlanLauncher_Module::init - " <<
                          "Initializing ORB");

      if (!DAnCEX11::State::instance ()->initialize (
              orb_argc, const_cast<char**> (orb_argv.get ())))
      {
        DANCEX11_LOG_PANIC ("PlanLauncher_Module::init - " <<
                            "Failed to initialize ORB");
        return false;
      }

      IDL::traits<CORBA::ORB>::ref_type orb =
          DAnCEX11::State::instance ()->orb ();

      if (!this->configure ())
      {
        DANCEX11_LOG_PANIC ("PlanLauncher_Module::init - " <<
                            "Failed to configure Plan Launcher.");
        return false;
      }

      if (!this->resolve_manager (orb))
      {
        DANCEX11_LOG_PANIC ("PlanLauncher_Module::init - " <<
                            "Failed to resolve Deployment Manager reference.");
        return false;
      }
    }
    catch (const CORBA::Exception& ex)
    {
      DANCEX11_LOG_PANIC ("PlanLauncher_Module::init - " << ex);
      return false;
    }

    return true;
  }

  bool
  PlanLauncher_Module::run ()
  {
    DANCEX11_LOG_TRACE ("PlanLauncher_Module::run");

    std::unique_ptr<Launcher_Base> launcher;
    if (this->svc_objectkind_ == "DomainManager")
    {
      launcher = std::make_unique<EM_Launcher> (
          this->options_, this->svc_objectkind_);
    }
    else if (this->svc_objectkind_ == "NodeManager")
    {
      launcher = std::make_unique<NM_Launcher> (
          this->options_, this->svc_objectkind_);
    }
    else
    {
      launcher = std::make_unique<LM_Launcher> (
          this->options_, this->svc_objectkind_);
    }

    DANCEX11_LOG_DEBUG ("PlanLauncher_Module::run - " <<
                        "plan launcher started for " << svc_objectkind_);

    if (this->options_.teardown_)
    {
      DANCEX11_LOG_DEBUG ("PlanLauncher_Module::run - " <<
                          "stopping running plan...");

      if (!this->options_.plan_file_.empty ())
      {
        // load plan
        DANCEX11_LOG_DEBUG ("PlanLauncher_Module::run - " <<
                            "reading plan from " <<
                            this->options_.plan_file_);

        // read plan from file
        Deployment::DeploymentPlan plan;
        if (!DAnCEX11::Plan_Loader::instance ()->read_plan (
            this->options_.plan_file_,
            plan,
            this->options_.plan_fmt_))
        {
          DANCEX11_LOG_PANIC ("PlanLauncher_Module::run - "
                              "failed to read plan from " <<
                              this->options_.plan_file_);
          return false;
        }

        this->options_.plan_uuid_ = plan.UUID ();
      }

      if (this->options_.plan_uuid_.empty ())
      {
        DANCEX11_LOG_PANIC ("PlanLauncher_Module::run - " <<
                            "missing plan UUID to identify "
                            "plan to tear down");
        return false;
      }

      return launcher->teardown_plan (this->dm_);
    }
    else
    {
      return launcher->launch_plan (this->dm_);
    }
  }

  std::unique_ptr<const char*[]>
  PlanLauncher_Module::create_orb_args (int &argc, const char* argv0)
  {
    std::unique_ptr<const char*[]> orb_argv;
    int narg = 0;
    // initialize ORB arguments with program
    argc = ACE_Utils::truncate_cast<int> (this->options_.other_opts_.size () + 1);
    orb_argv = std::make_unique<const char*[]> (argc);
    orb_argv.get ()[narg++] = argv0;
    // append all other remaining options
    for (const std::string& a : this->options_.other_opts_)
    {
      orb_argv.get ()[narg++] = a.c_str ();
    }
    return orb_argv;
  }

  bool PlanLauncher_Module::configure ()
  {
    static std::string def_cfg = "plan_launcher.config";

    std::string config_file = this->options_.pl_config_;
    // search for default config if none specified
    if (config_file.empty ())
    {
      ACE_stat  st;
      // first look if the default file is available in
      // the current working directory
      if (ACE_OS::stat (def_cfg.c_str (), &st) == 0
#if defined (S_ISREG)
          &&
          S_ISREG (st.st_mode)
#endif
         )
      {
        config_file = def_cfg;
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
            filename += def_cfg;
            if (ACE_OS::stat (filename.c_str (), &st) == 0
#if defined (S_ISREG)
              &&
              S_ISREG (st.st_mode)
#endif
              )
            {
              config_file = filename;
            }
          }
      }
      // if we were not able to find an accessible file
      // just set the default name and see if the config
      // loader has some brilliant way of finding it
      if (config_file.empty ())
      {
        config_file = def_cfg;
      }
    }

    // load configuration
    Deployment::DeploymentPlan plugin_plan;
    DAnCEX11::Config_Loader cl;
    if (!cl.load_plugins_config (config_file, plugin_plan))
    {
      DANCEX11_LOG_CRITICAL ("PlanLauncher_Module::configure - "
                             "Failed to load configuration from [" <<
                             config_file << "]");
      return false;
    }

    // deploy plugins
    DAnCEX11::Plugin_Configurator pc;
    pc.load_from_plan (this->plugins_, plugin_plan);

    return true;
  }

  bool
  PlanLauncher_Module::resolve_manager (
      IDL::traits<CORBA::ORB>::ref_type orb)
  {
    DANCEX11_LOG_TRACE ("PlanLauncher_Module::resolve_manager");

    IDL::traits<CosNaming::NamingContext>::ref_type instance_nc;
    std::string last_error;
    this->svc_objectid_ = this->options_.dm_name_;
    if (this->svc_objectid_.empty ())
    {
      this->svc_objectid_ = SOptions::svcname_prefix_;
    }

    ACE_Time_Value timeout (ACE_Time_Value ().now () +
                            ACE_Time_Value (this->options_.dm_timeout_));

    do
    {
      IDL::traits<CORBA::Object>::ref_type dm_obj;

      // should we try to resolve specified IOR?
      if (!this->options_.dm_ior_.empty ())
      {
        DANCEX11_LOG_TRACE ("PlanLauncher_Module::resolve_manager - " <<
                            "resolving deployment manager reference :" <<
                            this->options_.dm_ior_);

        try
        {
          // attempt to resolve DM handler reference
          dm_obj = orb->string_to_object (this->options_.dm_ior_);
        }
        catch (const CORBA::Exception &ex)
        {
          std::ostringstream os;
          os << ex;

          DANCEX11_LOG_WARNING ("PlanLauncher_Module::resolve_manager - " <<
                                "Caught CORBA Exception resolving deployment manager reference: " <<
                                os.str ());
          last_error = os.str ();
        }
      }

      if (!dm_obj)
      {
        for (const std::string &svc_kind : SOptions::svc_kinds)
        {
          std::string svc_name = this->svc_objectid_ + "." + svc_kind;

          // try to get IOR from corbaloc URL based on specific endpoint port (and optionally address)
          if (this->options_.dm_port_ > 0)
          {

            // construct corbaloc
            std::ostringstream os;
            os << "corbaloc:iiop:" << this->options_.dm_addr_
               << ":"
               << this->options_.dm_port_
               << "/" << svc_name;

            DANCEX11_LOG_TRACE ("PlanLauncher_Module::resolve_manager - " <<
                                "resolving deployment manager reference :" <<
                                os.str ());

            try
            {
              // attempt to resolve deployment launch manager reference
              dm_obj = orb->string_to_object (os.str ());
            }
            catch (const CORBA::Exception &ex)
            {
              std::ostringstream os;
              os << ex;

              DANCEX11_LOG_WARNING ("PlanLauncher_Module::resolve_manager - " <<
                                    "Caught CORBA Exception resolving deployment manager reference: " <<
                                    os.str ());
              last_error = os.str ();
            }
          }

          // should we try to resolve IOR from NC?
          if (!dm_obj && !this->options_.deployment_nc_.empty ())
          {
            DANCEX11_LOG_DEBUG ("PlanLauncher_Module::resolve_manager - "
                                "Resolving deployment manager from Deployment NC : " <<
                                this->options_.deployment_nc_);

            try
            {
              if (!instance_nc)
              {
                DANCEX11_LOG_TRACE ("PlanLauncher_Module::resolve_manager - "
                                    "Resolving DeploymentNC.");

                IDL::traits<CORBA::Object>::ref_type inc_obj =
                    orb->string_to_object (this->options_.deployment_nc_);
                if (inc_obj)
                {
                  instance_nc =
                      IDL::traits<CosNaming::NamingContext>::narrow (inc_obj);
                }
                else
                {
                  DANCEX11_LOG_ERROR ("PlanLauncher_Module::resolve_manager - "
                                      "DeploymentNC resolved to nil.");
                }
              }

              if (instance_nc)
              {
                // Resolving deployment manager reference from DeploymentNC
                DANCEX11_LOG_DEBUG ("PlanLauncher_Module::resolve_manager - " <<
                                    "Resolving DM from NC as \"" <<
                                    svc_name << "\".");
                CosNaming::Name name;
                name.push_back (CosNaming::NameComponent (this->svc_objectid_,
                                                          svc_kind));
                dm_obj = instance_nc->resolve (name);
              }
              else
              {
                DANCEX11_LOG_ERROR ("PlanLauncher_Module::resolve_manager - "
                                    "Narrow to NamingContext returned nil for DeploymentNC.");
              }
            }
            catch (const CORBA::Exception& ex)
            {
              std::ostringstream os;
              os << ex;
              DANCEX11_LOG_ERROR ("PlanLauncher_Module::resolve_manager - "
                                  "Exception while resolving from DeploymentNC : " <<
                                  os.str ());
              last_error = os.str ();
            }
          }

          if (dm_obj)
          {
            this->svc_objectkind_ = svc_kind;
            break;
          }
        }
      }

      if (dm_obj)
      {
        // check for expected reference type
        this->dm_ =
            IDL::traits<Deployment::Deployment_Manager>::narrow (dm_obj);

        if (!this->dm_)
        {
          DANCEX11_LOG_WARNING ("PlanLauncher_Module::resolve_manager - " <<
                                "failed to narrow resolved deployment manager reference");
        }
      }

      if (!this->dm_ && timeout > timeout.now ())
      {
        // wait 50msec before trying again
        std::this_thread::sleep_for (std::chrono::milliseconds (50));
      }

    } while (!this->dm_ && timeout > timeout.now ());

    if (this->dm_ && this->svc_objectkind_.empty ())
    {
      // see if we can determine type of deployment manager
      try
      {
        if (IDL::traits<Deployment::ExecutionManager>::narrow (this->dm_))
        {
          this->svc_objectkind_ = "DomainManager";
        }
        else if (IDL::traits<Deployment::NodeManager>::narrow (this->dm_))
        {
          this->svc_objectkind_ = "NodeManager";
        }
        else
        {
          this->svc_objectkind_ = "LocalityManager";
        }
      }
      catch (const CORBA::Exception& ex)
      {
        std::ostringstream os;
        os << ex;
        DANCEX11_LOG_ERROR ("PlanLauncher_Module::resolve_manager - "
                            "Exception while narrowing deployment manager : " <<
                            os.str ());
        last_error = os.str ();
      }
    }

    if (!this->dm_)
    {
      DANCEX11_LOG_PANIC ("PlanLauncher_Module::resolve_manager - " <<
                          "Unable to resolve deployment manager reference" <<
                          (last_error.empty () ? "." : ": " ) << last_error);
      return false;
    }

    return true;
  }
}
