// -*- C++ -*-
/**
 * @file dancex11_deployment_config_plan_loader.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_DEPLOYMENT_CONFIG_PLAN_LOADER_H
#define DANCEX11_DEPLOYMENT_CONFIG_PLAN_LOADER_H

#pragma once

#include "ace/Service_Object.h"

#include "dancex11_plan_loader_stub_export.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"

#include <string>

namespace DAnCEX11
{
  class DANCEX11_PLAN_LOADER_STUB_Export CFGPlan_Loader
    : public Plan_Loader_base
  {
  public:
    virtual ~CFGPlan_Loader () = default;

    virtual bool read_plan (const std::string& filename,
                            Deployment::DeploymentPlan& plan);

    virtual bool read_domain (const std::string& filename,
                              Deployment::Domain& dom);

    static CFGPlan_Loader* instance ();

  private:
    friend class Plan_Loader;

    CFGPlan_Loader () = default;

    void init (Plan_Loader&);
  };
};

#endif /* DANCEX11_DEPLOYMENT_CONFIG_PLAN_LOADER_H */
