/**
 * @file   convert_plan.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "ace/Dynamic_Service.h"
#include "ace/Get_Opt.h"
#include "ace/OS_NS_sys_stat.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/handler/instance/plugin_conf.h"
#include "dancex11/logger/log.h"
#include "convert_plan_impl.h"

#include <string>

std::string input_filename;
std::string output_filename;
std::string config_file;
std::string fmt;

DAnCEX11::Plugin_Manager plugins {};

void
usage ()
{
  DANCEX11_LOG_TRACE ("usage");
  //X11_FUZZ: disable check_cout_cerr
  std::cerr <<
    "usage:\tdancex11_convert_plan [options]\n"
    "\tConverts a provided input file to CDR.\n"
    "\tIf no output file is nominated, the input file will be read and validated and the program will exit\n"
    "\t-i PLAN\t\t Input file\n"
    "\t-f FMT\t\t Input file format (default: determined from input file extension)\n"
    "\t-o FILE\t\t Output destination\n"
    "\t-c CFG\t\t Plugin configuration file\n" <<
    std::endl;
  //X11_FUZZ: enable check_cout_cerr
}

bool
parse_args (int argc, ACE_TCHAR *argv [])
{
  DANCEX11_LOG_TRACE ("parse_args");
  DANCEX11_LOG_DEBUG ("dancex11_split_plan options: ");

  for (int i = 1; i < argc; ++i)
  {
    DANCEX11_LOG_DEBUG ("\t" << argv[i]);
  }

  if (argc < 2)
  {
    usage ();
    return false;
  }

  ACE_Get_Opt get_opt (argc, argv, ACE_TEXT ("i:f:o:h"), 0);

  int c;
  ACE_CString s;
  while ((c = get_opt ()) != EOF)
    {
      switch (c)
        {
        case 'c':
          config_file = get_opt.opt_arg ();
          break;
        case 'i':
          input_filename = get_opt.opt_arg ();
          break;
        case 'f':
          fmt = get_opt.opt_arg ();
          break;
        case 'o':
          output_filename = get_opt.opt_arg ();
          break;
        case 'h':
          usage ();
          return false;
        default:
          usage ();
          return false;
        }
    }

  return true;
}

void
configure ()
{
  static std::string def_cfg = "convert_plan.config";

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
    DANCEX11_LOG_PANIC ("convert_plan::configure - "
                        "Failed to load configuration from [" <<
                        config_file << "]");
    throw Deployment::StartError ("convert_plan",
                                  std::string ("convert_plan::configure - "
                                  "Failed to load configuration from [") +
                                  config_file + "]");
  }

  // deploy plugins
  DAnCEX11::Plugin_Configurator pc;
  pc.load_from_plan (plugins, plugin_plan);
}

int
ACE_TMAIN (int argc, ACE_TCHAR *argv [])
{
  int retval = 0;

  DANCEX11_DEFINE_DEPLOYMENT_STATE (Convert_Plan);

  try
  {
    if (!parse_args (argc, argv))
    {
      return -1;
    }

    if (!DAnCEX11::State::instance ()->initialize (argc, argv))
    {
      DANCEX11_LOG_PANIC ("Convert_Plan - "
                          "Unable to initialize Deployment State");
      return 1;
    }

    configure ();

    if (input_filename.empty ())
    {
      DANCEX11_LOG_PANIC ("Convert_Plan - "
                          "No input filename provided");
      return 1;
    }

    if (fmt.empty ())
    {
      // When the user hasn't specified a format we default our format to
      // the input filename extension
      std::string::size_type const n = input_filename.rfind ('.');
      if (n != std::string::npos)
      {
        fmt = input_filename.substr (n+1);
      }
    }

    if ((fmt.find ("cdd") != std::string::npos) ||
        (fmt.find ("ddcfg") != std::string::npos) ||
        (fmt.find ("domcfg") != std::string::npos))
    {
      Deployment::Domain domain;
      if (!DAnCEX11::Plan_Loader::instance ()->read_domain (input_filename,
                                                            domain, fmt))
      {
        DANCEX11_LOG_PANIC ("Convert_Plan - "
                            "Unable to load provided domain into IDL "
                            "representation: " << input_filename);
        return 1;
      }

      DANCEX11_LOG_DEBUG ("Convert_Plan - Input domain:\n" <<
                          IDL::traits<Deployment::Domain>::write (domain));

      if (output_filename.empty ())
      {
        DANCEX11_LOG_DEBUG ("Convert_Plan - "
                            "Input file loaded correctly with no "
                            "output file specified, exiting");
        return 0;
      }

      if (!DAnCEX11::Convert_Plan::write_cdr_domain (output_filename, domain))
      {
        DANCEX11_LOG_PANIC ("Convert_Plan - "
                            "Unable to write cdr domain description into: "
                            << output_filename);
        return 1;
      }
    }
    else
    {
      Deployment::DeploymentPlan plan;
      if (!DAnCEX11::Plan_Loader::instance ()->read_plan (input_filename,
                                                          plan, fmt))
      {
        DANCEX11_LOG_PANIC ("Convert_Plan - "
                            "Unable to load provided plan into IDL "
                            "representation: " << input_filename);
        return 1;
      }

      DANCEX11_LOG_DEBUG ("Convert_Plan - Input plan:\n" <<
                          IDL::traits<Deployment::DeploymentPlan>::write (plan));

      if (output_filename.empty ())
      {
        DANCEX11_LOG_DEBUG ("Convert_Plan - "
                            "Input file loaded correctly with no "
                            "output file specified, exiting");
        return 0;
      }

      if (!DAnCEX11::Convert_Plan::write_cdr_plan (output_filename, plan))
      {
        DANCEX11_LOG_PANIC ("Convert_Plan - "
                            "Unable to write cdr plan into: "
                            << output_filename);
        return 1;
      }
    }
  }
  catch (const ::Deployment::PlanError &ex)
  {
    DANCEX11_LOG_PANIC ("Convert_Plan - "
                        "Config error while parsing plan " <<
                        ex);
    retval = 1;
  }
  catch (const CORBA::Exception &ex)
  {
    DANCEX11_LOG_PANIC ("Convert_Plan - "
                        "CORBA Exception while parsing plan " <<
                        ex);
    retval = 1;
  }
  catch (...)
  {
    DANCEX11_LOG_ERROR ("Convert_Plan - "
                        "error: unknown c++ exception\n");
    retval = 1;
  }

  return retval;
}
