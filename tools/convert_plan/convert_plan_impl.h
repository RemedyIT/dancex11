// -*- C++ -*-
/**
 * @file   convert_plan_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_CONVERT_PLAN_H
#define DANCEX11_CONVERT_PLAN_H

#include "tools/convert_plan/dancex11_convert_plan_export.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_targetdataC.h"

namespace DAnCEX11
{
  class DAnCEX11_Convert_Plan_Export Convert_Plan
  {
  public:
    static bool write_cdr_plan (const std::string& filename, const Deployment::DeploymentPlan &plan);
    static bool write_cdr_domain (const std::string& filename, const Deployment::Domain &domain);
  private:
    static bool write_cdr_file (const std::string& output_filename, const TAO_OutputCDR& output_cdr);
  };
}

#endif
