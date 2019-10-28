/**
 * @file    node_handler.cpp
 * @author  Johnny Willemsen
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "node_handler.h"
#include "satisfierproperty_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::Resource
    convert_resource (const DAnCE::Config_Handlers::Resource& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_resource");

      Deployment::Resource dest {};

      dest.name (src.name ());

      std::copy(src.begin_resourceType (),
                src.end_resourceType (),
                std::back_inserter(dest.resourceType()));

      std::transform(src.begin_property (),
                     src.end_property (),
                     std::back_inserter(dest.property()),
                     DAnCEX11::Config_Handlers::convert_satisfierproperty);

      return dest;
    }

    Deployment::Node
    convert_node (const DAnCE::Config_Handlers::Node& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_node");

      Deployment::Node dest {};

      dest.name (src.name ());

      if (src.label_p ())
      {
        dest.label (src.label ());
      }

      std::transform(src.begin_resource (),
                     src.end_resource (),
                     std::back_inserter(dest.resource()),
                     DAnCEX11::Config_Handlers::convert_resource);

      return dest;
    }
  }
}
