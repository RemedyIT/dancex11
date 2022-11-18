// -*- C++ -*-
/**
 * @file dancex11_deployment_cdp_plan_loader.h
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DEPLOYMENT_CDP_PLAN_LOADER_H
#define DANCEX11_DEPLOYMENT_CDP_PLAN_LOADER_H

#pragma once

#include "ace/Service_Object.h"
#include "config_handlers_export.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"

#include <string>

namespace DAnCEX11
{
  class Config_Handlers_Export CDPPlan_Loader
    : public Plan_Loader_base
  {
  public:
    ~CDPPlan_Loader () override = default;

    bool read_plan (const std::string& filename,
                    Deployment::DeploymentPlan& plan) override;

    bool read_domain (const std::string& filename,
                      Deployment::Domain& domain) override;

    static CDPPlan_Loader* instance ();

  private:
    CDPPlan_Loader () = default;
  };

  class Config_Handlers_Export CDPPlan_Loader_Svc
    : public ACE_Service_Object
  {
  public:
    ~CDPPlan_Loader_Svc () override;

    /// Initializes object when dynamic linking occurs.
    int init (int argc, ACE_TCHAR *argv[]) override;

    /// Terminates object when dynamic unlinking occurs.
    int fini () override;

    static int Initializer ();
  };

  ACE_STATIC_SVC_DECLARE (CDPPlan_Loader_Svc)
  ACE_FACTORY_DECLARE (Config_Handlers, CDPPlan_Loader_Svc)
};

#endif /* DANCEX11_DEPLOYMENT_CDP_PLAN_LOADER_H */
