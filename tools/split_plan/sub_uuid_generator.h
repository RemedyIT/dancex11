// -*- C++ -*-
/**
 * @file sub_uuid_generator.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DAnCEX11_SUB_UUID_GENERATOR_H
#define DAnCEX11_SUB_UUID_GENERATOR_H

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dance_split_plan_export.h"

namespace DAnCEX11
{
  class DAnCE_Split_Plan_Export Copy_UUID_Generator
  {
  public:
    Copy_UUID_Generator ();

    void      generate_sub_uuid (const Deployment::DeploymentPlan &parent_plan,
                                 Deployment::DeploymentPlan &sub_plan,
                                 uint32_t sub_plan_index);
  };

  class DAnCE_Split_Plan_Export Unique_Sub_UUID_Generator
  {
  public:
    Unique_Sub_UUID_Generator ();

    void      generate_sub_uuid (const Deployment::DeploymentPlan &parent_plan,
                                 Deployment::DeploymentPlan &sub_plan,
                                 uint32_t sub_plan_index);
  };

}
#endif /* DAnCEX11_SUB_UUID_GENERATOR_H */
