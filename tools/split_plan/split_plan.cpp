// -*- C++ -*-
/**
 * @file split_plan.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DAnCEX11_SPLIT_PLAN_CPP
#define DAnCEX11_SPLIT_PLAN_CPP

#pragma once

#include "split_plan.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  template <class SPLITTER, class UUIDGEN>
  Split_Plan<SPLITTER, UUIDGEN>::Split_Plan ()
  {
    DANCEX11_LOG_TRACE ("Split_Plan::constructor");
  }

  template <class SPLITTER, class UUIDGEN>
  typename Split_Plan<SPLITTER, UUIDGEN>::TSubPlans &
  Split_Plan<SPLITTER, UUIDGEN>::plans ()
  {
    DANCEX11_LOG_TRACE ("Split_Plan::plans");

    return this->sub_plans_;
  }

  template <class SPLITTER, class UUIDGEN>
  void Split_Plan<SPLITTER, UUIDGEN>::split_plan (
                                                  const Deployment::DeploymentPlan &plan,
                                                  const typename Split_Plan<SPLITTER, UUIDGEN>::TSplitFilter &filter)
  {
    DANCEX11_LOG_TRACE ("Split_Plan::split_plan");

    TSubUUIDGen   sub_uuid_gen;
    TPlanSplitter plan_splitter (plan);

    // clear any bound sub plans
    this->sub_plans_.clear ();

    DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Creating sub-plans");

    /*
     *  Selection phase
     */

    // Create and prepare the necessary sub-plans
    for (uint32_t i = 0; i < plan.instance ().size (); ++i)
      {
        DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Matching instance " <<
                            plan.instance ()[i].name ());

        // match instance against filter criteria
        if (!plan_splitter.match_instance (i, filter))
          continue; // next instance

        // Empty sub plan and key
        Deployment::DeploymentPlan  sub_plan;
        TSubPlanKey sub_plan_key;

        // check if instance belongs to any existing sub plan
        if (!this->find_sub_plan (plan_splitter, i, sub_plan_key, sub_plan))
          {
            // generic initialization of additional sub plan

            // use UUID generator to generate UUID for sub plan
            sub_uuid_gen.generate_sub_uuid (plan,
                                            sub_plan,
                                            ACE_Utils::truncate_cast<uint32_t> (this->sub_plans_.size ()));

            DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Initializing additional sub plan " <<
                                sub_plan.UUID ());

            // @@ There is an optimization point here,
            // since we really don't have to pass the entire
            // CIAOServerResources into each individual child plan.
            sub_plan.infoProperty (plan.infoProperty ());

            // add property referring to parent plan
            Deployment::Property parent_property;
            parent_property.name ("nl.remedy.it.DnCX11.ParentPlan");
            parent_property.value () <<= plan.UUID ();
            sub_plan.infoProperty ().push_back (parent_property);

            // initialize locality constraints
            uint32_t const num_localities =
                ACE_Utils::truncate_cast<uint32_t> (plan.localityConstraint ().size ());
            Deployment::PlanLocalities default_loc;

            for (uint32_t i = 0; i < num_localities; ++i)
              {
                default_loc.push_back (Deployment::PlanLocality ());
                default_loc[i].constraint (Deployment::PlanLocalityKind::PlanNoConstraint);
              }

            sub_plan.localityConstraint (default_loc);
          }
        else
          {
            DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Instance " <<
                                plan.instance ()[i].name () <<
                                " matched to sub plan " <<
                                sub_plan.UUID ());
          }

        DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Preparing sub plan " <<
                            sub_plan.UUID () <<
                            " for instance " <<
                            plan.instance ()[i].name ());

        // Prepare the sub plan for this instance
        plan_splitter.prepare_sub_plan (i, sub_plan, sub_plan_key);

        // (re-)register sub plan
        this->sub_plans_[sub_plan_key] = sub_plan;
      }

    DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Selection phase produced " <<
                        this->sub_plans_.size () <<
                        " child plans, proceeding to copying phase");

    // build instance lists
    TSubPlanList sub_plan_list;
    for (TSubPlanIterator iter_plans = this->sub_plans_.begin ();
         iter_plans != this->sub_plans_.end ();
         ++iter_plans)
      {
        // get the sub plan and key for current instance
        const TSubPlanKey& sub_plan_key = iter_plans->first;
        SubPlanList list_item;
        list_item.first = sub_plan_key;

        sub_plan_list.push_back (list_item);
      }

      for (uint32_t i = 0; i < sub_plan_list.size (); ++i)
      {
        // get the sub plan and key for current instance
        TSubPlanKey& sub_plan_key = sub_plan_list[i].first;
        std::list < uint32_t > &instances = sub_plan_list[i].second;

        for (uint32_t i = 0; i < plan.instance ().size (); ++i)
          {
            if (plan_splitter.match_sub_plan (i, sub_plan_key))
              instances.push_back (i);
          }
      }

    /*
     *  Copying phase
     */

      // (1) Iterate over the <instance> field of the global DeploymentPlan
      //     structure.
      // (2) Retrieve the necessary information to contruct the sub plans
      //     one by one.
      for (uint32_t i = 0; i < (sub_plan_list.size ()); ++i)
        {
          // get the sub plan and key for current instance
          TSubPlanKey& sub_plan_key = sub_plan_list[i].first;

          ::Deployment::DeploymentPlan sub_plan = this->sub_plans_[sub_plan_key];

          // Get instance list
          std::list <uint32_t> &instances = sub_plan_list[i].second;

          for (InstanceList::const_iterator i = instances.begin ();
               i != instances.end ();
               ++i)
            {
              uint32_t pos = *i;

              DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Processing instance: " <<
                                  plan.instance ()[pos].name ());

              // Get the instance deployment description
              const Deployment::InstanceDeploymentDescription & my_instance =
                plan.instance ()[pos];

              // Fill in the contents of the sub plan entry.

              // Append the "MonolithicDeploymentDescriptions implementation"
              // field with the "implementation" which is specified by the
              // <implementationRef> field of <my_instance> entry.
              Deployment::MonolithicDeploymentDescription const & my_implementation
                = plan.implementation ()[my_instance.implementationRef ()];

              uint32_t const index_imp =
                  ACE_Utils::truncate_cast<uint32_t> (sub_plan.implementation ().size ());
              sub_plan.implementation ().push_back (my_implementation);

              // update the "ArtifactDeploymentDescriptions" <artifact> field
              // of the sub plan with the artifacts referenced by the <artifactRef>
              // sequence of the added implementation
              // NOTE: if the artifact description defines a node value it should only
              //       be copied to the subplan if the node value matches the node
              //       value of the instance referenced by <my_instance>

              // Initialize with the correct sequence length.
              std::vector<uint32_t> ulong_seq;

              // append the "ArtifactDeploymentDescriptions"
              uint32_t const impl_length =
                  ACE_Utils::truncate_cast<uint32_t> (my_implementation.artifactRef ().size ());
              uint32_t const artifact_offset =
                  ACE_Utils::truncate_cast<uint32_t> (sub_plan.artifact ().size ());
              uint32_t iter_off {};

              for (uint32_t iter = 0;
                   iter < impl_length;
                   iter ++)
                {
                  uint32_t const artifact_ref = my_implementation.artifactRef ()[iter];

                  const Deployment::ArtifactDeploymentDescription &add =
                      plan.artifact ()[artifact_ref];
                  if (add.node ().empty () || my_instance.node () == add.node ())
                  {
                    // copy the artifact entry to the sub plan
                    sub_plan.artifact ().push_back (plan.artifact ()[artifact_ref]);

                    // Fill in the artifactRef field of the
                    // MonolithicDeploymentDescription
                    ulong_seq.push_back (artifact_offset + iter_off);

                    ++iter_off;
                  }
                }

              // Change the <artifactRef> field of the added "implementation" to
              // reference the artifact field of the sub plan
              sub_plan.implementation ()[index_imp].artifactRef (ulong_seq);

              // Append the "InstanceDeploymentDescription instance" field with
              // an "instance" copy which is almost the same as the "instance" in
              // the global plan except the <implementationRef> field.
              uint32_t const index_ins =
                  ACE_Utils::truncate_cast<uint32_t> (sub_plan.instance ().size ());
              sub_plan.instance ().push_back (my_instance);

              DANCEX11_LOG_DEBUG("Split_Plan::split_plan - copied instance " << pos << " to " << index_ins);

              // Update the <implementationRef> field of the "instance".
              sub_plan.instance ()[index_ins].implementationRef (index_imp);

              DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Processing connections.");
              // Copy connections
              for (uint32_t j = 0; j < plan.connection ().size (); ++j)
                {
                  DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Connection: " <<
                                      plan.connection ()[j].name ());
                  for (uint32_t k = 0;
                       k < plan.connection ()[j].internalEndpoint ().size (); ++k)
                    {
                      DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Endpoint: " <<
                                          plan.connection ()[j].internalEndpoint ()[k].portName () <<
                                          "(" <<
                                          (plan.connection ()[j].internalEndpoint ()[k].provider () ? "provider" : "client") <<
                                          ")");

                      // check if connection endpoint references the instance (i)
                      // we're adding to the sub plan
                      if (pos == plan.connection ()[j].internalEndpoint ()[k].instanceRef ())
                        {
                          Deployment::PlanConnectionDescription *
                            connection_copied = 0;

                          // check if we already copied this connection
                          // (for an earlier endpoint match)
                          for (uint32_t m = 0;
                               m < sub_plan.connection ().size (); ++m)
                            {
                              if (plan.connection ()[j].name () ==
                                                  sub_plan.connection ()[m].name ())
                                {
                                  connection_copied = &sub_plan.connection ()[m];
                                  break;
                                }
                            }

                          if (!connection_copied)
                            {
                              // Copy the connection
                              sub_plan.connection ().push_back (plan.connection ()[j]);
                              connection_copied = &sub_plan.connection ().back ();
                              connection_copied->internalEndpoint ().clear ();
                            }

                          // Copy the endpoint
                          uint32_t const index_ep =
                              ACE_Utils::truncate_cast<uint32_t> (connection_copied->internalEndpoint ().size ());

                          DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Copying endpoint " <<
                                              k << " into endpoint " << index_ep);

                          connection_copied->internalEndpoint ().push_back (
                            plan.connection ()[j].internalEndpoint ()[k]);
                          connection_copied->internalEndpoint ()[index_ep].instanceRef (index_ins);
                        }
                    }
                }

              // copy any locality constraints matching the instance we're
              // adding to the sub plan
              for (uint32_t j = 0;
                   j < plan.localityConstraint ().size (); ++j)
                {
                  const Deployment::PlanLocality &loc =
                    plan.localityConstraint ()[j];
                  for (uint32_t k = 0;
                       k < loc.constrainedInstanceRef ().size (); ++k)
                    {
                      // we are the same instance...
                      if (loc.constrainedInstanceRef ()[k] == pos)
                        {
                          // add the reference to the added instance to the sub plan's
                          // corresponding constraint.
                          uint32_t const sub_loc_len =
                              ACE_Utils::truncate_cast<uint32_t> (
                                  sub_plan.localityConstraint ()[j].constrainedInstanceRef ().size ());

                          DANCEX11_LOG_TRACE ("Split_Plan::split_plan - Found matching locality constraint " <<
                                              j << ":" << k << "," <<
                                              " adding to " <<
                                              j << ":" << sub_loc_len << " as " << index_ins);

                          // set the correct constraint type
                          sub_plan.localityConstraint ()[j].constraint (
                            loc.constraint ());

                          // add instance reference to matched constraint
                          // thank god someone made an 18 and 20+ char
                          // member variable...
                          sub_plan.localityConstraint ()[j].constrainedInstanceRef ().push_back (index_ins);
                        }
                    }
                }
            }
          // rebing updated sub plan
          this->sub_plans_[sub_plan_key] = sub_plan;
        }
    /*
     *  Finalization
     */

    // finalize all sub plans
    for (TSubPlanIterator iter_plans = this->sub_plans_.begin ();
         iter_plans != this->sub_plans_.end ();
         ++iter_plans)
      {
        // get the sub plan and key for current instance
        ::Deployment::DeploymentPlan& sub_plan = iter_plans->second;
        const TSubPlanKey& sub_plan_key = iter_plans->first;

        // finalize sub plan
        plan_splitter.finalize_sub_plan (sub_plan, const_cast<TSubPlanKey&> (sub_plan_key));
      }
  }

  template <class SPLITTER, class UUIDGEN>
  bool
  Split_Plan<SPLITTER, UUIDGEN>::find_sub_plan (
                                                const TPlanSplitter &plan_splitter,
                                                uint32_t instance,
                                                TSubPlanKey &sub_plan_key,
                                                Deployment::DeploymentPlan &sub_plan)
  {
    DANCEX11_LOG_TRACE ("Split_Plan::find_sub_plan");

    for (TSubPlanIterator iter_plans = this->sub_plans_.begin ();
         iter_plans != this->sub_plans_.end ();
         ++iter_plans)
      {
        if (plan_splitter.match_sub_plan (instance, iter_plans->first))
          {
            // get the sub plan and key for current instance
            sub_plan_key = iter_plans->first;
            sub_plan = this->sub_plans_[sub_plan_key];
            this->sub_plans_.erase (sub_plan_key);
            return true;
          }
      }
    return false;
  }
}

#endif /* DAnCEX11_SPLIT_PLAN_CPP */
