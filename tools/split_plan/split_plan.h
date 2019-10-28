// -*- C++ -*-
/**
 * @file split_plan.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DAnCEX11_SPLIT_PLAN_H
#define DAnCEX11_SPLIT_PLAN_H

#pragma once

#include /**/ "ace/config-all.h"

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dance_split_plan_export.h"
#include "sub_uuid_generator.h"

#include <list>
#include <vector>
#include <unordered_map>
#include <map>

namespace DAnCEX11
{
  template <class SPLITTER, class UUIDGEN = Unique_Sub_UUID_Generator>
  class Split_Plan
  {
  public:
    typedef SPLITTER                          TPlanSplitter;
    typedef typename TPlanSplitter::KEY       TSubPlanKey;
    typedef typename TPlanSplitter::FILTER    TSplitFilter;
    typedef UUIDGEN                           TSubUUIDGen;

    typedef std::map<TSubPlanKey,
                     Deployment::DeploymentPlan
                    > TSubPlans;

    typedef std::list <uint32_t> InstanceList;
    typedef std::pair < TSubPlanKey, InstanceList > SubPlanList;

    typedef std::vector < SubPlanList > TSubPlanList;

    typedef typename TSubPlans::iterator TSubPlanIterator;
    typedef typename TSubPlans::const_iterator TSubPlanConstIterator;

    Split_Plan ();

    void split_plan (const Deployment::DeploymentPlan &plan,
                     const TSplitFilter &filter = TSplitFilter ());

    TSubPlans & plans ();

  private:
    bool find_sub_plan (const TPlanSplitter &plan_splitter,
                        uint32_t instance,
                        TSubPlanKey &sub_plan_key,
                        Deployment::DeploymentPlan &sub_plan);


    TSubPlans sub_plans_;
  };
}

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "split_plan.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("split_plan.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

#endif /* DAnCEX11_SPLIT_PLAN_H */
