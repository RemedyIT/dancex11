/**
 * @file    dd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dd_handler.h"
#include "node_handler.h"
#include "property_handler.h"
#include "satisfierproperty_handler.h"
#include "Deployment.hpp"
#include "dancex11/logger/log.h"
#include "common.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
  template <typename T>
  void add_named (T const& value,
                  std::unordered_map<std::string, size_t>& indices,
                  std::vector<T const*>& values)
  {
    if (indices.emplace (value.name (), values.size ()).second)
      values.push_back (std::addressof (value));
  }

  template <typename IndexMap>
  uint32_t find_index (IndexMap const& indices,
                       std::string const& name,
                       char const* type)
  {
    typename IndexMap::const_iterator const found = indices.find (name);
    if (found == indices.end ())
      throw DAnCEX11::Config_Handlers::Config_Error (
        name, std::string ("Unresolved ") + type + " reference in domain");
    return static_cast<uint32_t> (found->second);
  }
}

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    std::unique_ptr<Deployment::Domain>
    DD_Handler::resolve_domain (const DAnCE::Config_Handlers::Domain &domain) const
    {
      DANCEX11_LOG_TRACE("DD_Handler::resolve_domain");

      std::unique_ptr<Deployment::Domain> idl_dd = std::make_unique<Deployment::Domain> ();

      ::XMLSchema::string<char> const* label = domain.label_if_set ();
      if (label)
      {
        idl_dd->label(*label);
      }

      ::XMLSchema::string<char> const* uuid = domain.UUID_if_set ();
      if (uuid)
      {
        idl_dd->UUID (*uuid);
      }

      using Node = DAnCE::Config_Handlers::Node;
      using Interconnect = DAnCE::Config_Handlers::Interconnect;
      using Bridge = DAnCE::Config_Handlers::Bridge;
      using SharedResource = DAnCE::Config_Handlers::SharedResource;

      std::vector<Node const*> nodes;
      std::vector<Interconnect const*> interconnects;
      std::vector<Bridge const*> bridges;
      std::vector<SharedResource const*> shared_resources;
      std::unordered_map<std::string, size_t> node_indices;
      std::unordered_map<std::string, size_t> interconnect_indices;
      std::unordered_map<std::string, size_t> bridge_indices;
      std::unordered_map<std::string, size_t> shared_resource_indices;

      for (Node const& value : domain.node ())
        add_named (value, node_indices, nodes);
      for (Interconnect const& value : domain.interconnect ())
        add_named (value, interconnect_indices, interconnects);
      for (Bridge const& value : domain.bridge ())
        add_named (value, bridge_indices, bridges);
      for (SharedResource const& value : domain.sharedResource ())
        add_named (value, shared_resource_indices, shared_resources);

      for (size_t n {}, i {}, b {}, s {};
           n < nodes.size () || i < interconnects.size () ||
             b < bridges.size () || s < shared_resources.size ();)
      {
        while (n < nodes.size ())
        {
          Node const& value = *nodes[n++];
          for (Interconnect const& connection : value.connection ())
            add_named (connection, interconnect_indices, interconnects);
          for (SharedResource const& resource : value.sharedResource ())
            add_named (resource, shared_resource_indices, shared_resources);
        }
        while (i < interconnects.size ())
        {
          Interconnect const& value = *interconnects[i++];
          for (Bridge const& connection : value.connection ())
            add_named (connection, bridge_indices, bridges);
          for (Node const& connection : value.connect ())
            add_named (connection, node_indices, nodes);
        }
        while (b < bridges.size ())
        {
          for (Interconnect const& connection : bridges[b++]->connect ())
            add_named (connection, interconnect_indices, interconnects);
        }
        while (s < shared_resources.size ())
        {
          add_named (shared_resources[s++]->node (), node_indices, nodes);
        }
      }

      for (SharedResource const* src : shared_resources)
      {
        Deployment::SharedResource dest {};
        dest.name (src->name ());
        dest.resourceType ().push_back (src->resourceType ());
        dest.nodeRef ().push_back (find_index (node_indices, src->node ().name (), "node"));
        dest.property ().push_back (convert_satisfierproperty (src->property ()));
        idl_dd->sharedResource ().push_back (std::move (dest));
      }

      for (Node const* src : nodes)
      {
        Deployment::Node dest = convert_node (*src);
        for (SharedResource const& resource : src->sharedResource ())
          dest.sharedResourceRef ().push_back (
            find_index (shared_resource_indices, resource.name (), "shared resource"));
        for (Interconnect const& connection : src->connection ())
          dest.connectionRef ().push_back (find_index (interconnect_indices, connection.name (), "interconnect"));
        idl_dd->node ().push_back (std::move (dest));
      }

      for (Interconnect const* src : interconnects)
      {
        Deployment::Interconnect dest {};
        dest.name (src->name ());
        ::XMLSchema::string<char> const* label = src->label_if_set ();
        if (label) dest.label (*label);
        for (Bridge const& connection : src->connection ())
          dest.connectionRef ().push_back (find_index (bridge_indices, connection.name (), "bridge"));
        for (Node const& connection : src->connect ())
          dest.connectRef ().push_back (find_index (node_indices, connection.name (), "node"));
        std::transform (src->resource ().begin (), src->resource ().end (),
                        std::back_inserter (dest.resource ()), convert_resource);
        idl_dd->interconnect ().push_back (std::move (dest));
      }

      for (Bridge const* src : bridges)
      {
        Deployment::Bridge dest {};
        dest.name (src->name ());
        ::XMLSchema::string<char> const* label = src->label_if_set ();
        if (label) dest.label (*label);
        for (Interconnect const& connection : src->connect ())
          dest.connectRef ().push_back (find_index (interconnect_indices, connection.name (), "interconnect"));
        std::transform (src->resource ().begin (), src->resource ().end (),
                        std::back_inserter (dest.resource ()), convert_resource);
        idl_dd->bridge ().push_back (std::move (dest));
      }

      std::transform (domain.infoProperty ().begin (), domain.infoProperty ().end (),
                      std::back_inserter (idl_dd->infoProperty ()), convert_property);

      return idl_dd;
    }
  }
}

