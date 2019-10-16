/**
 * @file node_splitter.h
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DAnCEX11_NODE_SPLITTER_H
#define DAnCEX11_NODE_SPLITTER_H

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dance_split_plan_export.h"
#include <vector>

namespace DAnCEX11
{
  class DAnCE_Split_Plan_Export Node_Splitter
  {
  public:
    typedef std::string       KEY;
    typedef std::string       FILTER;

    Node_Splitter (const Deployment::DeploymentPlan &plan);

    bool    match_instance (uint32_t instance,
                            const FILTER &filter) const;

    bool    match_sub_plan (uint32_t instance,
                            const KEY &sub_plan_key) const;

    void    prepare_sub_plan (uint32_t instance,
                              Deployment::DeploymentPlan &sub_plan,
                              KEY &sub_plan_key);

    void    finalize_sub_plan (Deployment::DeploymentPlan &sub_plan,
                               KEY &sub_plan_key);

  private:
    const Deployment::DeploymentPlan &plan_;
  };
}
#endif /* DAnCEX11_NODE_SPLITTER_H */
