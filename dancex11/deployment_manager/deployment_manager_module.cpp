// -*- C++ -*-
/**
 * @file deployment_manager_module.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "ace/Get_Opt.h"
#include "ace/Service_Config.h"
#include "ace/Dynamic_Service.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/core/dancex11_dmh_loader.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "deployment_manager_module.h"

#include "tao/x11/ior_table/ior_table.h"
#include "orbsvcs/orbsvcs/naming_server/CosNamingC.h"

#include <fstream>
#include <sstream>
#include <array>

namespace DAnCEX11
{
  namespace DeploymentManager
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

  const char* DeploymentManager_Module::SOptions::orb_endpt_opt_ = "-ORBEndpoint";
  const char* DeploymentManager_Module::SOptions::orb_listen_opt_ = "-ORBListenEndpoints";
  const char* DeploymentManager_Module::SOptions::orb_dotted_dec_opt_ = "-ORBDottedDecimalAddresses";
  const char* DeploymentManager_Module::SOptions::svcname_prefix_ = "DeploymentManager";
  const char* DeploymentManager_Module::SOptions::svcobj_name_ = "DANCEX11_DEPLOYMENT_HANDLER";
  const char* DeploymentManager_Module::SOptions::svcobj_factory_ = "_make_DAnCEX11_DeploymentHandler_Impl";

  DeploymentManager_Module::SOptions::SOptions ()
  {
    const char* dancex11_env = std::getenv ("DANCEX11_ROOT");

    if (!dancex11_env)
    {
      DANCEX11_LOG_WARNING ("DeploymentManager_Module::SOptions::SOptions - no DANCEX11_ROOT set.");
    }

    // check for a default listen address
    const char* dancex11_dh_listen_addr = std::getenv ("DANCEX11_DEPHANDLER_LISTEN_ADDR");
    // listen on default or everywhere except when specific defined on commandline
    this->listen_addr_ = dancex11_dh_listen_addr ? dancex11_dh_listen_addr : "";
    this->dm_kind_ = "Launcher";
  }

  DeploymentManager_Module::DeploymentManager_Module ()
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::DeploymentManager_Module");
  }

  DeploymentManager_Module::~DeploymentManager_Module ()
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::~DeploymentManager_Module");
  }

  const char *
  DeploymentManager_Module::usage ()
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::usage");
    return "\ndancex11_deployment_manager --handler LIB [-c CFG] [-n NAME[=IORFILE] [-a ADDR] [-p PORT] [-N] [-d NC] [-l[ PLAN] [-f FMT]] [-- <handler-args>]\n"
             " or\n"
             "dancex11_deployment_manager -x [-n NAME[=IORFILE] [-k KIND] [-a ADDR] [-p PORT] [-d NC]\n\n"
      "Deployment Manager Options:\n"
      "\t--handler LIB\t\t\t\t Specify the shared library to load the Deployment Handler implementation from\n"
      "\t<handler-args>\t\t\t\t Specify (a list of space separated) switches for the Deployment Handler implementation\n"
      "\t-n|--instance-name NAME[=IORFILE]\t Specify the name for the deployment manager instance and (optionally)\n"
                                   "\t\t\t\t\t\t the file to write (read) the IOR to (from).\n"
      "\t-d|--deployment-nc NC\t\t\t Specify naming context for deployment manager instance registration.\n"
      "\t-c|--config CFG\t\t\t\t Provide a deployment configuration file used to initialize the Deployment Manager.\n"
      "\t-a|--listen-address ADDR\t\t Provide the network interface address on which the Deployment Manager\n"\
                        "\t\t\t\t\t\t listens (default \"\", i.e. all interfaces).\n"
      "\t-p|--listen-port PORT\t\t\t Provide the port for the Deployment Manager to listen on.\n"
      "\t-N|--numeric-addresses\t\t\t Use numeric IP addresses (dotted decimal) in service advertisement/profiles/IOR.\n"
      "\t-l|--launch [PLAN]\t\t\t Launch the deployment plan read from PLAN.\n"
      "\t-f|--plan-format FMT\t\t\t Specify the deployment plan format to read from PLAN.\n"
      "\t-P|--property PROPID=PROPVAL\t\t Specifies a deployment property definition. Can be specified more than once.\n"
      "\t-x|--teardown\t\t\t\t Teardown a previously launched Deployment Manager instance.\n"
      "\t-k|--kind KIND\t\t\t\t Specify the kind of the deployment manager instance in case no IORFILE specified.\n"
             "\t\t\t\t\t\t Possible KINDs are: Launcher (default), ExecutionManager, NodeManager and LocalityManager.\n"
      "\t-h|--help\t\t\t\t print this help message\n";
  }

  bool
  DeploymentManager_Module::parse_args (int argc, char * argv[])
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::parse_args");

    ACE_Get_Opt get_opts (argc,
                          argv,
                          ACE_TEXT("n:d:c:a:p:l::f:xk:NP:h"),
                          1,
                          0,
                          ACE_Get_Opt::RETURN_IN_ORDER);

    get_opts.long_option (ACE_TEXT("handler"), ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("instance-name"), 'n', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("deployment-nc"), 'd', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("config"), 'c', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("listen-addr"), 'a', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("listen-port"), 'p', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("numeric-addresses"), 'N', ACE_Get_Opt::NO_ARG);
    get_opts.long_option (ACE_TEXT("launch"), 'l', ACE_Get_Opt::ARG_OPTIONAL);
    get_opts.long_option (ACE_TEXT("plan-format"), 'f', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("property"), 'P', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("teardown"), 'x', ACE_Get_Opt::NO_ARG);
    get_opts.long_option (ACE_TEXT("kind"), 'k', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("help"), 'h', ACE_Get_Opt::NO_ARG);

    int c {}, plan_ind {-1};
    while ( (c = get_opts ()) != -1)
    {
      switch (c)
      {
      case 'n':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - " <<
                            "Provided Manager name: " << get_opts.opt_arg ());
        this->options_.dm_name_ = get_opts.opt_arg ();
        break;

      case 'd':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Binding to provided Deployment Naming Context: " << get_opts.opt_arg ());
        this->options_.deployment_nc_ = get_opts.opt_arg ();
        break;

      case 'c':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - " <<
                            "Using configuration file for deployment manager: " <<
                            get_opts.opt_arg ());
        this->options_.dm_config_ = ACE_TEXT_ALWAYS_CHAR (get_opts.opt_arg ());
        break;

      case 'a':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Listening at address: " << get_opts.opt_arg ());
        this->options_.listen_addr_ = get_opts.opt_arg ();
        break;

      case 'p':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Listening on port: " << get_opts.opt_arg ());
        this->options_.listen_port_ = static_cast<uint16_t> (ACE_OS::atoi (get_opts.opt_arg ()));
        break;

      case 'N':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Going to use numeric IP addresses.");
        this->options_.numeric_addresses_ = true;
        break;

      case 'l':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Launching local plan");
        this->options_.launcher_ = true;
        if (get_opts.opt_arg ())
        {
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                              "Launching plan from: " << get_opts.opt_arg ());
          this->options_.plan_file_ = get_opts.opt_arg ();
        }
        else
        {
          plan_ind = get_opts.opt_ind ();
        }
        break;

      case 'f':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Loading plan with format: " << get_opts.opt_arg ());
        this->options_.plan_fmt_ = get_opts.opt_arg ();
        break;

      case 'P':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Parsing property definition: " << get_opts.opt_arg ());
        {
          std::string propdef = get_opts.opt_arg ();
          Deployment::Property prop {};
          Config_Loader loader;
          if (loader.parse_property_definition(propdef, prop))
          {
            this->options_.dm_props_.push_back (prop);
          }
          else
          {
            DANCEX11_LOG_ERROR ("DeploymentManager_Module::parse_args - "
                                "Failed to parse property definition " << propdef);
          }
        }
        break;

      case 'x':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Going to teardown running node.");
        this->options_.teardown_ = true;
        break;

      case 'k':
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                            "Manager kind: " << get_opts.opt_arg ());
        this->options_.dm_kind_ = get_opts.opt_arg ();
        break;

      //X11_FUZZ: disable check_cout_cerr
      case 'h':
        std::cerr << this->usage () << std::endl;
        if (!this->options_.dm_handler_lib_.empty ())
        {
          this->options_.dm_handler_arg_ = "-h";
          this->load_handler ();
        }
        DANCEX11_LOG_INFO (argv [0] << " -" << char(c));
        return false;
        break;
      //X11_FUZZ: enable check_cout_cerr

      case 0:
        if (ACE_OS::strcmp (get_opts.long_option (),
                            ACE_TEXT("handler")) == 0)
        {
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                              "Loading deployment handler from: " <<
                              get_opts.opt_arg ());
          this->options_.dm_handler_lib_ = get_opts.opt_arg ();
        }
        else if (ACE_OS::strcmp (get_opts.long_option (),
                                 ACE_TEXT("handler-arg")) == 0)
        {
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                              "Deployment handler arguments: " <<
                              get_opts.opt_arg ());
          this->options_.dm_handler_arg_ = get_opts.opt_arg ();
        }
        else
        {
          DANCEX11_LOG_WARNING ("DeploymentManager_Module::parse_args - "
                                "Ignoring unknown long option: " <<
                                get_opts.long_option ());
        }
        break;

      case 1:
        if ((plan_ind+1) == get_opts.opt_ind ())
        {
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                              "Launching plan from: " << get_opts.opt_arg ());
          this->options_.plan_file_ = get_opts.opt_arg ();
        }
        else
        {
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - "
                              "Collecting other option: " << get_opts.opt_arg ());
          this->options_.other_opts_.push_back (get_opts.opt_arg ());
        }
        break;

      case '?':
        {
          std::string foreign_sw = argv[get_opts.opt_ind ()];
          DANCEX11_LOG_DEBUG ("DAnCEX11_NodeManager_Module::parse_args - "
                              "Collecting foreign switch: " << argv[get_opts.opt_ind ()]);
          if (foreign_sw == SOptions::orb_endpt_opt_ || foreign_sw == SOptions::orb_listen_opt_)
          {
            // skip these options since they would mess up things (warn user)
            DANCEX11_LOG_WARNING("DAnCEX11_NodeManager_Module::parse_args - "
                                 "Skipping invalid ORB endpoint switch: " << argv[get_opts.opt_ind ()]);
            // skip to beyond foreign switch argument argv entry
            for (int aix = get_opts.opt_ind ()+1; get_opts.opt_ind () == aix ;) get_opts ();
          }
          else
          {
            this->options_.other_opts_.push_back (argv[get_opts.opt_ind ()]);
            // skip to next argv entry
            for (int aix = get_opts.opt_ind (); get_opts.opt_ind () == aix ;) get_opts ();
          }
        }
        break;

      default:
        DANCEX11_LOG_WARNING ("DeploymentManager_Module::parse_args - ignoring incorrect option: "
                              << (get_opts.opt_arg () ? get_opts.opt_arg () : ""));
      }
    }

    // check for DMH handler arguments
    if (get_opts.opt_ind () > 0 && get_opts.opt_ind () < argc &&
        std::string("--") == argv[get_opts.opt_ind ()-1])
    {
      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - " <<
                          "collecting handler arguments");
      std::ostringstream os;
      for (int n = get_opts.opt_ind (); n < argc ;++n)
      {
        os << argv[n];
        if ((n+1) < argc)
          os << " ";
      }
      this->options_.dm_handler_arg_ = os.str ();

      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::parse_args - " <<
                          "collected handler arguments: " <<
                          this->options_.dm_handler_arg_);
    }

    return true;
  }

  bool
  DeploymentManager_Module::init (int argc,
                                  char *argv[])
  {
    try
    {
      if (!this->parse_args (argc, argv))
      {
        return false;
      }

      // Parsing DM name and DM ior file name
      std::string dm_name = this->options_.dm_name_;
      std::string dm_file;
      size_t npos = dm_name.find ('=');
      if (std::string::npos != npos)
      {
        dm_file = dm_name.substr (npos + 1);
        dm_name = dm_name.substr (0, npos);
      }

      if (this->options_.teardown_)
      {
        return this->init_teardown (argv[0], dm_name, dm_file);
      }
      else
      {
        return this->init_startup (argv[0], dm_name, dm_file);
      }
    }
    catch (const CORBA::Exception& ex)
    {
      DANCEX11_LOG_PANIC ("DeploymentManager_Module::init - " << ex);
      return false;
    }

    return true;
  }

  bool
  DeploymentManager_Module::init_teardown (
      const char* argv0,
      const std::string& dm_name,
      const std::string& dm_file)
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_teardown - " <<
                        "Initializing ORB arguments.");

    // setup ORB arguments
    int orb_argc {};
    std::unique_ptr<const char*[]> orb_argv = this->create_orb_args (orb_argc, argv0);

    // initialize ORB
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_teardown - " <<
                        "Initializing ORB");

    if (!DAnCEX11::State::instance ()->initialize (
            orb_argc, const_cast<char**> (orb_argv.get ())))
    {
      DANCEX11_LOG_PANIC ("DeploymentManager_Module::init_teardown - " <<
                          "Failed to initialize ORB");
      return false;
    }

    IDL::traits<CORBA::ORB>::ref_type orb =
        DAnCEX11::State::instance ()->orb ();

    // resolve manager object

    // determine object name and kind
    this->svc_objectid_ = dm_name;
    if (this->svc_objectid_.empty ())
    {
      this->svc_objectid_ = SOptions::svcname_prefix_;
    }
    this->svc_objectkind_ = this->options_.dm_kind_;

    // should we try to resolve IOR from file?
    if (!dm_file.empty ())
    {
      // read IOR
      std::string ior = DAnCEX11::DeploymentManager::read_IOR (dm_file);
      if (!ior.empty ())
      {
        DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_teardown - " <<
                            "resolving deployment manager reference :" <<
                            ior);

        // attempt to resolve DM handler reference
        this->dm_ = orb->string_to_object (ior);
      }
      else
      {
        DANCEX11_LOG_WARNING ("DeploymentManager_Module::init_teardown - " <<
                              "empty IOR read from :" <<
                              dm_file);
      }
    }

    // do we have a dm or should we look further?
    if (!this->dm_)
    {
      std::string const svc_name =
          this->svc_objectid_ + "." + this->svc_objectkind_;


      // get IOR from construct based on specific endpoint port?
      if (this->options_.listen_port_ > 0)
      {
        // construct corbaloc
        std::ostringstream os;
        os << "corbaloc:iiop:" << this->options_.listen_addr_
           << ":"
           << this->options_.listen_port_
           << "/" << svc_name;

        DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_teardown - " <<
                            "resolving deployment manager reference : " <<
                            os.str ());

        // attempt to resolve deployment launch manager reference
        this->dm_ = orb->string_to_object (os.str ());
      }

      // should we try to resolve IOR from NC?
      if (!this->dm_ && !this->options_.deployment_nc_.empty ())
      {
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_teardown - "
                            "Resolving DM from DeploymentNC : " <<
                            this->options_.deployment_nc_);

        try
        {
          DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_teardown - "
                              "Resolving DeploymentNC.");
          IDL::traits<CORBA::Object>::ref_type inc_obj =
              orb->string_to_object (this->options_.deployment_nc_);
          if (inc_obj)
          {
            IDL::traits<CosNaming::NamingContext>::ref_type instance_nc =
                IDL::traits<CosNaming::NamingContext>::narrow (inc_obj);
            if (instance_nc)
            {
              // Resolving deployment manager reference from DeploymentNC
              DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_teardown - " <<
                                  "Resolving DM from NC as \"" <<
                                  svc_name << "\".");
              CosNaming::Name name;
              name.push_back (CosNaming::NameComponent (this->svc_objectid_,
                                                        this->svc_objectkind_));
              this->dm_ = instance_nc->resolve (name);
            }
            else
            {
              DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::init_teardown - "
                                  "Narrow to NamingContext returned nil for DeploymentNC.");
            }
          }
          else
          {
            DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::init_teardown - "
                                "DeploymentNC resolved to nil.");
          }
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::init_teardown - "
                              "Exception while resolving from DeploymentNC : " <<
                              ex);
        }
      }
    }

    // check for expected reference type for 'teardown'
    IDL::traits<DAnCEX11::ShutdownControl>::ref_type sc =
        IDL::traits<DAnCEX11::ShutdownControl>::narrow (this->dm_);

    if (!sc)
    {
      DANCEX11_LOG_WARNING ("DeploymentManager_Module::init_teardown - " <<
                            "failed to narrow resolved deployment manager reference");

      this->dm_.reset ();
    }

    if (!this->dm_)
    {
      DANCEX11_LOG_PANIC ("DeploymentManager_Module::init_teardown - " <<
                          "Unable to resolve deployment manager reference.");
      return false;
    }

    return true;
  }

  bool
  DeploymentManager_Module::init_startup (
      const char* argv0,
      const std::string& dm_name,
      const std::string& dm_file)
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_startup - " <<
                        "Initializing ORB arguments.");

    // setup ORB arguments
    int orb_argc {};
    std::unique_ptr<const char*[]> orb_argv =
        this->create_orb_args (orb_argc, argv0);

    DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_startup - " <<
                        "Initializing ORB");

    if (!DAnCEX11::State::instance ()->initialize (
            orb_argc, const_cast<char**> (orb_argv.get ())))
    {
      DANCEX11_LOG_PANIC ("DeploymentManager_Module::init_startup - " <<
                          "Failed to initialize ORB");
      return false;
    }

    IDL::traits<CORBA::ORB>::ref_type orb =
        DAnCEX11::State::instance ()->orb ();

    DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " <<
                        "Loading deployment manager handler");

    // load deployment manager handler
    this->load_handler ();

    if (!this->handler_)
    {
      DANCEX11_LOG_PANIC ("DeploymentManager_Module::init_startup - " <<
                          "Failed to load deployment manager handler");
      return false;
    }

    DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " <<
                        "Loaded deployment manager handler [" <<
                        this->handler_->handler_type () << "]");

    DANCEX11_LOG_TRACE ("DeploymentManager_Module::init_startup - " <<
                        "configuring DMH : " << this->options_.dm_config_);

    // configure DMH
    Deployment::Properties dmh_prop (this->options_.dm_props_);
    CORBA::Any any;
    // add standard DM properties if applicable
    any <<= dm_name;
    dmh_prop.push_back (Deployment::Property (DAnCEX11::DM_INSTANCE_NAME, any));
    if (this->options_.listen_port_ > 0)
    {
      any <<= this->options_.listen_port_;
      dmh_prop.push_back (Deployment::Property (DAnCEX11::DM_INSTANCE_PORT, any));
    }
    if (!this->options_.listen_addr_.empty ())
    {
      any <<= this->options_.listen_addr_;
      dmh_prop.push_back (Deployment::Property (DAnCEX11::DM_INSTANCE_ADDRESS, any));
    }
    if (this->options_.numeric_addresses_)
    {
      any <<= this->options_.numeric_addresses_;
      dmh_prop.push_back (Deployment::Property (DAnCEX11::DM_NUMERIC_ADDRESSES, any));
    }
    if (!this->options_.deployment_nc_.empty ())
    {
      // pass naming context reference for domain/deployment objects to DMH
      any <<= this->options_.deployment_nc_;
      dmh_prop.push_back (Deployment::Property (DAnCEX11::DOMAIN_NC, any));
    }
    this->handler_->configure (this->options_.dm_config_,
                               dmh_prop,
                               this->_this ());

    DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " <<
                        "Activating POA manager");

    // Activate POA manager
    IDL::traits<PortableServer::POAManager>::ref_type mgr =
        DAnCEX11::State::instance ()->root_poa ()->the_POAManager ();
    mgr->activate ();

    this->svc_objectid_ = dm_name;
    if (this->svc_objectid_.empty ())
    {
      this->svc_objectid_ = SOptions::svcname_prefix_;
    }

    if (this->options_.plan_file_.empty ())
    {
      this->svc_objectkind_ = this->handler_->handler_type ();
    }
    else
    {
      this->svc_objectkind_ = "Launcher";
    }

    std::string const svc_name =
        this->svc_objectid_ + "." + this->svc_objectkind_;

    if (this->options_.launcher_)
    {
      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " <<
                          "Activating launch manager : " << svc_name);

      // local plan launch requested; activate DeploymentLaunchManager
      IDL::traits<DAnCEX11::DeploymentLaunchManager>::ref_type dlm =
          this->handler_->activate_launcher (svc_name);

      if (!this->options_.plan_file_.empty ())
      {
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " \
                            "reading plan from " << this->options_.plan_file_);

        // read plan from file
        Deployment::DeploymentPlan plan;
        if (!DAnCEX11::Plan_Loader::instance ()->read_plan (
            this->options_.plan_file_,
            plan,
            this->options_.plan_fmt_))
        {
          DANCEX11_LOG_PANIC ("DeploymentManager_Module::init_startup - "
                              "failed to read plan from " <<
                              this->options_.plan_file_);
          return false;
        }

        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - "
                            "initializing launch manager with plan [" <<
                            plan.UUID () <<
                            "] from [" << this->options_.plan_file_ << "]");

        dlm->plan (std::move (plan));
      }

      // launch the configured plan
      dlm->launch ();

      this->dm_ = dlm;
    }
    else
    {
      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " <<
                          "Activating deployment manager service : " <<
                          svc_name);

      // deployment management service requested; activate standard DM
      IDL::traits<Deployment::Deployment_Manager>::ref_type ddm =
          this->handler_->activate_manager (svc_name);

      this->dm_ = ddm;
    }

    // Getting deployment manager service ior
    std::string const ior = orb->object_to_string (this->dm_);

    // Writing ior to file
    if (!dm_file.empty ())
    {
      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " <<
                          "Writing node IOR " << ior << " to file " <<  dm_file);
      if (!DAnCEX11::DeploymentManager::write_IOR (dm_file, ior))
      {
        DANCEX11_LOG_ERROR ("DeploymentManager_Module::init_startup - " <<
                            "Error: Unable to write IOR to file " <<
                            dm_file);
      }
    }

    // Finishing Deployment part
    DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " \
                        "deployment servant activated...");

    DANCEX11_LOG_DEBUG ("DeploymentManager_Module::init_startup - " \
                        "servant IOR: " << ior);

    return true;
  }

  void
  DeploymentManager_Module::load_handler ()
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::load_handler");

    if (!this->options_.dm_handler_lib_.empty ())
    {
      // create an ACE Service Config directive for the DMH loader
      std::ostringstream os;
      os << "dynamic " << SOptions::svcobj_name_ << " Service_Object * " <<
            this->options_.dm_handler_lib_ << ":" << SOptions::svcobj_factory_ <<
            "() \"" << this->options_.dm_handler_arg_ << "\"";
      std::string svcfg_txt = os.str ();

      if (ACE_Service_Config::process_directive (svcfg_txt.c_str ()) != 0)
      {
        DANCEX11_LOG_ERROR ("DeploymentManager_Module::load_handler - "
                            "Error(s) while processing DMH loader "
                            "Service Config directive :\n\t" <<
                            svcfg_txt);
        return;
      }
    }

    DAnCEX11::DMHandler_Loader *dmh_loader {};
    try {
      DANCEX11_LOG_DEBUG ( "DeploymentManager_Module::load_handler - "
                           "Resolving DMH loader <" << SOptions::svcobj_name_ << ">");
      dmh_loader =
        ACE_Dynamic_Service<DAnCEX11::DMHandler_Loader>::instance (SOptions::svcobj_name_);
    }
    catch (...) {
      DANCEX11_LOG_ERROR ("DeploymentManager_Module::load_handler - "
                          "Unknown exception caught while resolving the "
                          "DMH loader <" << SOptions::svcobj_name_ << ">");
    }

    this->handler_ = dmh_loader->create_handler ();
  }

  void
  DeploymentManager_Module::remove_handler ()
  {
    DANCEX11_LOG_TRACE ("DeploymentManager_Module::remove_handler");

    // destroy handler instance
    this->handler_.reset ();

    // destroy deployment manager instance
    this->dm_.reset ();

    // create an ACE Service Config directive to remove the DMH loader
    std::ostringstream os;
    os << "remove " << SOptions::svcobj_name_;
    std::string svcfg_txt = os.str ();

    if (ACE_Service_Config::process_directive (svcfg_txt.c_str ()) != 0)
    {
      DANCEX11_LOG_ERROR ("DeploymentManager_Module::remove_handler - "
                          "Error(s) while processing DMH loader "
                          "Service Config directive :\n\t" <<
                          svcfg_txt);
    }
  }

  bool
  DeploymentManager_Module::run ()
  {
    if (this->options_.teardown_)
    {
      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - " \
                          "stopping running launcher...");

      IDL::traits<DAnCEX11::ShutdownControl>::ref_type sc =
          IDL::traits<DAnCEX11::ShutdownControl>::narrow (this->dm_);

      sc->shutdown ();

      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - " \
                          "launcher stopped.");
    }
    else
    {
      IDL::traits<CosNaming::NamingContext>::ref_type instance_nc {};
      IDL::traits<IORTable::Table>::ref_type adapter {};

      IDL::traits<CORBA::ORB>::ref_type orb =
          DAnCEX11::State::instance ()->orb ();

      // Getting deployment manager service ior
      std::string const ior = orb->object_to_string (this->dm_);

      std::string const service_id =
          this->svc_objectid_ + "." + this->svc_objectkind_;

      // register IOR in local IORTable
      // do this whether or not an explicit fixed port is provided as the
      // user may have specified the port through an ORBListenEnpoints arg
      try
      {
        // Initialize IOR table
        IDL::traits<CORBA::Object>::ref_type table_object =
            orb->resolve_initial_references ("IORTable");

        adapter =
            IDL::traits<IORTable::Table>::narrow (table_object);

        if (!adapter)
        {
          DANCEX11_LOG_ERROR ("DeploymentManager_Module::run - "
                              "Unable to RIR the IORTable.");
          return false;
        }

        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - " <<
                            "Registering DM in IORTable as \"" <<
                            service_id << "\".");

        // Binding ior to IOR Table
        adapter->bind (service_id, ior);
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::run - "
                            "Exception while registering in IORTable : " <<
                            ex);
        adapter.reset ();
      }

      // registering IOR with naming context
      if (!this->options_.deployment_nc_.empty ())
      {
        DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - "
                            "Registering with DeploymentNC : " <<
                            this->options_.deployment_nc_);

        try
        {
          DANCEX11_LOG_TRACE ("DeploymentManager_Module::run - Resolving DeploymentNC.");
          IDL::traits<CORBA::Object>::ref_type inc_obj =
              orb->string_to_object (this->options_.deployment_nc_);
          if (inc_obj)
          {
            instance_nc =
                IDL::traits<CosNaming::NamingContext>::narrow (inc_obj);
            if (instance_nc)
            {
              // Binding deployment manager IOR to DeploymentNC
              DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - " <<
                                  "Registering DM in NC as \"" <<
                                  service_id << "\".");
              CosNaming::Name name;
              name.push_back (CosNaming::NameComponent (this->svc_objectid_,
                                                        this->svc_objectkind_));
              instance_nc->rebind (name, this->dm_);
            }
            else
            {
              DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::run - "
                                  "Narrow to NamingContext returned nil for DeploymentNC.");
            }
          }
          else
          {
            DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::run - "
                                "DeploymentNC resolved to nil.");
          }
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::run - "
                              "Exception while registering with DeploymentNC : " <<
                              ex);
          instance_nc.reset ();
        }
      }

      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - " \
                          "running ORB...");

      orb->run ();

      DANCEX11_LOG_DEBUG ("DeploymentManager_Module::run - " \
                          "ORB shut down.");

      if (adapter)
      {
        try
        {
          adapter->unbind (service_id);
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_WARNING ("DeploymentManager_Module::run - "
                                "Exception while deregistering from IORTable : " <<
                                ex);
        }
      }

      try
      {
        this->handler_->shutdown ();
      }
      catch (const Deployment::StopError & ex)
      {
        DANCEX11_LOG_WARNING ("DeploymentManager_Module::run - "
                              "Exception while shutting down deployment handler : " <<
                              ex);
      }

      // unload the handler service
      this->remove_handler ();
    }

    return true;
  }


  void
  DeploymentManager_Module::shutdown ()
  {
    if (!this->options_.deployment_nc_.empty ())
    {
      try
      {
        IDL::traits<CORBA::ORB>::ref_type orb =
          DAnCEX11::State::instance ()->orb ();

        DANCEX11_LOG_TRACE ("DeploymentManager_Module::shutdown - " <<
                            "Resolving DeploymentNC.");

        IDL::traits<CORBA::Object>::ref_type inc_obj =
            orb->string_to_object (this->options_.deployment_nc_);

        IDL::traits<CosNaming::NamingContext>::ref_type instance_nc =
            IDL::traits<CosNaming::NamingContext>::narrow (inc_obj);

        if (instance_nc)
        {
          // Unbinding deployment manager IOR from DeploymentNC
          DANCEX11_LOG_DEBUG ("DeploymentManager_Module::shutdown - " <<
                              "Deregistering DM from NC as \"" <<
                              this->svc_objectid_ << "." << this->svc_objectkind_ << "\".");
          CosNaming::Name name;
          name.push_back (CosNaming::NameComponent (this->svc_objectid_,
                                                    this->svc_objectkind_));
          instance_nc->unbind (name);
        }
        else
        {
          DANCEX11_LOG_ERROR ("DAnCEX11_NodeManager_Module::shutdown - "
                              "Narrow to NamingContext returned nil for DeploymentNC.");
        }
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_WARNING ("DeploymentManager_Module::shutdown - " <<
                              "Exception while deregistering from DeploymentNC : " <<
                              ex);
      }
    }
  }

  std::unique_ptr<const char*[]>
  DeploymentManager_Module::create_orb_args (int &argc, const char* argv0)
  {
    std::unique_ptr<const char*[]> orb_argv;
    int narg = 0;
    if (this->options_.teardown_)
    {
      // initialize ORB arguments with program
      argc = ACE_Utils::truncate_cast<int> (this->options_.other_opts_.size () + 1);
      orb_argv = std::make_unique<const char*[]> (argc);
      orb_argv.get ()[narg++] = argv0;
    }
    else
    {
      // should launcher listen at specific address and/or port?
      if (this->options_.listen_port_ > 0 || !this->options_.listen_addr_.empty ())
      {
        DANCEX11_LOG_TRACE ("DeploymentManager_Module::create_orb_args - " <<
                            "Initializing ORB listen endpoint argument: " <<
                            "address=" << this->options_.listen_addr_ <<
                            "; port=" << this->options_.listen_port_);

        // initialize ORB arguments with program and ORB listen endpoint
        argc = ACE_Utils::truncate_cast<int> (this->options_.other_opts_.size () +
                                              (this->options_.numeric_addresses_ ? 5 : 3));
        orb_argv = std::make_unique<const char*[]> (argc);
        orb_argv.get ()[narg++] = argv0;
        orb_argv.get ()[narg++] = SOptions::orb_listen_opt_;
        std::ostringstream os;
        os << "iiop://" << this->options_.listen_addr_;
        if (this->options_.listen_port_ > 0)
        {
          os << ":" << this->options_.listen_port_;
        }
        this->orb_endpoint_ = os.str ();
        orb_argv.get ()[narg++] = this->orb_endpoint_.c_str ();

        DANCEX11_LOG_TRACE ("DeploymentManager_Module::create_orb_args - " <<
                            "Added ORB listen endpoint: " <<
                            this->orb_endpoint_);
      }
      else
      {
        // initialize ORB arguments with program
        argc = ACE_Utils::truncate_cast<int> (this->options_.other_opts_.size () +
                                              (this->options_.numeric_addresses_ ? 3 : 1));
        orb_argv = std::make_unique<const char*[]> (argc);
        orb_argv.get ()[narg++] = argv0;
      }
      if (this->options_.numeric_addresses_)
      {
        orb_argv.get ()[narg++] = SOptions::orb_dotted_dec_opt_;
        orb_argv.get ()[narg++] = "1";

        DANCEX11_LOG_TRACE ("DeploymentManager_Module::create_orb_args - " <<
                            "Added ORB DottedDecimalAddresses (1) option");
      }
    }
    // append all other remaining options
    for (const std::string& a : this->options_.other_opts_)
    {
      orb_argv.get ()[narg++] = a.c_str ();
    }
    return orb_argv;
  }

}
