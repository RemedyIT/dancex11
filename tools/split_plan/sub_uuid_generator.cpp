// -*- C++ -*-
/**
 * @file sub_uuid_generator.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "sub_uuid_generator.h"
#include <sstream>

namespace DAnCEX11
{
  void Unique_Sub_UUID_Generator::generate_sub_uuid (
      const Deployment::DeploymentPlan &parent_plan,
      Deployment::DeploymentPlan &sub_plan,
      uint32_t sub_plan_index)
    {
      std::ostringstream sub_uuid_str;
      sub_uuid_str << parent_plan.UUID () << "_" << sub_plan_index;
      sub_plan.UUID (sub_uuid_str.str ());
    }

  void Copy_UUID_Generator::generate_sub_uuid (
      const Deployment::DeploymentPlan &parent_plan,
      Deployment::DeploymentPlan &sub_plan,
      uint32_t /*sub_plan_index*/)
    {
      sub_plan.UUID (parent_plan.UUID ());
    }
}
