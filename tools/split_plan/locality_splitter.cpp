// -*- C++ -*-
/**
 * @file locality_splitter.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "locality_splitter.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "ace/UUID.h"
#include <algorithm>

namespace DAnCEX11
{
  Locality_Splitter::LocalityKey::LocalityKey ()
    : loc_manager_ ((uint32_t)ULONG_MAX)
    , implicit_lm_ (true)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityKey::constructor");
  }

  Locality_Splitter::LocalityKey::LocalityKey (
      const Locality_Splitter::LocalityKey& k)
    : plan_uuid_ (k.uuid ()),
      node_ (k.node ()),
      instances_ (k.instances ()),
      loc_manager_ (k.loc_manager_),
      implicit_lm_ (k.implicit_lm_)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityKey::destructor");
  }

  Locality_Splitter::LocalityKey&
  Locality_Splitter::LocalityKey::operator =(
      const Locality_Splitter::LocalityKey& k)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityKey::operator=");

    this->plan_uuid_ = k.uuid ();
    this->node_ = k.node ();
    this->instances_ = k.instances ();
    this->loc_manager_ = k.loc_manager_;
    this->implicit_lm_ = k.implicit_lm_;
    return *this;
  }

  bool Locality_Splitter::LocalityKey::has_instance (
    uint32_t instance) const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityKey::has_instance");

    Locality_Splitter::TInstanceList::iterator it =
        std::find (const_cast<Locality_Splitter::LocalityKey*> (this)
                      ->instances_.begin (),
                   const_cast<Locality_Splitter::LocalityKey*> (this)
                      ->instances_.end (),
                   instance);
    return it != this->instances_.end ();
  }

  void Locality_Splitter::LocalityKey::add_instance (uint32_t instance)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityKey::add_instance");

    this->instances_.push_back (instance);
  }

  std::string Locality_Splitter::LocalityKey::locality_manager_label(const Deployment::DeploymentPlan &sub_plan) const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::locality_manager_label");

    if (this->has_explicit_locality_manager ())
    {
      DANCEX11_LOG_DEBUG("Locality_Splitter::LocalityFilter::locality_manager_label - explicit LM label");

      // find the LM instance (cannot use the recorded instance indexes as these are out of sync with the sub_plan)
      for (const Deployment::InstanceDeploymentDescription& inst : sub_plan.instance())
      {
        // get instance type
        const std::string instance_type =
          DAnCEX11::Utility::get_instance_type (
            sub_plan.implementation ()[inst.implementationRef()].execParameter ());
        if (!instance_type.empty ())
        {
          if (instance_type == DANCE_LOCALITYMANAGER)
          {
            // LM found

            // check for existence of DANCE_LM_PROCESSNAME exec property for LM instance
            for (const Deployment::Property& cfgProp : inst.configProperty ())
            {
              if (cfgProp.name() == DAnCEX11::DANCE_LM_PROCESSNAME)
              {
                std::string lm_name;
                if (cfgProp.value() >>= lm_name)
                {
                  return this->node_ + "-" + lm_name;
                }
                else
                {
                  DANCEX11_LOG_ERROR("Locality_Splitter::LocalityFilter::locality_manager_label - " \
                        "Failed to extract " << DAnCEX11::DANCE_LM_PROCESSNAME << " for LM label");
                }
              }
            }
            // use instance name
            return this->node_ + "-" + inst.name();
          }
        }
      }
    }
    return this->to_string();
  }

  Locality_Splitter::LocalityKey::operator std::string () const
  {
    return this->to_string ();
  }

  std::string Locality_Splitter::LocalityKey::to_string () const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityKey::to_string");

    char buf[16];
    std::string id (this->node_);
    // This code only generates unique id.
    ACE_OS::snprintf (buf, sizeof (buf), "-%p", this);
    id += buf;
    return id;
  }

  Locality_Splitter::LocalityFilter::LocalityFilter ()
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::constructor");
  }

  const std::string& Locality_Splitter::LocalityFilter::node () const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::node");

    return this->node_;
  }

  void Locality_Splitter::LocalityFilter::node (const std::string &node_name)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::node");

    this->node_ = node_name;
  }

  const Locality_Splitter::TInstanceNames&
  Locality_Splitter::LocalityFilter::included_instances () const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::included_instances");

    return this->included_instances_;
  }

  bool Locality_Splitter::LocalityFilter::includes_instance (
      const std::string &instance_name) const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::includes_instance");

    Locality_Splitter::TInstanceNames::iterator it =
        std::find (const_cast<Locality_Splitter::LocalityFilter*> (this)
                      ->included_instances_.begin (),
                   const_cast<Locality_Splitter::LocalityFilter*> (this)
                      ->included_instances_.end (),
                   instance_name);
    return it != this->included_instances_.end ();
  }

  void Locality_Splitter::LocalityFilter::add_instance (
      const std::string &instance_name)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::LocalityFilter::add_instance");

    this->included_instances_.push_back (instance_name);
  }

  Locality_Splitter::Locality_Splitter (
    const Deployment::DeploymentPlan &plan)
    : plan_ (plan)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::constructor");
  }

  bool Locality_Splitter::match_instance (
      uint32_t instance,
      const FILTER &filter) const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::match_instance");

    // check if the node for the instance matches the filter
    bool match =
        filter.node ().empty () ||
          (filter.node () == this->plan_.instance ()[instance].node ());
    if (match)
      {
        // check if instance explicitly included or any allowed
        match = filter.included_instances ().empty () ||
                  filter.includes_instance (
                    this->plan_.instance ()[instance].node ());
      }
    return match;
  }

  bool Locality_Splitter::match_sub_plan (
      uint32_t instance,
      const KEY &sub_plan_key) const
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::match_sub_plan");

    // check if the node for the instance matches the sub plan
    bool match =
          (sub_plan_key.node () == this->plan_.instance ()[instance].node ());

    // if the node matches see if the instance is or can be included in the
    // sub plan
    if (match)
      {
        if (sub_plan_key.has_instance (instance))   return true; // matches

        // see if the locality constraints allow this instance to be
        // included in this sub plan
        uint32_t num_localities =
            ACE_Utils::truncate_cast<uint32_t> (this->plan_.localityConstraint ().size ());
        for (uint32_t l = 0; l < num_localities; ++l)
          {
            const Deployment::PlanLocality &ploc =
              this->plan_.localityConstraint ()[l];
            // only interested in real constraints
            if (ploc.constraint () != Deployment::PlanLocalityKind::PlanNoConstraint)
              {
                // see if the instance we're matching is
                // included in this constraint
                bool apply_constraint = false;
                for (uint32_t i = 0;
                     i < ploc.constrainedInstanceRef ().size ();
                     ++i)
                  {
                    apply_constraint =
                      ploc.constrainedInstanceRef ()[i] == instance;
                    if (apply_constraint)
                      break;
                  }

                if (ploc.constraint () == Deployment::PlanLocalityKind::PlanDifferentProcess)
                  {
                    if (apply_constraint) // in case the constraint applies
                      {
                        // check if there are no incompatible
                        // instances already in
                        // the sub plan we're matching
                        for (uint32_t i = 0;
                            i < ploc.constrainedInstanceRef ().size ();
                            ++i)
                          {
                            if (ploc.constrainedInstanceRef ()[i] != instance)
                              {
                                uint32_t incompatible_instance =
                                    ploc.constrainedInstanceRef ()[i];
                                if (sub_plan_key.has_instance (
                                              incompatible_instance))
                                  {
                                    // sub plan includes an instance which
                                    // is constrained to a different locality
                                    return false;
                                  }
                              }
                          }
                      }
                    // in case the constraint does not apply to the instance
                    // we're matching forget about it
                  }
                else // Deployment::PlanSameProcess
                  {
                    if (apply_constraint) // in case the constraint applies
                      {
                        // check to see that all instances included in the
                        // sub plan also comply with this constraint
                        // since if one complies with the constraint
                        // than all should, it is enough to test the first
                        // instance in the sub plan
                        if (!sub_plan_key.instances ().empty ())
                          {
                            uint32_t first_inst_nr =
                              sub_plan_key.instances ().front ();
                            bool included = false;
                            for (uint32_t j = 0;
                                j < ploc.constrainedInstanceRef ().size ();
                                ++j)
                              {
                                if (first_inst_nr ==
                                      ploc.constrainedInstanceRef ()[j])
                                  {
                                    included = true;
                                    break;
                                  }
                              }
                            if (!included)
                              {
                                // sub plan includes instance(s) which do(es)
                                // not comply with this constraint
                                return false;
                              }
                          }
                      }
                    else
                      {
                        // check to see that the constraint also does *not*
                        // apply to any of the instances included in the sub
                        // plan since if one complies with the constraint than
                        // all should, it is enough to test the first instance
                        // in the sub plan
                        if (!sub_plan_key.instances ().empty ())
                          {
                            uint32_t first_inst_nr =
                              sub_plan_key.instances ().front ();
                            for (uint32_t j = 0;
                                j < ploc.constrainedInstanceRef ().size ();
                                ++j)
                              {
                                if (first_inst_nr ==
                                      ploc.constrainedInstanceRef ()[j])
                                  {
                                    // sub plan complies with this constraint
                                    return false;
                                  }
                              }
                          }
                      }
                  }
              }
          }
      }
    return match;
  }

  void Locality_Splitter::prepare_sub_plan (
      uint32_t instance,
      Deployment::DeploymentPlan &sub_plan,
      KEY &sub_plan_key)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::prepare_sub_plan");

    // first time?
    if (sub_plan_key.uuid ().empty ())
      {
        // use UUID to uniquely identify sub plan key
        sub_plan_key.uuid (sub_plan.UUID ());
      }
    if (sub_plan_key.node () != this->plan_.instance ()[instance].node ())
      {
        // set sub plan key to node name for instance
        sub_plan_key.node (this->plan_.instance ()[instance].node ());
      }
    if (!sub_plan_key.has_instance (instance))
      {
        sub_plan_key.add_instance (instance);

        if (!sub_plan_key.has_locality_manager ())
          {
            // check if this is the LocalityManager instance
            uint32_t impl_idx =
              this->plan_.instance ()[instance].implementationRef ();

            if (impl_idx >= this->plan_.implementation ().size ())
              {
                DANCEX11_LOG_ERROR ("Locality_Splitter::prepare_sub_plan - " <<
                                    "Invalid implementation index");
              }
            else
              {
                const std::string instance_type =
                  DAnCEX11::Utility::get_instance_type (
                    plan_.implementation ()[impl_idx].execParameter ());
                if (!instance_type.empty ())
                  {
                    if (instance_type == DANCE_LOCALITYMANAGER)
                      {
                        // mark locality manager instance offset
                        sub_plan_key.locality_manager_instance (
                            ACE_Utils::truncate_cast<uint32_t> (sub_plan_key.instances ().size ()) - 1);

                        DANCEX11_LOG_DEBUG ("Locality_Splitter::prepare_sub_plan - " <<
                                            "Found locality manager instance " <<
                                            instance << ":" << plan_.instance ()[instance].name ());
                      }
                  }
              }
          }
      }
    if (sub_plan.label ().empty ())
      {
        // derive descriptive label
        std::string sub_label ("Split plan from ");
        if (!this->plan_.label ().empty ())
          {
            sub_label += this->plan_.label ();
          }
        else
          {
            sub_label += this->plan_.UUID ();
          }
        sub_label += " for a Locality on Node ";
        sub_label += sub_plan_key.node ();
        sub_plan.label (sub_label);
      }
  }

  void Locality_Splitter::finalize_sub_plan (
      Deployment::DeploymentPlan &sub_plan,
      KEY &sub_plan_key)
  {
    DANCEX11_LOG_TRACE ("Locality_Splitter::finalize_sub_plan");

    // check for availability of LocalityManager instance
    if (!sub_plan_key.has_locality_manager ())
      {
        // add minimal default LocalityManager instance to sub plan
        uint32_t impl_index =
            ACE_Utils::truncate_cast<uint32_t> (sub_plan.implementation ().size ());
        sub_plan.implementation ().push_back (::Deployment::MonolithicDeploymentDescription ());
        uint32_t inst_index =
            ACE_Utils::truncate_cast<uint32_t> (sub_plan.instance ().size ());
        sub_plan.instance ().push_back (::Deployment::InstanceDeploymentDescription ());
        sub_plan.instance ().back ().implementationRef (impl_index);

        // set correct implementation type
        Deployment::Property exec_property;
        exec_property.name (IMPL_TYPE);
        exec_property.value () <<= DANCE_LOCALITYMANAGER;
        sub_plan.implementation ().back ().execParameter ().push_back (exec_property);

        // create unique name for Locality Manager
        ACE_Utils::UUID uuid;
        ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID (uuid);
        std::string lm_name ("Locality Manager [");
        lm_name += uuid.to_string ()->c_str ();
        lm_name += "]";
        sub_plan.instance ().back ().name (lm_name);

        DANCEX11_LOG_DEBUG ("Locality_Splitter::finalize_sub_plan - " <<
                            "No locality manager found, created a default locality named<" <<
                            sub_plan.instance ().back ().name () << ">");

        // add instance to sub plan key
        sub_plan_key.add_instance (inst_index);
        // mark as implicit locality manager
        sub_plan_key.locality_manager_instance (
            ACE_Utils::truncate_cast<uint32_t> (sub_plan_key.instances ().size ()) - 1, true);
      }
  }
}
