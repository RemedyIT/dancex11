/**
 * @file node_splitter.cpp
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */


#include "node_splitter.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "ace/UUID.h"
#include <algorithm>

namespace DAnCEX11
{
  Node_Splitter::Node_Splitter (const Deployment::DeploymentPlan &plan)
  : plan_ (plan)
  {
  }

  bool Node_Splitter::match_instance (uint32_t instance,
                                      const FILTER &filter) const
  {
    if (filter.empty ()) return true;
    if (filter == this->plan_.instance()[instance].node())
      return true;
    else
      return false;

  }

  bool Node_Splitter::match_sub_plan (uint32_t instance,
                                      const KEY &sub_plan_key) const
  {
    if (sub_plan_key == this->plan_.instance()[instance].node())
      return true;
    else
      return false;
  }

  void Node_Splitter::prepare_sub_plan (uint32_t instance,
                                        Deployment::DeploymentPlan &sub_plan,
                                        KEY &sub_plan_key)
  {
    if (sub_plan_key != this->plan_.instance()[instance].node())
    {
      // set sub plan key to node name for instance
      sub_plan_key = this->plan_.instance()[instance].node();
    }

    if (sub_plan.label().empty())
    {
      // derive descriptive label
      std::string sub_label ("Split plan from ");
      if (!this->plan_.label().empty())
      {
        sub_label += this->plan_.label();
      }
      else
      {
        sub_label += this->plan_.UUID();
      }
      sub_label += " for Node ";
      sub_label += sub_plan_key;
      sub_plan.label() = sub_label;
    }
  }

  void Node_Splitter::finalize_sub_plan (Deployment::DeploymentPlan & /*sub_plan*/,
                                         KEY & /*sub_plan_key*/)
  {
    // nothing to do
  }
}
