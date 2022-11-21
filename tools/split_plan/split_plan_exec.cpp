// -*- C++ -*-
/**
 * @file   split_plan_exec.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "ace/Dynamic_Service.h"
#include "ace/Get_Opt.h"
#include "ace/OS_NS_sys_stat.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/handler/instance/plugin_conf.h"
#include "tools/convert_plan/convert_plan_impl.h"
#include "split_plan.h"
#include "node_splitter.h"
#include "locality_splitter.h"

#include <string>

std::string input_filename;
std::string fmt;
std::string config_file;
char split_type = 'N';
bool warnings_as_error = false;
bool allow_external_ref_storage = false;

DAnCEX11::Plugin_Manager plugins {};

namespace fn
{
  template <typename Char_T>
  std::basic_string<Char_T>
  extension (const std::basic_string<Char_T>& path)
  {
    typename std::basic_string<Char_T>::size_type n = path.rfind ('.');
    return (n == std::basic_string<Char_T>::npos) ? "" : path.substr (n+1);
  }

  template <typename Char_T>
  std::basic_string<Char_T>
  basename (const std::basic_string<Char_T>& path, bool no_ext = false)
  {
    typename std::basic_string<Char_T>::size_type n = std::basic_string<Char_T>::npos;
    typename std::basic_string<Char_T>::size_type off = path.rfind (ACE_DIRECTORY_SEPARATOR_CHAR_A);
    if (off == std::basic_string<Char_T>::npos)
      off = 0;
    else
      off += 1;
    if (no_ext)
    {
      n = path.rfind ('.');
      if (n == std::basic_string<Char_T>::npos || n < off)
        n = std::basic_string<Char_T>::npos;
      else
        n -= off;
    }
    return path.substr (off, n);
  }

  template <typename Char_T>
  std::basic_string<Char_T>
  dirname (const std::basic_string<Char_T>& path)
  {
    typename std::basic_string<Char_T>::size_type off = path.rfind (ACE_DIRECTORY_SEPARATOR_CHAR_A);
    return (off == std::basic_string<Char_T>::npos) ? "" : path.substr (0, off);
  }
}

void
usage ()
{
  DANCEX11_LOG_TRACE ("usage");

  //X11_FUZZ: disable check_cout_cerr
  std::cerr <<
    ACE_TEXT ("usage:\tdancex11_split_plan [options]\n")
    ACE_TEXT ("\tApplies the split plan algorithm and saves the "\
              "result to CDR encoded plans\n")
    ACE_TEXT ("\t-i PLAN\t\t Input plan\n")
    ACE_TEXT ("\t-f FMT\t\t Input plan format (default: determined from PLAN extension)\n")
    ACE_TEXT ("\t-t SPLIT\t Type of split to perform; "\
              "N=node (default), L=locality\n")
    ACE_TEXT ("\t-c CFG\t\t Plugin configuration file\n") <<
    ACE_TEXT ("\t-e \t\t Report warnings as errors\n") <<
    ACE_TEXT ("\t-r \t\t Allow default external facet reference storage\n") <<
    std::endl;
  //X11_FUZZ: enable check_cout_cerr
}

bool
parse_args (int argc, ACE_TCHAR *argv [])
{
  DANCEX11_LOG_TRACE ("parse_args");

  DANCEX11_LOG_DEBUG ("parse_args - dancex11_split_plan options: ");

  for (int i = 1; i < argc; ++i)
  {
    DANCEX11_LOG_DEBUG ("\t" << argv[i]);
  }

  if (argc < 2)
  {
    usage ();
    return false;
  }

  ACE_Get_Opt get_opt (argc, argv, ACE_TEXT ("i:f:t:her"), 0);

  int c;
  ACE_CString s;
  while ((c = get_opt ()) != EOF)
    {
      switch (c)
        {
        case 'i':
          input_filename = ACE_TEXT_ALWAYS_CHAR (get_opt.opt_arg ());
          break;

        case 'f':
          fmt = ACE_TEXT_ALWAYS_CHAR (get_opt.opt_arg ());
          break;

        case 't':
          split_type = *ACE_TEXT_ALWAYS_CHAR (get_opt.opt_arg ());
          if (split_type != 'N' && split_type != 'L')
            {
              DANCEX11_LOG_ERROR ("parse_args - "
                                  "Invalid split type specified: " <<
                                  split_type);
              split_type = 'N';
            }
          break;
        case 'e':
          warnings_as_error = true;
          break;
        case 'r':
          allow_external_ref_storage = true;
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
  static std::string def_cfg = "split_plan.config";

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
    DANCEX11_LOG_PANIC ("split_plan::configure - "
                        "Failed to load configuration from [" <<
                        config_file << "]");
    throw Deployment::StartError ("split_plan",
                                  std::string ("split_plan::configure - "
                                  "Failed to load configuration from [") +
                                  config_file + "]");
  }

  // deploy plugins
  DAnCEX11::Plugin_Configurator pc;
  pc.load_from_plan (plugins, plugin_plan);
}

bool
check_sub_plan (const Deployment::DeploymentPlan &plan, const std::string& plan_id)
{
  DANCEX11_LOG_INFO ("check_sub_plan - checking plan " << plan_id);

  bool rc = true;
  for (const ::Deployment::PlanConnectionDescription& conn : plan.connection ())
  {
    DANCEX11_LOG_INFO ("check_sub_plan - plan " << plan_id << " connection " << conn.name ());

    // in case of a single internal endpoint
    if (conn.internalEndpoint ().size () == 1)
    {
      DANCEX11_LOG_INFO ("check_sub_plan - plan " << plan_id << " connection " << conn.name ()
                         << " has a single internal endpoint");

      // check if there is an external (CORBA NS refs) reference to connect to
      if (conn.externalReference ().size () != 1)
      {
        // or if the internalEndpoint is a facet that will be registered with CORBA NS
        if (conn.internalEndpoint ()[0].kind () != ::Deployment::CCMComponentPortKind::Facet)
        {
          if (warnings_as_error)
            DANCEX11_LOG_ERROR ("Split_Plan - sub plan ["
                << plan_id
                << "] has an unconnected receptacle in connection ["
                << conn.name () << "]");
          else
            DANCEX11_LOG_WARNING ("Split_Plan - sub plan ["
                << plan_id
                << "] has an unconnected receptacle in connection ["
                << conn.name () << "]");
          rc = false;
        }
        else
        {
          // get the instance providing the internal endpoint
          const ::Deployment::InstanceDeploymentDescription &inst =
              plan.instance ()[conn.internalEndpoint ()[0].instanceRef ()];
          // check for the properties specifying external reference storage if allowed
          std::string name;
          if (!allow_external_ref_storage
              ||
              !(DAnCEX11::Utility::get_property_value (DAnCEX11::REGISTER_NAMING,
                                                     inst.configProperty (),
                                                     name)
               ||
               DAnCEX11::Utility::get_property_value (DAnCEX11::INSTANCE_IOR_FILE,
                                                                  inst.configProperty (),
                                                                  name)))
          {
            if (warnings_as_error)
              DANCEX11_LOG_ERROR ("Split_Plan - sub plan ["
                  << plan_id
                  << "] has an unconnectable facet in connection ["
                  << conn.name () << "]");
            else
              DANCEX11_LOG_WARNING ("Split_Plan - sub plan ["
                  << plan_id
                  << "] has an unconnectable facet in connection ["
                  << conn.name () << "]");
            rc = false;
          }
        }
      }
    }
  }
  return rc;
}

int
ACE_TMAIN (int argc, ACE_TCHAR *argv [])
{
  int retval = 0;

  DANCEX11_DEFINE_DEPLOYMENT_STATE (Split_Plan);

  try
  {
    if (!parse_args (argc, argv))
    {
      return 1;
    }

    // set default log priorities if not overruled by a user defined setting
    if (!std::getenv ("DANCEX11_LOG_MASK"))
    {
      DANCEX11_LOGGER::priority_mask(
          x11_logger::X11_LogMask::LP_WARNING|x11_logger::X11_LogMask::LP_ALL_ERROR);
    }

    std::vector<std::string> orb_args;
    for (int n=0; n<argc ;++n)
      orb_args.push_back (argv[n]);
    DAnCEX11::State::instance ()->initialize (std::move (orb_args));

    configure ();

    // trigger ORB creation
    (void)DAnCEX11::State::instance ()->orb ();

    Deployment::DeploymentPlan plan;
    if (!DAnCEX11::Plan_Loader::instance ()->read_plan (input_filename,
                                                        plan, fmt))
    {
      DANCEX11_LOG_PANIC ("Split_Plan - "
                          "Unable to load provided plan into IDL "
                          "representation: " << input_filename);
      return 1;
    }

    if (split_type == 'N')
    {
      DAnCEX11::Split_Plan<DAnCEX11::Node_Splitter> split;
      split.split_plan (plan);

      bool check = true;
      for (DAnCEX11::Split_Plan<DAnCEX11::Node_Splitter>::TSubPlanIterator
             iter_plans = split.plans ().begin ();
          iter_plans != split.plans ().end ();
          ++iter_plans)
      {
        check = check_sub_plan(iter_plans->second, iter_plans->first);
      }
      if (!check && warnings_as_error)
      {
        return 1;
      }

      for (DAnCEX11::Split_Plan<DAnCEX11::Node_Splitter>::TSubPlanIterator
             iter_plans = split.plans ().begin();
          iter_plans != split.plans ().end();
          ++iter_plans)
      {
        std::string label (iter_plans->second.label ());
        DANCEX11_LOG_DEBUG ("Split_Plan - "
                            "Writing sub plan: " <<
                            label);

        std::string dirname = fn::dirname (input_filename);
        std::string name (iter_plans->first);
        name += "-";
        name += fn::basename (input_filename, true);
        name += ".cdr";
        if (!dirname.empty ())
        {
          name = dirname + ACE_DIRECTORY_SEPARATOR_STR_A + name;
        }
        if (!DAnCEX11::Convert_Plan::write_cdr_plan (name, iter_plans->second))
        {
          DANCEX11_LOG_PANIC ("Split_Plan - "
                              "Unable to write cdr plan into: "
                              << name);
          return 1;
        }
      }
    }
    else
    {
      DAnCEX11::Split_Plan<DAnCEX11::Locality_Splitter> split;
      split.split_plan (plan);

      bool check = true;
      for (DAnCEX11::Split_Plan<DAnCEX11::Locality_Splitter>::TSubPlanIterator
             iter_plans = split.plans ().begin ();
          iter_plans != split.plans ().end ();
          ++iter_plans)
      {
        check = check_sub_plan(iter_plans->second, iter_plans->first.locality_manager_label(iter_plans->second));
      }
      if (!check && warnings_as_error)
      {
        return 1;
      }

      for (DAnCEX11::Split_Plan<DAnCEX11::Locality_Splitter>::TSubPlanIterator
             iter_plans = split.plans ().begin ();
          iter_plans != split.plans ().end ();
          ++iter_plans)
      {
        std::string label (iter_plans->second.label ());
        DANCEX11_LOG_DEBUG ("Split_Plan - "
                            "Writing sub plan: " <<
                            label);

        std::string dirname = fn::dirname (input_filename);
        std::string name (iter_plans->first.locality_manager_label(iter_plans->second));
        name += "-";
        name += fn::basename (input_filename, true);
        name += ".cdr";
        if (!dirname.empty ())
        {
          name = dirname + ACE_DIRECTORY_SEPARATOR_STR_A + name;
        }
        if (!DAnCEX11::Convert_Plan::write_cdr_plan (name, iter_plans->second))
        {
          DANCEX11_LOG_PANIC ("Split_Plan - "
                              "Unable to write cdr plan into: "
                              << name);
          return 1;
        }
      }
    }
  }
  catch (const CORBA::Exception &ex)
  {
    DANCEX11_LOG_PANIC ("Split_Plan - "
                        "CORBA Exception while parsing plan " <<
                        ex);
    retval = 1;
  }
  catch (...)
  {
    DANCEX11_LOG_PANIC ("Convert_Plan - "
                        "error: unknown c++ exception\n");
    retval = 1;
  }

  return retval;
}
