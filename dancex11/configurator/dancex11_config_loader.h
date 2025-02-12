/**
 * @file    dancex11_config_loader.h
 * @author  Martin Corino
 *
 * @brief   Loader for DAnCEX11 Deployment config files.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#pragma once

#include "dancex11_configurator_export.h"
#include "dancex11_config_parser.h"

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_targetdataC.h"

#include "tao/x11/anytypecode/any.h"

#include <map>
#include <vector>

#ifndef DANCEX11_CONFIG_LOADER_H_
#define DANCEX11_CONFIG_LOADER_H_

namespace DAnCEX11
{
  class DAnCE_Configurator_Export Config_Loader
  {
  public:
    Config_Loader () = default;
    ~Config_Loader () = default;

    using parser_if_type = Config_Parser_T<std::ifstream>;
    using parser_type = parser_if_type::base_type;
    using string_type = parser_type::string_type;

    bool load_deployment_config (const string_type& filename,
                                 Deployment::DeploymentPlan& plugins,
                                 Deployment::DeploymentPlan& components);

    bool load_plugins_config (const string_type& filename,
                              Deployment::DeploymentPlan& plugins);

    bool load_plan_config (const string_type& filename,
                           Deployment::DeploymentPlan& components);

    bool load_domain_config (const string_type& filename,
                             Deployment::Domain& domain);

    struct literal_type
    {
      enum : uint32_t
      {
        lt_char = 0x0001,
        lt_bool = 0x0002,
        lt_int  = 0x0004,
        lt_uint = 0x0008,
        lt_hex  = 0x0010,
        lt_oct  = 0x0020,
        lt_dec  = 0x0040,
        lt_str  = 0x0080,
        lt_seq  = 0x1000
      };

      uint32_t flags_ {};

      bool unknown () const { return !this->flags_; }

      bool is_char () const { return this->flags_ & lt_char; }
      bool is_bool () const { return this->flags_ & lt_bool; }
      bool is_int () const { return this->flags_ & lt_int; }
      bool is_uint () const { return this->flags_ & lt_uint; }
      bool is_hex () const { return this->flags_ & lt_hex; }
      bool is_oct () const { return this->flags_ & lt_oct; }
      bool is_dec () const { return this->flags_ & lt_dec; }
      bool is_str () const { return this->flags_ & lt_str; }
      bool is_seq () const { return this->flags_ & lt_seq; }

      uint32_t base () const { return this->flags_ & 0x00ff; }

      literal_type () = default;
    };

    bool parse_property_definition (const string_type&, Deployment::Property&);

  private:
    using DeploymentRefMap = std::map<string_type, uint32_t>;
    using LocalityInstanceMap = std::map<std::string, std::vector<uint32_t>>;
    using NodeLocalityMap = std::map<std::string, LocalityInstanceMap>;
    using EndpointIndex = std::pair<uint32_t, uint32_t>;
    using InstanceEndpointMap = std::map<std::string, std::vector<EndpointIndex>>;

    struct state_type
    {
      Deployment::DeploymentPlan& plugins_;
      Deployment::DeploymentPlan& components_;

      DeploymentRefMap plugin_instances_;
      DeploymentRefMap component_implementations_;
      DeploymentRefMap component_instances_;
      DeploymentRefMap connection_instances_;
      DeploymentRefMap lm_instances_;
      NodeLocalityMap node_map_;
      InstanceEndpointMap instance_ep_map_;

      state_type (Deployment::DeploymentPlan& plugins,
                  Deployment::DeploymentPlan& components)
        : plugins_ (plugins), components_ (components)
      {}
      state_type (Deployment::DeploymentPlan& plan)
        : plugins_ (plan), components_ (plan)
      {}
    };

    bool load_plugin_config (parser_type&, state_type&);
    bool load_component_config (parser_type&, state_type&);
    bool load_locality_config (parser_type&, state_type&);
    bool load_container_config (parser_type&, state_type&);

    bool load_node_config (parser_type&, Deployment::Domain&);

    bool finalize_component_plan (state_type&);

    bool load_artifact_definition (parser_type&, state_type&);
    bool load_property_definition (parser_type&, Deployment::Property&);
    bool load_connection_definitions (parser_type&, const string_type&, state_type&);

    bool load_property_value(parser_type&, parser_type::type_id, CORBA::Any&);

    literal_type parse_literal_type (parser_type&);

    void check_type (parser_type::type_id&, literal_type);

    bool load_property_sequence (parser_type&, parser_type::type_id, literal_type, CORBA::Any&);
  };
}

#endif /* DANCEX11_CONFIG_LOADER_H_ */
