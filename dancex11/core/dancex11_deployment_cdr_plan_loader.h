// -*- C++ -*-
/**
 * @file dancex11_deployment_cdr_plan_loader.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_DEPLOYMENT_CDR_PLAN_LOADER_H
#define DANCEX11_DEPLOYMENT_CDR_PLAN_LOADER_H

#pragma once

#include "ace/Service_Object.h"

#include "dancex11_deployment_cdrplanloader_export.h"
#include "dancex11_deployment_plan_loader.h"

#include <string>

namespace DAnCEX11
{
  class DANCEX11_CDRPLAN_LOADER_Export CDRPlan_Loader
    : public Plan_Loader_base
  {
  public:
    virtual ~CDRPlan_Loader () = default;

    virtual bool read_plan (const std::string& filename,
                            Deployment::DeploymentPlan& plan) override;

    virtual bool read_domain (const std::string& filename,
                              Deployment::Domain& dom) override;

    static CDRPlan_Loader* instance ();

  private:
    CDRPlan_Loader () = default;
  };

  class DANCEX11_CDRPLAN_LOADER_Export CDRPlan_Loader_Svc
    : public ACE_Service_Object
  {
  public:
    virtual ~CDRPlan_Loader_Svc ();

    /// Initializes object when dynamic linking occurs.
    virtual int init (int argc, ACE_TCHAR *argv[]) override;

    /// Terminates object when dynamic unlinking occurs.
    virtual int fini () override;

    static int Initializer ();
  };

  ACE_STATIC_SVC_DECLARE (CDRPlan_Loader_Svc)
  ACE_FACTORY_DECLARE (DANCEX11_CDRPLAN_LOADER, CDRPlan_Loader_Svc)

};

#endif /* DANCEX11_DEPLOYMENT_CDR_PLAN_LOADER_H */
