// -*- C++ -*-
/**
 * @file locality_splitter.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DAnCEX11_LOCALITY_SPLITTER_H
#define DAnCEX11_LOCALITY_SPLITTER_H

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dance_split_plan_export.h"
#include <vector>

namespace DAnCEX11
{
  class DAnCE_Split_Plan_Export Locality_Splitter
  {
  public:
    using TInstanceList = std::vector<uint32_t>;

    class DAnCE_Split_Plan_Export LocalityKey
    {
    public:
      LocalityKey ();
      LocalityKey (const LocalityKey& k);

      LocalityKey& operator =(const LocalityKey& k);

      const std::string&    uuid () const
      {
        return this->plan_uuid_;
      }
      void                  uuid (const std::string &plan_uuid)
      {
        this->plan_uuid_ = plan_uuid;
      }

      const std::string&    node () const
      {
        return this->node_;
      }
      void                  node (const std::string &node_name)
      {
        this->node_ = node_name;
      }

      const TInstanceList&  instances () const
      {
        return this->instances_;
      }
      bool                  has_instance (uint32_t instance) const;
      void                  add_instance (uint32_t instance);

      bool                  has_locality_manager () const
      {
        return this->loc_manager_ < this->instances_.size ();
      }
      uint32_t              locality_manager_instance () const
      {
        return this->has_locality_manager () ? this->loc_manager_ : 0;
      }
      void                  locality_manager_instance (uint32_t sub_plan_index, bool implicit_lm=false)
      {
        if (sub_plan_index < this->instances_.size ())
        {
          this->loc_manager_ = sub_plan_index;
          this->implicit_lm_ = implicit_lm;
        }
      }
      bool                  has_explicit_locality_manager () const
      {
        return !this->implicit_lm_;
      }

      std::string           locality_manager_label(const Deployment::DeploymentPlan &sub_plan) const;

      bool  operator ==(const LocalityKey &other_key) const
      {
        return this->uuid () == other_key.uuid ();
      }
      bool  operator <(const LocalityKey &other_key) const
      {
        return this->uuid () < other_key.uuid ();
      }

      std::string to_string () const;

      operator std::string () const;

    private:
      std::string plan_uuid_;
      std::string node_;
      TInstanceList instances_;
      uint32_t loc_manager_;
      bool implicit_lm_;
    };

    struct LocalityKeyHash
    {
      size_t operator ()(const LocalityKey& r) const
      {
        return std::hash<std::string>()(r.node ());
      }
    };

    using KEY = LocalityKey;
    using KEY_HASH = LocalityKeyHash;

    using TInstanceNames = std::vector<std::string>;

    class DAnCE_Split_Plan_Export LocalityFilter
    {
    public:
      LocalityFilter ();

      const std::string&    node () const;
      void                  node (const std::string &node_name);

      const TInstanceNames& included_instances () const;
      bool                  includes_instance (
                                const std::string &instance_name) const;
      void                  add_instance (const std::string &instance_name);

    private:
      std::string node_;
      TInstanceNames included_instances_;
    };

    using FILTER = LocalityFilter;

    Locality_Splitter (const Deployment::DeploymentPlan &plan);

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
#endif /* DAnCEX11_LOCALITY_SPLITTER_H */
