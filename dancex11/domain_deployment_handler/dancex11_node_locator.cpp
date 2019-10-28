/**
 * @file   dancex11_node_locator.cpp
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  locate nodes
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_node_locator.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/logger/log.h"

#include <sstream>

namespace DAnCEX11
{
  Node_Locator::Node_Locator (std::string nc)
    : orb_(DAnCEX11::State::instance ()->orb ())
  {
    DANCEX11_LOG_TRACE ("Node_Locator::Node_Locator");

    if (!nc.empty ())
    {
      DANCEX11_LOG_DEBUG ("Node_Locator::Node_Locator - " <<
                          "Resolving NC: " << nc);
      IDL::traits<CORBA::Object>::ref_type nc_obj =
          this->orb_->string_to_object (nc);
      if (nc_obj)
      {
        try
        {
          this->nc_ =
              IDL::traits<CosNaming::NamingContext>::narrow (nc_obj);
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_WARNING ("Node_Locator::Node_Locator - " <<
                                "CORBA exception when narrowing NC: " <<
                                ex);
        }
      }
      else
      {
        DANCEX11_LOG_WARNING ("Node_Locator::Node_Locator - " <<
                              "unable to resolve NC reference: " <<
                              nc);
      }
    }

    if (!this->nc_)
    {
      DANCEX11_LOG_INFO ("Node_Locator::Node_Locator - " <<
                         "no naming context provided");
    }
  }

  IDL::traits< ::Deployment::NodeManager>::ref_type
  Node_Locator::locate_node (const std::string name)
  {
    std::string ior;

    NODEMAP::const_iterator it = nodes_.find(name);
    if(it != nodes_.end()) {
      ior = it->second;
      return this->resolve_ior (name, ior);
    }
    else
    {
      return this->ns_lookup (name);
    }
  }

  bool
  Node_Locator::process_cdd (const std::string &filename, const std::string& fmt)
  {
    DANCEX11_LOG_TRACE ("Node_Locator::process_cdd");

    if (filename.empty ())
    {
      DANCEX11_LOG_ERROR( "Node_Locator::process_cdd - "
                          "Error: Provided with empty filename");

      return false;
    }

    // load domain config
    Deployment::Domain dom;
    if (!DAnCEX11::Plan_Loader::instance ()->read_domain (filename, dom, fmt))
    {
      DANCEX11_LOG_ERROR ("Node_Locator::process_cdd - "
                          "Failed to read domain config: " << filename <<
                          " (format=" << fmt << ")");
      return false;
    }

    // install nodes
    for (const Deployment::Node& node : dom.node ())
    {
      Deployment::Resource resource;

      if (!get_resource_value (DAnCEX11::NODE_RESOURCE_TYPE,
                               node.resource (),
                               resource))
      {
        DANCEX11_LOG_DEBUG("Node_Locator::process_cdd - "
                           "Error: Resource <" <<
                           DAnCEX11::NODE_RESOURCE_TYPE <<
                           "> not found.");
        return false;
      }
      std::string ior;
      if (Utility::get_satisfier_property_value (DAnCEX11::NODE_IOR,
                                                 resource.property (),
                                                 ior))
      {
        // is this a corbaloc URL style IOR?
        std::string::size_type pos = ior.find ("corbaloc:");
        if (pos == 0)
        {
          // finish the reference
          ior += "/";
          ior += node.name ();
          ior += ".NodeManager";
        }
        // otherwise assume the IOR provides a full reference to the
        // NM
      }
      else
      {
        // Do we have a port defined?
        uint16_t port;
        if (Utility::get_satisfier_property_value (DAnCEX11::NODE_PORT,
                                                   resource.property (),
                                                   port))
        {
          std::string host = node.name ();
          Utility::get_satisfier_property_value (DAnCEX11::NODE_HOST,
                                                 resource.property (),
                                                 host);
          // Construct IOR
          std::ostringstream os;
          os << "corbaloc:iiop:"
             << host << ':' << port
             << '/' << node.name ()
             << ".NodeManager";
          ior = os.str ();
        }
        else
        {
          DANCEX11_LOG_ERROR("Node_Locator::process_cdd - "
                             "Error: no <" <<
                             DAnCEX11::NODE_IOR <<
                             "> or <" <<
                             DAnCEX11::NODE_PORT <<
                             "> properties defined.");
          return false;
        }

      }

      DANCEX11_LOG_DEBUG ("Node_Locator::process_cdd - "
                          "Storing IOR [" << ior <<
                          "] for destination [" <<
                          node.name () << "]");

      this->nodes_[node.name ()] = ior;
    }

    return true;
  }

  IDL::traits< ::Deployment::NodeManager>::ref_type
  Node_Locator::resolve_ior (const std::string name, const std::string ior)
  {
    DANCEX11_LOG_TRACE ("Node_Locator::resolve_ior");

    DANCEX11_LOG_DEBUG ("Node_Locator::resolve_ior - "
                        "Resolving ior [" << ior << "] for destination [" << name << "]");

    IDL::traits<CORBA::Object>::ref_type  obj = this->orb_->string_to_object (ior);

    IDL::traits< ::Deployment::NodeManager>::ref_type nm = IDL::traits< ::Deployment::NodeManager>::narrow (obj);

    if (!nm)
    {
      DANCEX11_LOG_ERROR ("Node_Locator::resolve_ior - "
                          "Error: Unable to retrieve reference for destination ["
                          << name << "] and ior [" << ior << "]");
    }
    return nm;
  }

  void
  Node_Locator::store_ior (const std::string name, const std::string ior)
  {
    DANCEX11_LOG_TRACE ("Node_Locator::store_ior");

    this->nodes_.insert ( std::pair<std::string, std::string>(name, ior));
  }

  IDL::traits< ::Deployment::NodeManager>::ref_type
  Node_Locator::ns_lookup (const std::string nodename)
  {
    DANCEX11_LOG_TRACE ("Node_Locator::ns_lookup");
    IDL::traits< ::Deployment::NodeManager>::ref_type nm ;

    if (!this->nc_)
    {
      DANCEX11_LOG_ERROR ("Node_Locator::ns_lookup - "
                          "Nameservice lookup of node [" << nodename
                          << "] failed because there is no naming service.");

      return nm;
    }

    try
    {
      CosNaming::Name name;
      name.push_back(CosNaming::NameComponent (nodename, "NodeManager"));

      IDL::traits<CORBA::Object>::ref_type obj = this->nc_->resolve (name);

      nm = IDL::traits< ::Deployment::NodeManager>::narrow (obj);

      if (!nm)
      {
        DANCEX11_LOG_ERROR ("Node_Locator::ns_lookup - "
                          "Unable to narrow provided reference for node [" << nodename << "]");
      }

      return nm;
    }
    catch (const CORBA::Exception &e)
    {
      DANCEX11_LOG_ERROR ("Node_Locator::ns_lookup - "
                          "Caught CORBA exception while looking up node [" <<
                          nodename << "] :" << e);
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("Node_Locator::ns_lookup - "
                          "Caught unexpected exception while looking up node [" <<
                          nodename << "]");
    }
    return nm;
  }

  bool
  Node_Locator::get_resource_value (const std::string type,
                                    const ::Deployment::Resources &resources,
                                    ::Deployment::Resource  &val)
  {
    DANCEX11_LOG_TRACE ("Node_Locator::get_resource_value<const char *>");

    DANCEX11_LOG_DEBUG ("Node_Locator::get_resource_value - "
                        "Finding resource for type '" << type << "'");

    for (uint32_t i = 0; i < resources.size (); ++i)
    {
      // search for the resource with resourceType
      for (uint32_t k = 0;k < resources[i].resourceType().size ();k++)
      {
        if (type == resources[i].resourceType()[k])
        {
          DANCEX11_LOG_DEBUG ("Node_Locator::get_resource_value - "
                            "Found resource for type '" << type << "'");

          val = resources[i];
          return true;
        }
      }
    }
    DANCEX11_LOG_ERROR ("Node_Locator::get_resource_value - "
                        "Failed to extract resource for " << type);
    return false;
  }
}

