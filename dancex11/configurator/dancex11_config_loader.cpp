/**
 * @file    dancex11_config_loader.cpp
 * @author  Martin Corino
 *
 * @brief   Loader for DAnCEX11 Deployment config files.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_config_loader.h"
#include "dancex11_config_util.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/logger/log.h"

#include "ace/DLL.h"

#include <cctype>
#include <sstream>

namespace DAnCEX11
{
  bool
  Config_Loader::load_deployment_config (
      const string_type& filename,
      Deployment::DeploymentPlan& plugins,
      Deployment::DeploymentPlan& components)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_deployment_config - "
                        "loading deployment config [" <<
                        filename << "]");

    state_type state(plugins, components);

    parser_if_type parser_(filename, std::ios_base::in);

    if (parser_.bad () || parser_.fail ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_deployment_config - "
                          "failed opening config [" <<
                          filename << "]");
      return false;
    }

    parser_.parse_next ();
    while (parser_)
    {
      uint32_t ln = parser_.line ();
      switch (parser_.current_token_id ())
      {
        case parser_type::IInstanceHandler:
        case parser_type::IInterceptor:
        case parser_type::IConfigHandler:
        case parser_type::IServiceObject:
          {
            string_type plugin_type = parser_.current_token ();
            if (!this->load_plugin_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_deployment_config - "
                                  "Error while loading plugin config for [" <<
                                  plugin_type << "] at line#" << ln);
              return false;
            }
          }
          break;
        case parser_type::IComponent:
          {
            string_type comp_type = parser_.current_token ();
            if (!this->load_component_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_deployment_config - "
                                  "Error while loading component config for [" <<
                                  comp_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::ILocalityManager:
          {
            string_type lm_type = parser_.current_token ();
            if (!this->load_locality_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_deployment_config - "
                                  "Error while loading locality config for [" <<
                                  lm_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::IContainer:
          {
            string_type lm_type = parser_.current_token ();
            if (!this->load_container_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_deployment_config - "
                                  "Error while loading container config for [" <<
                                  lm_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::IProperty:
          {
            Deployment::Property prop;
            string_type prop_type = parser_.current_token ();
            if (!this->load_property_definition (parser_, prop))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_deployment_config - "
                                  "Error while loading property [" <<
                                  prop_type << "] at line #" << ln);
              return false;
            }
            plugins.infoProperty ().push_back (prop);
            components.infoProperty ().push_back (std::move (prop));
          }
          break;

        default:
          {
            DANCEX11_LOG_PANIC ("Config_Loader::load_deployment_config - "
                                "Unexpected token found [" <<
                                parser_.current_token () << "] at #" <<
                                ln);
            return false;
          }
      }
    }

    if (parser_.bad ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_deployment_config - "
                          "Error while reading deployment config [" <<
                          filename << "]");
    }
    else
    {
      if (!this->finalize_component_plan (state))
        return false;

      // set plan names based on file basename if not set
      if (plugins.UUID ().empty ())
      {
        plugins.UUID (cfg::basename (filename, true) + ".plugins");
      }
      if (plugins.label ().empty ())
      {
        plugins.label ("DeploymentPlan loaded from "+filename);
      }
      if (components.UUID ().empty ())
      {
        components.UUID (cfg::basename (filename, true) + ".components");
      }
      if (components.label ().empty ())
      {
        components.label ("DeploymentPlan loaded from "+filename);
      }
    }

    return !parser_.bad ();
  }

  bool
  Config_Loader::load_plugins_config (
      const string_type& filename,
      Deployment::DeploymentPlan& plugins)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_plugins_config - "
                        "loading plugins config [" <<
                        filename << "]");

    state_type state(plugins);

    parser_if_type parser_(filename, std::ios_base::in);

    if (parser_.bad () || parser_.fail ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_plugins_config - "
                          "failed opening config [" <<
                          filename << "]");
      return false;
    }

    parser_.parse_next ();
    while (parser_)
    {
      uint32_t ln = parser_.line ();
      switch (parser_.current_token_id ())
      {
        case parser_type::IInstanceHandler:
        case parser_type::IInterceptor:
        case parser_type::IConfigHandler:
        case parser_type::IServiceObject:
          {
            string_type plugin_type = parser_.current_token ();
            if (!this->load_plugin_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_plugins_config - "
                                  "Error while loading plugin config for [" <<
                                  plugin_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::IProperty:
          {
            Deployment::Property prop;
            string_type prop_type = parser_.current_token ();
            if (!this->load_property_definition (parser_, prop))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                  "Error while loading property [" <<
                                  prop_type << "] at line #" << ln);
              return false;
            }
            plugins.infoProperty ().push_back (prop);
          }
          break;

        default:
          {
            DANCEX11_LOG_PANIC ("Config_Loader::load_plugins_config - "
                                "Unexpected token found [" <<
                                parser_.current_token () << "] at #" <<
                                ln);
            return false;
          }
      }
    }

    if (parser_.bad ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_plugins_config - "
                          "Error while reading plugins config [" <<
                          filename << "]");
    }
    else
    {
      // set plan names based on file basename if not set
      if (plugins.UUID ().empty ())
      {
        plugins.UUID (cfg::basename (filename, true) + ".plugins");
      }
      if (plugins.label ().empty ())
      {
        plugins.label ("DeploymentPlan loaded from "+filename);
      }
    }

    return !parser_.bad ();
  }

  bool
  Config_Loader::load_plan_config (
      const string_type& filename,
      Deployment::DeploymentPlan& components)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_plan_config - "
                        "loading deployment config [" <<
                        filename << "]");

    state_type state (components);

    parser_if_type parser_(filename, std::ios_base::in);

    if (parser_.bad () || parser_.fail ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_plan_config - "
                          "failed opening config [" <<
                          filename << "]");
      return false;
    }

    parser_.parse_next ();
    while (parser_)
    {
      uint32_t ln = parser_.line ();
      switch (parser_.current_token_id ())
      {
        case parser_type::IComponent:
          {
            string_type comp_type = parser_.current_token ();
            if (!this->load_component_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                  "Error while loading component config for [" <<
                                  comp_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::ILocalityManager:
          {
            string_type lm_type = parser_.current_token ();
            if (!this->load_locality_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                  "Error while loading locality config for [" <<
                                  lm_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::IContainer:
          {
            string_type lm_type = parser_.current_token ();
            if (!this->load_container_config (parser_, state))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                  "Error while loading container config for [" <<
                                  lm_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        case parser_type::IProperty:
          {
            Deployment::Property prop;
            string_type prop_type = parser_.current_token ();
            if (!this->load_property_definition (parser_, prop))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                  "Error while loading property [" <<
                                  prop_type << "] at line #" << ln);
              return false;
            }
            components.infoProperty ().push_back (prop);
          }
          break;

        default:
          {
            DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                "Unexpected token found [" <<
                                parser_.current_token () << "] at #" <<
                                ln);
            return false;
          }
      }
    }

    if (parser_.bad ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_plan_config - "
                          "Error while reading plan config [" <<
                          filename << "]");
    }
    else
    {
      if (!this->finalize_component_plan (state))
        return false;

      if (components.UUID ().empty ())
      {
        components.UUID (cfg::basename (filename, true));
      }
      if (components.label ().empty ())
      {
        components.label ("DeploymentPlan loaded from "+filename);
      }
    }

    return !parser_.bad ();
  }

  bool
  Config_Loader::finalize_component_plan (
      state_type& state_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::process_localities");

    // resolve instance references for created internal endpoints
    for (InstanceEndpointMap::value_type& entry : state_.instance_ep_map_)
    {
      // resolve instance reference
      DeploymentRefMap::iterator cinst_it = state_.component_instances_.find (entry.first);
      if (cinst_it == state_.component_instances_.end ())
      {
        // invalid instance id
        DANCEX11_LOG_ERROR ("Config_Loader::finalize_component_plan - "
                            "Invalid instance name defined for connection: "
                            "[" << entry.first << "]");
        return false;
      }

      uint32_t instRef = cinst_it->second;
      // update endpoints
      for (EndpointIndex& epix : entry.second)
      {
        state_.components_.connection ()[epix.first].internalEndpoint ()[epix.second].instanceRef (instRef);
      }
    }

    // see if we need to set up locality restraints for any nodes
    for (const NodeLocalityMap::value_type& entry: state_.node_map_)
    {
      // analyse the localities defined for this node
      const LocalityInstanceMap &locality_map = entry.second;

      // create one DifferentProcess locality constraint for the first instances in each locality
      Deployment::PlanLocality locdiff_constraint(
          Deployment::PlanLocalityKind::PlanDifferentProcess,
          {});
      for (const LocalityInstanceMap::value_type& locentry : locality_map)
      {
        locdiff_constraint.constrainedInstanceRef ().push_back (locentry.second.front ());
      }
      state_.components_.localityConstraint ().push_back (std::move (locdiff_constraint));
      // create SameProcess locality constraints for all instances in each locality
      for (const LocalityInstanceMap::value_type& locentry : locality_map)
      {
        Deployment::PlanLocality loc_constraint(
            Deployment::PlanLocalityKind::PlanSameProcess,
            std::move (locentry.second));
        state_.components_.localityConstraint ().push_back (std::move (loc_constraint));
      }
    }

    return true;
  }

  bool
  Config_Loader::load_plugin_config (
      parser_type& parser_,
      state_type& state_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_plugin_config - "
                        "loading plugin config for [" <<
                        parser_.current_token () << "]");

    string_type plugin_type = parser_.current_token ();

    Deployment::MonolithicDeploymentDescription mdd;
    Deployment::InstanceDeploymentDescription idd;

    uint32_t const ln = parser_.line ();

    // get artifact and factory (and optionally openmode)
    string_type factory;
    uint32_t open_mode = ACE_DEFAULT_SHLIB_MODE;
    if (parser_.parse_next() == parser_type::IText)
    {
      // set artifact id as name for implementation record
      mdd.name (parser_.current_token ());
      if (parser_.parse_next() == parser_type::IText)
      {
        factory = parser_.current_token ();

        // set <artifact>.<factory> as instance id/name
        idd.name (mdd.name () + "." + factory);

        // check for duplicate plugin instance
        if (state_.plugin_instances_.find (idd.name ())
              != state_.plugin_instances_.end ())
        {
          DANCEX11_LOG_ERROR ("Config_Loader::load_plugin_config - "
                              "duplicate plugin instance defined: "
                              "artifact [" << mdd.name () << "], "
                              "factory [" << factory << "]: "
                              "at line#" << ln);
          return false;
        }

        DANCEX11_LOG_TRACE ("Config_Loader::load_plugin_config - "
                            "artifact [" << mdd.name () << "], "
                            "factory [" << factory << "]");

        // look for plugin openmode
        if (parser_.parse_next () == parser_type::IText)
        {
          literal_type lt = this->parse_literal_type (parser_);
          if (!(lt.is_int () || lt.is_hex ()))
          {
            // invalid openmode
            DANCEX11_LOG_ERROR ("Config_Loader::load_plugin_config - "
                                "Invalid plugin openmode defined for "
                                "artifact [" << mdd.name () << "], "
                                "factory [" << factory << "]: "
                                "found [" << parser_.current_token () << "]");
            return false;
          }

          // parse openmode
          open_mode = cfg::parse_int<int32_t> (parser_.current_token (), lt);
          // proceed to next token

          parser_.parse_next ();
        }

        // parse any plugin properties
        while (parser_.at_object_property ())
        {
          // load property definition
          Deployment::Property prop;
          parser_type::token_id prop_type = parser_.current_token_id ();
          if (!this->load_property_definition (parser_, prop))
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_plugin_config - "
                                "Error loading property definition for "
                                "artifact [" << mdd.name () << "], "
                                "factory [" << factory << "]");

            return false;
          }

          // add property to appropriate list
          if (prop_type == parser_type::IExecParam)
          {
            mdd.execParameter ().push_back (std::move (prop));
          }
          else
          {
            idd.configProperty ().push_back (std::move (prop));
          }
        }

        // set basic properties for plugin records
        Deployment::Property exec_parameter;
        exec_parameter.name (DAnCEX11::DANCE_PLUGIN_ARTIFACT);
        exec_parameter.value () <<= mdd.name ();
        mdd.execParameter ().push_back (std::move (exec_parameter));

        exec_parameter.name (DAnCEX11::DANCE_PLUGIN_FACTORY);
        exec_parameter.value () <<= factory;
        mdd.execParameter ().push_back (std::move (exec_parameter));

        exec_parameter.name (DAnCEX11::DANCE_PLUGIN_OPENMODE);
        exec_parameter.value () <<= open_mode;
        mdd.execParameter ().push_back (std::move (exec_parameter));

        exec_parameter.name (DAnCEX11::IMPL_TYPE);
        exec_parameter.value () <<= plugin_type;
        mdd.execParameter ().push_back (std::move (exec_parameter));

        // set implementation reference for plugin instance
        idd.implementationRef (
            ACE_Utils::truncate_cast<uint32_t> (state_.plugins_.implementation ().size ()));

        // add plugin implementation record to plan
        state_.plugins_.implementation ().push_back (std::move (mdd));

        // record plugin instance
        state_.plugin_instances_[idd.name ()] =
            ACE_Utils::truncate_cast<uint32_t> (state_.plugins_.instance ().size ());

        // add plugin instance record to plan
        state_.plugins_.instance ().push_back (std::move (idd));

        DANCEX11_LOG_DEBUG ("Config_Loader::load_plugin_config - "
                            "successfully loaded plugin " << plugin_type <<
                            " artifact [" << mdd.name () << "],"
                            " factory [" << factory << "]");

        return true;
      }
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_plugin_config - "
                        "Error loading plugin config for [" <<
                        plugin_type << "]; artifact="
                        << mdd.name () << ", factory=" << factory);

    return false;
  }


  bool
  Config_Loader::load_domain_config (
      const string_type& filename,
      Deployment::Domain& domain)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_domain_config - "
                        "loading deployment config [" <<
                        filename << "]");

    parser_if_type parser_(filename, std::ios_base::in);
    parser_.parse_next ();
    while (parser_)
    {
      uint32_t ln = parser_.line ();
      switch (parser_.current_token_id ())
      {
        case parser_type::INode:
          {
            string_type node_type = parser_.current_token ();
            if (!this->load_node_config (parser_, domain))
            {
              DANCEX11_LOG_PANIC ("Config_Loader::load_node_config - "
                                  "Error while loading Node config for [" <<
                                  node_type << "] at line#" << ln);
              return false;
            }
          }
          break;

        default:
          {
            DANCEX11_LOG_PANIC ("Config_Loader::load_plan_config - "
                                "Unexpected token found [" <<
                                parser_.current_token () << "] at #" <<
                                ln);
            return false;
          }
      }
    }

    if (parser_.bad ())
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_domain_config - "
                          "Error while reading domain config [" <<
                          filename << "]");
    }
    else
    {
      if (domain.UUID ().empty ())
      {
        domain.UUID (cfg::basename (filename, true));
      }
      if (domain.label ().empty ())
      {
        domain.label ("DeploymentPlan loaded from "+filename);
      }
    }

    return !parser_.bad ();
  }

  bool
  Config_Loader::load_node_config (
      parser_type& parser_,
      Deployment::Domain& domain)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_node_config");

    literal_type lt;
    if (parser_.parse_next () == parser_type::IText &&
        (lt = this->parse_literal_type (parser_)).is_str ())
    {
      std::string node_name =
          cfg::parse_string (parser_.current_token (), lt);

      // create Node description
      Deployment::Node node;
      node.name (node_name);

      DANCEX11_LOG_DEBUG ("Config_Loader::load_node_config - " <<
                          "node [" << node.name () << "]");

      // create address resource
      Deployment::Resource res;
      res.name ("Node Address");
      res.resourceType ().push_back (DAnCEX11::NODE_RESOURCE_TYPE);

      while (parser_.parse_next () == parser_type::IText)
      {
        uint32_t ln = parser_.line ();

        if (parser_.current_token () == DAnCEX11::NODE_IOR)
        {
          // read node address
          if (parser_.parse_next () == parser_type::IText &&
              (lt = this->parse_literal_type (parser_)).is_str ())
          {
            std::string node_address =
                cfg::parse_string (parser_.current_token (), lt);

            DANCEX11_LOG_DEBUG ("Config_Loader::load_node_config - " <<
                                "node [" << node.name () << "],"
                                "IOR [" << node_address << "]");

            // create satisfier property for address
            Deployment::SatisfierProperty prop;

            prop.name (DAnCEX11::NODE_IOR);
            prop.kind (Deployment::SatisfierPropertyKind::Attribute);
            prop.value () <<= node_address;

            // add property
            res.property ().push_back (std::move (prop));
          }
          else
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_node_config - " <<
                                "Invalid " << DAnCEX11::NODE_IOR <<
                                " definition for node " <<
                                node_name  << " at line#" << ln);
            return false;
          }
        }
        else if (parser_.current_token () == DAnCEX11::NODE_PORT)
        {
          // read node port
          if (parser_.parse_next () == parser_type::IText &&
              !(lt = this->parse_literal_type (parser_)).unknown ())
          {
            uint16_t node_port =
                cfg::parse_uint<uint16_t> (parser_.current_token (), lt);

            DANCEX11_LOG_DEBUG ("Config_Loader::load_node_config - " <<
                                "node [" << node.name () << "],"
                                "port [" << node_port << "]");

            // create satisfier property for address
            Deployment::SatisfierProperty prop;

            prop.name (DAnCEX11::NODE_PORT);
            prop.kind (Deployment::SatisfierPropertyKind::Attribute);
            prop.value () <<= node_port;

            // add property
            res.property ().push_back (std::move (prop));
          }
          else
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_node_config - " <<
                                "Invalid " << DAnCEX11::NODE_PORT <<
                                " definition for node " <<
                                node_name  << " at line#" << ln);
            return false;
          }
        }
        else if (parser_.current_token () == DAnCEX11::NODE_HOST)
        {
          // read node port
          if (parser_.parse_next () == parser_type::IText &&
              (lt = this->parse_literal_type (parser_)).is_str ())
          {
            std::string node_host =
                cfg::parse_string (parser_.current_token (), lt);

            DANCEX11_LOG_DEBUG ("Config_Loader::load_node_config - " <<
                                "node [" << node.name () << "],"
                                "host [" << node_host << "]");

            // create satisfier property for address
            Deployment::SatisfierProperty prop;

            prop.name (DAnCEX11::NODE_HOST);
            prop.kind (Deployment::SatisfierPropertyKind::Attribute);
            prop.value () <<= node_host;

            // add property
            res.property ().push_back (std::move (prop));
          }
          else
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_node_config - " <<
                                "Invalid " << DAnCEX11::NODE_HOST <<
                                " definition for node " <<
                                node_name  << " at line#" << ln);
            return false;
          }
        }
        else
        {
          DANCEX11_LOG_ERROR ("Config_Loader::load_node_config - " <<
                              "Invalid property "
                              "definition for node " <<
                              node_name  << " at line#" << ln);
          return false;
        }
      }

      // add node resource
      node.resource ().push_back (std::move (res));

      // add node
      domain.node ().push_back (std::move (node));

      return true;
    }
    else
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_node_config - " <<
                          "Invalid node definition at line#" <<
                          parser_.line ());
    }

    return false;
  }

  bool
  Config_Loader::load_component_config (
      parser_type& parser_,
      state_type& state_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_component_config - " <<
                        "loading component config");


    string_type component_type = parser_.current_token ();

    bool new_mdd (true);
    Deployment::MonolithicDeploymentDescription mdd_new;
    Deployment::MonolithicDeploymentDescription& mdd (mdd_new);
    Deployment::InstanceDeploymentDescription idd;

    uint32_t ln = parser_.line ();

    // get instance, artifact and factory (and optionally openmode)
    string_type artifact;
    string_type factory;
    uint32_t open_mode = ACE_DEFAULT_SHLIB_MODE;
    if (parser_.parse_next() == parser_type::IText)
    {
      // set instance id
      idd.name (parser_.current_token ());

      // check for duplicate instance
      if (state_.component_instances_.find (idd.name ())
              != state_.component_instances_.end ())
      {
        // invalid openmode
        DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                            "duplicate component instance defined "
                            "instance [" << idd.name () << "]"
                            "at line#" << ln);
        return false;
      }

      if (parser_.parse_next() == parser_type::IText)
      {
        // get artifact id
        artifact = parser_.current_token ();
        if (parser_.parse_next() == parser_type::IText)
        {
          // get factory
          factory = parser_.current_token ();

          // set <artifact>.<factory> as implementation id/name
          mdd.name (artifact + "." + factory);

          // check for existing implementation
          if (state_.component_implementations_.find (mdd.name ()) !=
                  state_.component_implementations_.end ())
          {
            mdd = state_.components_.implementation ()[
                                 state_.component_implementations_[mdd.name ()]];
            new_mdd = false;
          }

          DANCEX11_LOG_TRACE ("Config_Loader::load_component_config - " <<
                              "instance [" << idd.name () << "],"
                              "artifact [" << artifact << "], "
                              "factory [" << factory << "]" <<
                              (new_mdd ? "" : " (reusing existing definition)"));

          // look for component openmode
          if (parser_.parse_next () == parser_type::IText)
          {
            literal_type lt = this->parse_literal_type (parser_);
            if (!(lt.is_int () || lt.is_hex ()))
            {
              // invalid openmode
              DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                                  "Invalid plugin openmode defined for "
                                  "instance [" << idd.name () << "],"
                                  "artifact [" << artifact << "], "
                                  "factory [" << factory << "]: "
                                  "found [" << parser_.current_token () << "]");
              return false;
            }

            // parse openmode
            open_mode = cfg::parse_int<int32_t> (parser_.current_token (), lt);
            // proceed to next token

            parser_.parse_next ();
          }

          std::string locality;

          // parse any component properties or connections
          while (parser_.at_object_property () ||
                 parser_.at_component_node () ||
                 parser_.at_component_locality () ||
                 parser_.at_component_container_id () ||
                 parser_.at_component_artifact () ||
                 parser_.at_component_connection ())
          {
            if (parser_.at_object_property ())
            {
              // load property definition
              Deployment::Property prop;
              parser_type::token_id prop_type = parser_.current_token_id ();
              if (!this->load_property_definition (parser_, prop))
              {
                DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                                    "Error loading property definition for "
                                    "instance [" << idd.name () << "],"
                                    "artifact [" << artifact << "], "
                                    "factory [" << factory << "]");

                return false;
              }

              // add property to appropriate list
              if (prop_type == parser_type::IExecParam)
              {
                mdd.execParameter ().push_back (std::move (prop));
              }
              else
              {
                idd.configProperty ().push_back (std::move (prop));
              }
            }
            else if (parser_.at_component_node ())
            {
              if (parser_.parse_next () == parser_type::IText)
              {
                literal_type lt = this->parse_literal_type (parser_);
                if (!lt.is_str ())
                {
                  // invalid openmode
                  DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                                      "Invalid deployment node defined for "
                                      "instance [" << idd.name () << "],"
                                      "artifact [" << artifact << "], "
                                      "factory [" << factory << "]: "
                                      "found [" << parser_.current_token () << "]");
                  return false;
                }

                // set instance node
                idd.node (cfg::parse_string (parser_.current_token (), lt));

                // proceed to next token
                parser_.parse_next ();
              }
            }
            else if (parser_.at_component_locality ())
            {
              if (parser_.parse_next () == parser_type::IText)
              {
                literal_type lt = this->parse_literal_type (parser_);
                if (!lt.is_str ())
                {
                  // invalid openmode
                  DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                                      "Invalid deployment locality defined for "
                                      "instance [" << idd.name () << "],"
                                      "artifact [" << artifact << "], "
                                      "factory [" << factory << "]: "
                                      "found [" << parser_.current_token () << "]");
                  return false;
                }

                // get instance locality
                locality = cfg::parse_string (parser_.current_token (), lt);
                // set instance locality property
                Deployment::Property config_prop;
                config_prop.name (DAnCEX11::COMPONENT_LOCALITY);
                config_prop.value () <<= locality;
                idd.configProperty ().push_back (std::move (config_prop));

                // proceed to next token
                parser_.parse_next ();
              }
            }
            else if (parser_.at_component_container_id ())
            {
              if (parser_.parse_next () == parser_type::IText)
              {
                literal_type lt = this->parse_literal_type (parser_);
                if (!lt.is_str ())
                {
                  // invalid container id
                  DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                                      "Invalid container ID defined for "
                                      "instance [" << idd.name () << "],"
                                      "artifact [" << artifact << "], "
                                      "factory [" << factory << "]: "
                                      "found [" << parser_.current_token () << "]");
                  return false;
                }

                // get container instance id
                std::string container = cfg::parse_string (parser_.current_token (), lt);
                // set instance container id property
                Deployment::Property config_prop;
                config_prop.name (DAnCEX11::COMPONENT_CONTAINER);
                config_prop.value () <<= container;
                idd.configProperty ().push_back (std::move (config_prop));

                // proceed to next token
                parser_.parse_next ();
              }
            }
            else if (parser_.at_component_artifact ())
            {
              uint32_t line = parser_.line ();
              if (!this->load_artifact_definition (parser_, state_))
              {
                // invalid artifact decl
                DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                                    "Invalid deployment artifact defined for "
                                    "instance [" << idd.name () <<
                                    "] at #" << line);
                return false;
              }

              // register last added artifact reference with implementation
              mdd.artifactRef ().push_back (
                  ACE_Utils::truncate_cast<uint32_t> (state_.components_.artifact ().size ())-1);
            }
            else
            {
              // load connection definitions
              if (!this->load_connection_definitions (parser_, idd.name (), state_))
              {
                DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                                    "Error loading connection definitions for "
                                    "instance [" << idd.name () << "],"
                                    "artifact [" << artifact << "], "
                                    "factory [" << factory << "]");

                return false;
              }
            }
          }

          if (new_mdd)
          {
            // set basic properties for component records
            Deployment::Property exec_parameter;
            exec_parameter.name (DAnCEX11::COMPONENT_ARTIFACT);
            exec_parameter.value () <<= artifact;
            mdd.execParameter ().push_back (std::move (exec_parameter));

            exec_parameter.name (DAnCEX11::COMPONENT_FACTORY);
            exec_parameter.value () <<= factory;
            mdd.execParameter ().push_back (std::move (exec_parameter));

            exec_parameter.name (DAnCEX11::DANCE_PLUGIN_OPENMODE);
            exec_parameter.value () <<= open_mode;
            mdd.execParameter ().push_back (std::move (exec_parameter));

            exec_parameter.name (DAnCEX11::IMPL_TYPE);
            exec_parameter.value () <<= component_type;
            mdd.execParameter ().push_back (std::move (exec_parameter));

            // set implementation reference for component instance
            idd.implementationRef (
                ACE_Utils::truncate_cast<uint32_t> (state_.components_.implementation ().size ()));
            // record position for additional component implementation record
            state_.component_implementations_[mdd.name ()] =
                idd.implementationRef ();
            // add component implementation record to plan
            state_.components_.implementation ().push_back (std::move (mdd));

            // update node reference for all exclusive artifacts
          }
          else
          {
            // set existing implementation reference for component instance
            idd.implementationRef (
                state_.component_implementations_[mdd.name ()]);
          }

          // update node reference for all exclusive artifacts
          for (uint32_t artRef : mdd.artifactRef ())
          {
            ::Deployment::ArtifactDeploymentDescription& add =
                state_.components_.artifact ()[artRef];
            // marked exclusive?
            if (add.node () == "_+_")
            {
              // copy comp instance node to artifact
              add.node (idd.node ());
            }
          }

          // record position for additional component instance record
          state_.component_instances_[idd.name ()] =
              ACE_Utils::truncate_cast<uint32_t> (state_.components_.instance ().size ());
          // if a locality has been defined add instance to node map
          // for later locality constraint analysis
          if (!locality.empty ())
          {
            state_.node_map_[idd.node ()][locality].push_back (
                ACE_Utils::truncate_cast<uint32_t> (state_.components_.instance ().size ()));
          }

          // add component instance record to plan
          state_.components_.instance ().push_back (std::move (idd));

          DANCEX11_LOG_DEBUG ("Config_Loader::load_component_config - " <<
                              "successfully loaded component " << component_type <<
                              " instance [" <<
                              state_.components_.instance ().back ().name () << "],"
                              " artifact [" << artifact << "],"
                              " factory [" << factory << "]");

          return true;
        }
      }
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_component_config - " <<
                        "Error loading component " << component_type <<
                        " instance [" << idd.name () << "],"
                        " artifact [" << artifact << "],"
                        " factory [" << factory << "]"
                        " at #" << parser_.line ());

    return false;
  }

  bool
  Config_Loader::load_locality_config (
      parser_type& parser_,
      state_type& state_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_locality_config - " <<
                        "loading locality manager config");


    string_type locality_type = parser_.current_token ();
    uint32_t lm_line = parser_.line ();

    bool new_mdd (true);
    Deployment::MonolithicDeploymentDescription mdd_new;
    Deployment::MonolithicDeploymentDescription& mdd (mdd_new);
    Deployment::InstanceDeploymentDescription idd;

    // get instance id
    if (parser_.parse_next() == parser_type::IText)
    {
      // set instance id
      idd.name (parser_.current_token ());
      // set standard implementation id/name
      mdd.name ("_AXCIOMA_LocalityManagerSvc_");

      // check for existing implementation
      if (state_.component_implementations_.find (mdd.name ()) !=
              state_.component_implementations_.end ())
      {
        mdd = state_.components_.implementation ()[
                             state_.component_implementations_[mdd.name ()]];
        new_mdd = false;
      }

      DANCEX11_LOG_TRACE ("Config_Loader::load_locality_config - " <<
                          "instance [" << idd.name () << "]," <<
                          (new_mdd ? "" : " (reusing existing definition)"));

      // proceed to next token
      parser_.parse_next ();

      // by default instance id == locality id
      std::string locality = idd.name ();

      // parse any component properties or connections
      while (parser_.at_object_property () ||
             parser_.at_component_node () ||
             parser_.at_component_locality () ||
             parser_.at_component_artifact ())
      {
        if (parser_.at_object_property ())
        {
          // load property definition
          Deployment::Property prop;
          parser_type::token_id prop_type = parser_.current_token_id ();
          if (!this->load_property_definition (parser_, prop))
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                                "Error loading property definition for "
                                "instance [" << idd.name () << "]");

            return false;
          }

          // add property to appropriate list
          if (prop_type == parser_type::IExecParam)
          {
            mdd.execParameter ().push_back (std::move (prop));
          }
          else
          {
            idd.configProperty ().push_back (std::move (prop));
          }
        }
        else if (parser_.at_component_artifact ())
        {
          uint32_t line = parser_.line ();
          if (!this->load_artifact_definition (parser_, state_))
          {
            // invalid artifact decl
            DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                                "Invalid deployment artifact defined for "
                                "instance [" << idd.name () <<
                                "] at #" << line);
            return false;
          }

          // register last added artifact reference with implementation
          mdd.artifactRef ().push_back (
              ACE_Utils::truncate_cast<uint32_t> (state_.components_.artifact ().size ())-1);
        }
        else if (parser_.at_component_locality ())
        {
          if (parser_.parse_next () == parser_type::IText)
          {
            literal_type lt = this->parse_literal_type (parser_);
            if (!lt.is_str ())
            {
              // invalid openmode
              DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                                  "Invalid deployment locality defined for "
                                  "instance [" << idd.name () << "]:"
                                  "found [" << parser_.current_token () << "]");
              return false;
            }

            // get instance locality
            locality = cfg::parse_string (parser_.current_token (), lt);
            // set instance locality property
            Deployment::Property config_prop;
            config_prop.name (DAnCEX11::COMPONENT_LOCALITY);
            config_prop.value () <<= locality;
            idd.configProperty ().push_back (std::move (config_prop));

            // proceed to next token
            parser_.parse_next ();
          }
        }
        else
        {
          if (parser_.parse_next () == parser_type::IText)
          {
            literal_type lt = this->parse_literal_type (parser_);
            if (!lt.is_str ())
            {
              // invalid openmode
              DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                                  "Invalid deployment node defined for "
                                  "instance [" << idd.name () << "]:"
                                  "found [" << parser_.current_token () << "]");
              return false;
            }

            // set instance node
            idd.node (cfg::parse_string (parser_.current_token (), lt));

            // proceed to next token
            parser_.parse_next ();
          }
        }
      }

      // check for duplicate locality manager declaration
      if (state_.lm_instances_.find (locality) !=
                                state_.lm_instances_.end ())
      {
        // duplicate LM instance for locality
        DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                            "Duplicate " << locality_type <<
                            " instance [" << locality << "]"
                            " at #" << lm_line);
        return false;
      }

      if (new_mdd)
      {
        // set implementation type property for locality manager records
        Deployment::Property exec_parameter;
        exec_parameter.name (DAnCEX11::IMPL_TYPE);
        exec_parameter.value () <<= locality_type;
        mdd.execParameter ().push_back (std::move (exec_parameter));

        // set implementation reference for component instance
        idd.implementationRef (
            ACE_Utils::truncate_cast<uint32_t> (state_.components_.implementation ().size ()));
        // record position for additional component implementation record
        state_.component_implementations_[mdd.name ()] =
            idd.implementationRef ();
        // add component implementation record to plan
        state_.components_.implementation ().push_back (std::move (mdd));
      }
      else
      {
        // set existing implementation reference for component instance
        idd.implementationRef (
            state_.component_implementations_[mdd.name ()]);
      }

      // update node reference for all exclusive artifacts
      for (uint32_t artRef : mdd.artifactRef ())
      {
        ::Deployment::ArtifactDeploymentDescription& add =
            state_.components_.artifact ()[artRef];
        // marked exclusive?
        if (add.node () == "_+_")
        {
          // copy comp instance node to artifact
          add.node (idd.node ());
        }
      }

      // add instance to node map
      // for later locality constraint analysis
      state_.node_map_[idd.node ()][locality].push_back (
          ACE_Utils::truncate_cast<uint32_t> (state_.components_.instance ().size ()));

      // register LM instance for locality
      state_.lm_instances_[locality] =
          ACE_Utils::truncate_cast<uint32_t> (state_.components_.instance ().size ());

      // add component instance record to plan
      state_.components_.instance ().push_back (std::move (idd));

      DANCEX11_LOG_DEBUG ("Config_Loader::load_locality_config - " <<
                          "successfully loaded component " << locality_type <<
                          " instance [" <<
                          state_.components_.instance ().back ().name () << "]");

      return true;
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_locality_config - " <<
                        "Error loading " << locality_type <<
                        " instance [" << idd.name () << "]"
                        " at #" << parser_.line ());

    return false;
  }

  bool
  Config_Loader::load_container_config (
      parser_type& parser_,
      state_type& state_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_container_config - " <<
                        "loading container config");


    string_type container_type = parser_.current_token ();
    uint32_t cont_line = parser_.line ();

    bool new_mdd (true);
    Deployment::MonolithicDeploymentDescription mdd_new;
    Deployment::MonolithicDeploymentDescription& mdd (mdd_new);
    Deployment::InstanceDeploymentDescription idd;

    // get instance id
    if (parser_.parse_next() == parser_type::IText)
    {
      // set instance id
      idd.name (parser_.current_token ());

      // check for duplicate instance
      if (state_.component_instances_.find (idd.name ())
              != state_.component_instances_.end ())
      {
        // invalid openmode
        DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                            "duplicate container/component instance defined "
                            "instance [" << idd.name () << "]"
                            " at line#" << cont_line);
        return false;
      }

      // set standard implementation id/name
      mdd.name ("_AXCIOMA_CCMContainer_");

      // check for existing implementation
      if (state_.component_implementations_.find (mdd.name ()) !=
              state_.component_implementations_.end ())
      {
        mdd = state_.components_.implementation ()[
                             state_.component_implementations_[mdd.name ()]];
        new_mdd = false;
      }

      DANCEX11_LOG_TRACE ("Config_Loader::load_container_config - " <<
                          "instance [" << idd.name () << "]," <<
                          (new_mdd ? "" : " (reusing existing definition)"));

      // proceed to next token
      parser_.parse_next ();

      // by default instance id == container id
      std::string locality;

      // parse any component/object properties
      while (parser_.at_object_property () ||
             parser_.at_component_node () ||
             parser_.at_component_locality () ||
             parser_.at_component_container_id () ||
             parser_.at_component_artifact ())
      {
        if (parser_.at_object_property ())
        {
          // load property definition
          Deployment::Property prop;
          parser_type::token_id prop_type = parser_.current_token_id ();
          if (!this->load_property_definition (parser_, prop))
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                                "Error loading property definition for "
                                "instance [" << idd.name () << "]");

            return false;
          }

          // add property to appropriate list
          if (prop_type == parser_type::IExecParam)
          {
            mdd.execParameter ().push_back (std::move (prop));
          }
          else
          {
            idd.configProperty ().push_back (std::move (prop));
          }
        }
        else if (parser_.at_component_artifact ())
        {
          uint32_t line = parser_.line ();
          if (!this->load_artifact_definition (parser_, state_))
          {
            // invalid artifact decl
            DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                                "Invalid deployment artifact defined for "
                                "instance [" << idd.name () <<
                                "] at #" << line);
            return false;
          }

          // register last added artifact reference with implementation
          mdd.artifactRef ().push_back (
              ACE_Utils::truncate_cast<uint32_t> (state_.components_.artifact ().size ())-1);
        }
        else if (parser_.at_component_locality ())
        {
          if (parser_.parse_next () == parser_type::IText)
          {
            literal_type lt = this->parse_literal_type (parser_);
            if (!lt.is_str ())
            {
              // invalid openmode
              DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                                  "Invalid deployment locality defined for "
                                  "instance [" << idd.name () << "]:"
                                  "found [" << parser_.current_token () << "]");
              return false;
            }

            // get instance locality
            locality = cfg::parse_string (parser_.current_token (), lt);
            // set instance locality property
            Deployment::Property config_prop;
            config_prop.name (DAnCEX11::COMPONENT_LOCALITY);
            config_prop.value () <<= locality;
            idd.configProperty ().push_back (std::move (config_prop));

            // proceed to next token
            parser_.parse_next ();
          }
        }
        else if (parser_.at_component_container_id ())
        {
          if (parser_.parse_next () == parser_type::IText)
          {
            literal_type lt = this->parse_literal_type (parser_);
            if (!lt.is_str ())
            {
              // invalid container id
              DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                                  "Invalid container ID defined for "
                                  "instance [" << idd.name () << "]:"
                                  "found [" << parser_.current_token () << "]");
              return false;
            }

            // get container instance id
            std::string container = cfg::parse_string (parser_.current_token (), lt);
            // set instance container id property
            Deployment::Property config_prop;
            config_prop.name (DAnCEX11::COMPONENT_CONTAINER);
            config_prop.value () <<= container;
            idd.configProperty ().push_back (std::move (config_prop));

            // proceed to next token
            parser_.parse_next ();
          }
        }
        else
        {
          if (parser_.parse_next () == parser_type::IText)
          {
            literal_type lt = this->parse_literal_type (parser_);
            if (!lt.is_str ())
            {
              // invalid openmode
              DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                                  "Invalid deployment node defined for "
                                  "instance [" << idd.name () << "]:"
                                  "found [" << parser_.current_token () << "]");
              return false;
            }

            // set instance node
            idd.node (cfg::parse_string (parser_.current_token (), lt));

            // proceed to next token
            parser_.parse_next ();
          }
        }
      }

      if (new_mdd)
      {
        // set implementation type property for container records
        Deployment::Property exec_parameter;
        exec_parameter.name (DAnCEX11::IMPL_TYPE);
        exec_parameter.value () <<= container_type;
        mdd.execParameter ().push_back (std::move (exec_parameter));

        // set implementation reference for container component instance
        idd.implementationRef (
            ACE_Utils::truncate_cast<uint32_t> (state_.components_.implementation ().size ()));
        // record position for additional component implementation record
        state_.component_implementations_[mdd.name ()] =
            idd.implementationRef ();
        // add component implementation record to plan
        state_.components_.implementation ().push_back (std::move (mdd));
      }
      else
      {
        // set existing implementation reference for component instance
        idd.implementationRef (
            state_.component_implementations_[mdd.name ()]);
      }

      // update node reference for all exclusive artifacts
      for (uint32_t artRef : mdd.artifactRef ())
      {
        ::Deployment::ArtifactDeploymentDescription& add =
            state_.components_.artifact ()[artRef];
        // marked exclusive?
        if (add.node () == "_+_")
        {
          // copy comp instance node to artifact
          add.node (idd.node ());
        }
      }

      // if a locality has been defined add instance to node map
      // for later locality constraint analysis
      if (!locality.empty ())
      {
        state_.node_map_[idd.node ()][locality].push_back (
            ACE_Utils::truncate_cast<uint32_t> (state_.components_.instance ().size ()));
      }

      // add component instance record to plan
      state_.components_.instance ().push_back (std::move (idd));

      DANCEX11_LOG_DEBUG ("Config_Loader::load_container_config - " <<
                          "successfully loaded component " << container_type <<
                          " instance [" << state_.components_.instance ().back ().name () << "]");

      return true;
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_container_config - " <<
                        "Error loading " << container_type <<
                        " instance [" << idd.name () << "]"
                        " at #" << parser_.line ());

    return false;
  }

  bool
  Config_Loader::load_artifact_definition (
      parser_type& parser_,
      state_type& state_)
  {
    if (parser_.parse_next () == parser_type::IText)
    {
      std::string artifact_name = parser_.current_token ();
      literal_type lt;
      if (parser_.parse_next () == parser_type::IText &&
            (lt = Config_Loader::parse_literal_type (parser_)).is_str ())
      {
        std::string location =
            cfg::parse_string (parser_.current_token (), lt);

        bool exclusive (false);
        if (parser_.parse_next () == parser_type::IText)
        {
          lt = this->parse_literal_type (parser_);
          if (lt.is_bool ())
          {
            exclusive = cfg::parse_bool (parser_.current_token (), lt);

            // proceed to next token
            parser_.parse_next ();
          }
          else
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_artifact_definition - " <<
                                "Invalid exclusiveness definition for artifact " <<
                                artifact_name  << ":" <<
                                "found [" << parser_.current_token () << "]");
            return false;
          }
        }

        // create artifact description
        Deployment::ArtifactDeploymentDescription add;
        add.name (artifact_name);
        if (exclusive)
          add.node ("_+_"); // mark as exclusive to comp.instance node; will replace later
        add.location ().push_back (location);

        // check for artifact properties
        while (parser_.current_token_id () == parser_type::IProperty)
        {
          uint32_t ln = parser_.line ();
          Deployment::Property prop;
          if (!this->load_property_definition (parser_, prop))
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_artifact_definition - "
                                "Invalid property definition for artifact " <<
                                artifact_name  << " at line#" << ln);
            return false;
          }
          // add artifact property
          add.execParameter ().push_back (std::move (prop));
        }

        // add artifact description to plan
        state_.components_.artifact ().push_back (std::move (add));

        return true;
      }
      else
      {
        DANCEX11_LOG_ERROR ("Config_Loader::load_artifact_definition - " <<
                            "Invalid location definition for artifact " <<
                            artifact_name  << ":" <<
                            "found [" << parser_.current_token () << "]");
      }
    }
    else
    {
      DANCEX11_LOG_ERROR ("Config_Loader::load_artifact_definition - " <<
                          "Invalid artifact definition at line#" <<
                          parser_.line ());
    }

    return false;
  }

  bool
  Config_Loader::load_property_definition (
      parser_type& parser_,
      Deployment::Property& prop_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_property_definition - "
                        "loading property definition for [" <<
                        parser_.current_token () << "]");

    string_type prop_type = parser_.current_token ();

    // load property id, type (optional) and value
    parser_type::type_id type = parser_type::TNone;
    if (parser_.parse_next() == parser_type::IText)
    {
      // set property id/name
      prop_.name (parser_.current_token ());
      if (parser_.parse_next() == parser_type::IText)
      {
        if (parser_.break_ch () == ':')
        {
          type = parser_type::token_to_type (parser_.current_token ());
          if (type == parser_type::TNone || type == parser_type::TUser)
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_property_definition - "
                                "Invalid type for property definition for [" <<
                                prop_type << "]; propid="
                                << prop_.name () << ", type=" << type);
            return false;
          }
          parser_.parse_next ();
        }

        if (parser_.current_token_id() == parser_type::IText)
        {
          // load property value into Any
          if (!this->load_property_value (parser_, type, prop_.value ()))
          {
            DANCEX11_LOG_ERROR ("Config_Loader::load_property_definition - "
                                "Unable to load value for property definition for [" <<
                                prop_type << "]; propid="
                                << prop_.name () << ", type=" << type);
            return false;
          }

          return true;
        }
        else
        {
          DANCEX11_LOG_ERROR ("Config_Loader::load_property_definition - "
                              "Missing value for property definition for [" <<
                              prop_type << "]; propid="
                              << prop_.name () << ", type=" << type <<
                              " at #" << parser_.line () <<
                              ": found [" << parser_.current_token () << "]");
          return false;
        }
      }
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_property_definition - "
                        "Error loading property definition for [" <<
                        prop_type << "]; propid="
                        << prop_.name () << ", type=" << type <<
                        " at #" << parser_.line ());

    return false;
  }

  bool
  Config_Loader::parse_property_definition (
      const string_type& propdef_,
      Deployment::Property& prop_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::parse_property_definition - "
                        "parsing property definition");

    using parser_is_type = Config_Parser_T<std::istringstream>;
    parser_is_type parser_ (propdef_);

    // load property id, type (optional) and value
    parser_type::type_id type = parser_type::TNone;
    // support <id>=<value> format
    if (parser_.parse_next(true) == parser_type::IText)
    {
      // set property id/name
      prop_.name (parser_.current_token ());
      if (parser_.parse_next() == parser_type::IText)
      {
        if (parser_.break_ch () == ':')
        {
          type = parser_type::token_to_type (parser_.current_token ());
          if (type == parser_type::TNone || type == parser_type::TUser)
          {
            DANCEX11_LOG_ERROR ("Config_Loader::parse_property_definition - "
                                "Invalid type for property definition; "
                                << "propid="
                                << prop_.name () << ", type=" << type);
            return false;
          }
          parser_.parse_next ();
        }

        if (parser_.current_token_id() == parser_type::IText)
        {
          // load property value into Any
          if (!this->load_property_value (parser_, type, prop_.value ()))
          {
            DANCEX11_LOG_ERROR ("Config_Loader::parse_property_definition - "
                                "Unable to load value for property definition; "
                                << "propid="
                                << prop_.name () << ", type=" << type);
            return false;
          }

          return true;
        }
        else
        {
          DANCEX11_LOG_ERROR ("Config_Loader::parse_property_definition - "
                              "Missing value for property definition; "
                              << "propid="
                              << prop_.name () << ", type=" << type
                              << " : found [" << parser_.current_token () << "]");
          return false;
        }
      }
    }

    DANCEX11_LOG_ERROR ("Config_Loader::parse_property_definition - "
                        "Error loading property definition; "
                        << "propid="
                        << prop_.name () << ", type=" << type);

    return false;
  }

  bool
  Config_Loader::load_connection_definitions (
      parser_type& parser_,
      const string_type& instId_,
      state_type& state_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_connection_definitions - "
                        "loading " << parser_.current_token () <<
                        " definition");

    if (parser_.current_token_id () == parser_type::IConnection)
    {
      // load peer instance id and links/properties
      if (parser_.parse_next () == parser_type::IText)
      {
        // get peer instance id
        string_type peer_instId = parser_.current_token ();

        // proceed to next token
        parser_.parse_next ();

        while (parser_.current_token_id () == parser_type::IText)
        {
          string_type this_port, peer_port;
          string_type::value_type link {};
          // get this component instances port name
          this_port = parser_.current_token ();
          // get the link indicator ('<' or '>')
          link = parser_.break_ch ();
          if (parser_.parse_next () == parser_type::IText)
          {
            // get the peer instances port name
            peer_port = parser_.current_token ();
          }

          if (!(link == '<' || link == '>') || peer_port.empty ())
          {
            // invalid connection definition
            DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definition - "
                                "invalid connection definition for " <<
                                instId_ <<
                                ": peer instance=" << peer_instId <<
                                "; this_port=" << this_port <<
                                "; link=" << link <<
                                "; peer_port=" << peer_port);
            return false;
          }

          Deployment::PlanConnectionDescription conn;
          // set connection id/name
          std::ostringstream cos;
          cos << instId_ << '.' << this_port
              << ':'
              << peer_instId << '.' << peer_port;
          conn.name (cos.str ());

          DANCEX11_LOG_DEBUG ("Config_Loader::load_connection_definition - "
                              "loading connection definition [" <<
                              conn.name () << "] for " <<
                              instId_ <<
                              ": peer instance=" << peer_instId <<
                              "; this_port=" << this_port <<
                              "; link=" << link <<
                              "; peer_port=" << peer_port);


          // check for duplicate connection
          if (state_.connection_instances_.find (conn.name ())
                              != state_.connection_instances_.end ())
          {
            // duplicate connection
            DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definitions - " <<
                                "duplicate connection instance [" <<
                                conn.name () << "] defined for " <<
                                instId_ <<
                                ": peer instance=" << peer_instId <<
                                "; this_port=" << this_port <<
                                "; link=" << link <<
                                "; peer_port=" << peer_port);
            return false;
          }

          // create internal endpoints
          Deployment::PlanSubcomponentPortEndpoint this_ep;
          Deployment::PlanSubcomponentPortEndpoint peer_ep;
          this_ep.portName (this_port);
          peer_ep.portName (peer_port);
          if (link == '<')
          {
            this_ep.provider (false);
            this_ep.kind (Deployment::CCMComponentPortKind::SimplexReceptacle);
            peer_ep.provider (true);
            peer_ep.kind (Deployment::CCMComponentPortKind::Facet);
          }
          else
          {
            this_ep.provider (true);
            this_ep.kind (Deployment::CCMComponentPortKind::Facet);
            peer_ep.provider (false);
            peer_ep.kind (Deployment::CCMComponentPortKind::SimplexReceptacle);
          }

          // check for any connections properties
          if (parser_.parse_next () == parser_type::IProperty)
          {
            Deployment::Requirement conn_req;
            conn_req.name (DAnCEX11::CONNECTION_PROPERTY);
            conn_req.resourceType ("property-list");
            while (parser_.current_token_id () == parser_type::IProperty)
            {
              Deployment::Property prop;
              if (!this->load_property_definition (parser_, prop))
              {
                DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definitions - "
                                    "Error loading connection property definition for "
                                    "instance [" << instId_ << "]");
                return false;
              }
              // add connection property
              conn_req.property ().push_back (std::move (prop));
            }
            // set deployment properties requirement
            conn.deployRequirement ().push_back (std::move (conn_req));
          }

          // update connection with endpoints
          conn.internalEndpoint ().push_back (std::move (this_ep));
          conn.internalEndpoint ().push_back (std::move (peer_ep));

          // record index for additional connection
          uint32_t conn_idx =
              ACE_Utils::truncate_cast<uint32_t> (state_.components_.connection ().size ());
          // register connection instance
          state_.connection_instances_[conn.name ()] = conn_idx;
          // add connection
          state_.components_.connection ().push_back (std::move (conn));

          // record endpoint references for later instance resolution
          state_.instance_ep_map_[instId_].push_back (EndpointIndex (conn_idx, 0));
          state_.instance_ep_map_[peer_instId].push_back (EndpointIndex (conn_idx, 1));
        }

        return true;
      }
    }
    else  // external connection
    {
      // proceed to next token
      parser_.parse_next ();
      // load links/properties
      while (parser_.current_token_id () == parser_type::IText)
      {
        string_type this_port, external_port, external_ref;
        string_type::value_type link {};
        // get this component instances port name
        this_port = parser_.current_token ();
        // get the link indicator ('<' or '>')
        link = parser_.break_ch ();
        if (parser_.parse_next () == parser_type::IText)
        {
          literal_type lt = this->parse_literal_type (parser_);
          if (!lt.is_str ())
          {
            // no string literal so this must be the external port
            // get the external port name
            external_port = parser_.current_token ();

            // next token must be external reference location
            if (parser_.parse_next () != parser_type::IText
                ||
                !(lt = this->parse_literal_type (parser_)).is_str ())
            {
              // invalid reference location
              DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definition - "
                                  "Invalid external reference defined for "
                                  "instance [" << instId_ << "] at #" <<
                                  parser_.line () <<
                                  "found [" << parser_.current_token () << "]");
              return false;
            }
          }

          // get the external reference location
          external_ref = cfg::parse_string (parser_.current_token (), lt);
        }

        if (!(link == '<' || link == '>') || external_ref.empty ())
        {
          // invalid connection definition
          DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definition - "
                              "invalid external connection for " <<
                              instId_ <<
                              "; this_port=" << this_port <<
                              "; link=" << link <<
                              "; external_port=" << external_port <<
                              "; external_ref=" << external_ref);
          return false;
        }

        Deployment::PlanConnectionDescription conn;
        // set connection id/name
        std::ostringstream cos;
        cos << instId_ << '.' << this_port
            << ':'
            << external_port << '@' << external_ref;
        conn.name (cos.str ());

        DANCEX11_LOG_DEBUG ("Config_Loader::load_connection_definition - "
                            "loading connection definition [" <<
                            conn.name () << "] for " <<
                            instId_ <<
                            "; this_port=" << this_port <<
                            "; external_port=" << external_port <<
                            "; external_ref=" << external_ref);

        // check for duplicate connection
        if (state_.connection_instances_.find (conn.name ())
                            != state_.connection_instances_.end ())
        {
          // duplicate connection
          DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definitions - " <<
                              "duplicate connection instance [" <<
                              conn.name () << "] defined for " <<
                              instId_ <<
                              "; this_port=" << this_port <<
                              "; link=" << link <<
                              "; external_port=" << external_port <<
                              "; external_ref=" << external_ref);
          return false;
        }

        // create internal endpoints
        Deployment::PlanSubcomponentPortEndpoint this_ep;
        Deployment::ExternalReferenceEndpoint ext_ep;
        this_ep.portName (this_port);
        ext_ep.location (external_ref);
        ext_ep.portName (external_port);
        if (link == '<')
        {
          this_ep.provider (false);
          this_ep.kind (Deployment::CCMComponentPortKind::SimplexReceptacle);
          ext_ep.provider (true);
        }
        else
        {
          this_ep.provider (true);
          this_ep.kind (Deployment::CCMComponentPortKind::Facet);
          ext_ep.provider (false);
        }

        // check for any connections properties
        if (parser_.parse_next () == parser_type::IProperty)
        {
          Deployment::Requirement conn_req;
          conn_req.name (DAnCEX11::CONNECTION_PROPERTY);
          conn_req.resourceType ("property-list");
          while (parser_.current_token_id () == parser_type::IProperty)
          {
            Deployment::Property prop;
            if (!this->load_property_definition (parser_, prop))
            {
              DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definitions - "
                                  "Error loading connection property definition for "
                                  "instance [" << instId_ << "]");
              return false;
            }
            // add connection property
            conn_req.property ().push_back (std::move (prop));
          }
          // set deployment properties requirement
          conn.deployRequirement ().push_back (std::move (conn_req));
        }

        // update connection with endpoints
        conn.internalEndpoint ().push_back (std::move (this_ep));
        conn.externalReference ().push_back (std::move (ext_ep));

        // record index for additional connection
        uint32_t conn_idx =
            ACE_Utils::truncate_cast<uint32_t> (state_.components_.connection ().size ());
        // register connection instance
        state_.connection_instances_[conn.name ()] = conn_idx;
        // add connection
        state_.components_.connection ().push_back (std::move (conn));

        // record endpoint references for later instance resolution
        state_.instance_ep_map_[instId_].push_back (EndpointIndex (conn_idx, 0));
      }

      return true;
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_connection_definition - "
                        "error loading " << parser_.current_token () <<
                        " definition at #" << parser_.line ());

    return false;
  }

  Config_Loader::literal_type
  Config_Loader::parse_literal_type (
      parser_type& parser_)
  {
    literal_type lt;

    // determine type of literal value
    if (parser_.current_token () == "[")
    {
      // sequence
      if (parser_.parse_next() == parser_type::IText)
      {
        // call recursively to determine type of first array literal member
        lt = this->parse_literal_type (parser_);
        if (!lt.unknown ())
        {
          lt.flags_ |= literal_type::lt_seq;
         }
      }
    }
    else if (str::icasecmp(parser_.current_token (), string_type("false")) ||
             str::icasecmp(parser_.current_token (), string_type("true")))
    {
      // boolean
      lt.flags_ |= literal_type::lt_bool;
    }
    else if (str::icasencmp(parser_.current_token (), string_type("0x"), 2))
    {
      // hexadecimal
      lt.flags_ |= literal_type::lt_uint;
    }
    else
    {
      string_type::value_type first = parser_.current_token ()[0];

      if (first == '\"')
      {
        // string
        lt.flags_ |= literal_type::lt_str;
      }
      else if (first == '\'')
      {
        // character
        lt.flags_ |= literal_type::lt_char;
      }
      else if (first == 'x' || first =='X' || first =='0')
      {
        if (first == '0' && parser_.current_token ().size () == 1)
        {
          lt.flags_ |= literal_type::lt_uint;
        }
        else if (first == '0' && std::isdigit (parser_.current_token ()[1]))
        {
          lt.flags_ |= literal_type::lt_oct;
        }
        else if (first != '0' || parser_.current_token ()[1] == 'x' || parser_.current_token ()[1] == 'X')

        {
          lt.flags_ |= literal_type::lt_hex;
        }
      }
      else if (first == '+' || first == '-' || first == '.' || std::isdigit (first))
      {
        // integer or decimal number
        if (first == '.' ||
            parser_.current_token ().find ('.') != string_type::npos ||
            parser_.current_token ().find ('e') != string_type::npos ||
            parser_.current_token ().find ('E') != string_type::npos)
        {
          // decimal number
          lt.flags_ |= literal_type::lt_dec;
        }
        else
        {
          // integer number
          if (first == '-')
            lt.flags_ |= literal_type::lt_int;
          else
            lt.flags_ |= literal_type::lt_uint;
        }
      }
    }

    return lt;
  }

  void
  Config_Loader::check_type (
      parser_type::type_id& type,
      literal_type lt)
  {
    if (type == parser_type::TNone)
    {
      switch (lt.base ())
      {
        case literal_type::lt_char:
          type = parser_type::TChar;
          break;
        case literal_type::lt_bool:
          type = parser_type::TBool;
          break;
        case literal_type::lt_int:
          type = parser_type::TInt64;
          break;
        case literal_type::lt_uint:
        case literal_type::lt_hex:
        case literal_type::lt_oct:
          type = parser_type::TUInt64;
          break;
        case literal_type::lt_dec:
          type = parser_type::TDouble;
          break;
        case literal_type::lt_str:
          type = parser_type::TString;
          break;
      }
    }
  }

  bool
  Config_Loader::load_property_value (
      parser_type& parser_,
      parser_type::type_id type_,
      CORBA::Any& any_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_property_value - "
                        "loading property value at #" <<
                        parser_.line ());

    literal_type lt = this->parse_literal_type (parser_);
    if (!lt.unknown ())
    {
      // check declared type
      this->check_type (type_, lt);

      if (lt.is_seq ())
      {
        return this->load_property_sequence (parser_, type_, lt, any_);
      }
      else
      {
        switch (type_)
        {
          case parser_type::TChar:
            {
              char v = cfg::parse_char (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " <<
                                  "'" << v << "'");
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TOctet:
            {
              uint8_t v = cfg::parse_uint<uint8_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " <<
                                  "0x"<< std::hex << (uint32_t)v << std::dec);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TBool:
            {
              bool v = cfg::parse_bool (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " <<
                                  (v ? "true" : "false"));
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TInt16:
            {
              int16_t v = cfg::parse_int<int16_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TInt32:
            {
              int32_t v = cfg::parse_int<int32_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TInt64:
            {
              int64_t v = cfg::parse_int<int64_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TUInt16:
            {
              uint16_t v = cfg::parse_uint<uint16_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TUInt32:
            {
              uint32_t v = cfg::parse_uint<uint32_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TUInt64:
            {
              uint64_t v = cfg::parse_uint<uint64_t> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TFloat:
            {
              float v = cfg::parse_dec<float> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TDouble:
            {
              double v = cfg::parse_dec<double> (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": " << v);
#endif
              any_ <<= v;
            }
            break;
          case parser_type::TString:
            {
              std::string v = cfg::parse_string (parser_.current_token (), lt);
#if !defined (NDEBUG)
              DANCEX11_LOG_DEBUG ("Config_Loader::load_property_value - "
                                  "loaded property value at #" <<
                                  parser_.line () << ": \"" << v << "\"");
#endif
              any_ <<= v;
            }
            break;

          default:
            return false; // should not ever happen
        }

        // setup next token
        parser_.parse_next ();

        return true;
      }
    }

    // error unidentified type
    DANCEX11_LOG_ERROR ("Config_Loader::load_property_value - "
                        "Error loading property sequence value "
                        "at #" << parser_.line () <<
                        ": unable to determine integral type; found ["
                        << parser_.current_token () << "]");

    return false;
  }

  bool
  Config_Loader::load_property_sequence (
      parser_type& parser_,
      parser_type::type_id type_,
      literal_type lt_,
      CORBA::Any& any_)
  {
    DANCEX11_LOG_TRACE ("Config_Loader::load_property_sequence - "
                        "loading property sequence value at #" <<
                        parser_.line ());

    switch (type_)
    {
      case parser_type::TChar:
        {
          std::vector<char> seq;
          do {
            seq.push_back (cfg::parse_char (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<char> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TOctet:
        {
          std::vector<uint8_t> seq;
          do {
            seq.push_back (cfg::parse_uint<uint8_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<uint8_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TBool:
        {
          std::vector<bool> seq;
          do {
            seq.push_back (cfg::parse_bool (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<bool> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TInt16:
        {
          std::vector<int16_t> seq;
          do {
            seq.push_back (cfg::parse_int<int16_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<int16_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TInt32:
        {
          std::vector<int32_t> seq;
          do {
            seq.push_back (cfg::parse_int<int32_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<int32_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TInt64:
        {
          std::vector<int64_t> seq;
          do {
            seq.push_back (cfg::parse_int<int64_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<int64_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TUInt16:
        {
          std::vector<uint16_t> seq;
          do {
            seq.push_back (cfg::parse_uint<uint16_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<uint16_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TUInt32:
        {
          std::vector<uint32_t> seq;
          do {
            seq.push_back (cfg::parse_uint<uint32_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<uint32_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TUInt64:
        {
          std::vector<uint64_t> seq;
          do {
            seq.push_back (cfg::parse_uint<uint64_t> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<uint64_t> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TFloat:
        {
          std::vector<float> seq;
          do {
            seq.push_back (cfg::parse_dec<float> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<float> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TDouble:
        {
          std::vector<double> seq;
          do {
            seq.push_back (cfg::parse_dec<double> (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<double> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;
      case parser_type::TString:
        {
          std::vector<std::string> seq;
          do {
            seq.push_back (cfg::parse_string (parser_.current_token (), lt_));
          } while (parser_.parse_next () == parser_type::IText &&
                   parser_.current_token() != "]");
#if !defined (NDEBUG)
          DANCEX11_LOG_DEBUG ("Config_Loader::load_property_sequence - "
                              "loaded property sequence value at #" <<
                              parser_.line () << ": " << std::endl <<
                              IDL::traits <std::vector<std::string> >::write (seq));
#endif
          if (parser_.current_token() == "]")
          {
            any_ <<= seq;
            parser_.parse_next ();
            return true;
          }
        }
        break;

      default:
        return false; // should not ever happen
    }

    DANCEX11_LOG_ERROR ("Config_Loader::load_property_sequence - "
                        "Error loading property sequence value "
                        "at #" << parser_.line ());

    return false;
  }

}
