// -*- C++ -*-
/**
 * @file dancex11_deployment_config_plan_loader.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11_deployment_config_plan_loader.h"
#include "dancex11/configurator/dancex11_config_loader.h"
#include "dancex11/logger/log.h"

#include <fstream>

namespace DAnCEX11
{
  CFGPlan_Loader* CFGPlan_Loader::instance ()
  {
    static CFGPlan_Loader instance_;

    return &instance_;
  }

  bool
  CFGPlan_Loader::read_plan (const std::string& filename, Deployment::DeploymentPlan& plan)
  {
    DANCEX11_LOG_TRACE ("CFGPlan_Loader::read_plan");

    if (filename.empty ())
    {
      DANCEX11_LOG_ERROR ("CFGPlan_Loader::read_plan - "
                          "Passed an empty filename, returning.");
      return false;
    }

    Config_Loader loader;
    return loader.load_plan_config (filename, plan);
  }

  bool
  CFGPlan_Loader::read_domain (const std::string& filename, Deployment::Domain& dom)
  {
    DANCEX11_LOG_TRACE ("CFGPlan_Loader::read_domain");

    if (filename.empty ())
    {
      DANCEX11_LOG_ERROR ("CFGPlan_Loader::read_domain - "
                          "Passed an empty filename, returning.");
      return false;
    }

    Config_Loader loader;
    return loader.load_domain_config (filename, dom);
  }

  void
  CFGPlan_Loader::init (Plan_Loader& pldr)
  {
    DANCEX11_LOG_TRACE ("CFGPlan_Loader::init");
    pldr.register_plan_loader_i ("cfg", this);
    pldr.register_plan_loader_i ("conf", this);
    pldr.register_plan_loader_i ("config", this);
    pldr.register_plan_loader_i ("dpcfg", this);
    pldr.register_plan_loader_i ("ddcfg", this);
    pldr.register_plan_loader_i ("domcfg", this);
  }

};
