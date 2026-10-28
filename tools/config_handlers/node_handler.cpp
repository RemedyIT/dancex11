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

      std::copy(src.resourceType ().begin (),
                src.resourceType ().end (),
                std::back_inserter(dest.resourceType()));

      std::transform(src.property ().begin (),
                     src.property ().end (),
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

      ::XMLSchema::string<char> const* label = src.label_if_set ();
      if (label)
      {
        dest.label (*label);
      }

      std::transform(src.resource ().begin (),
                     src.resource ().end (),
                     std::back_inserter(dest.resource()),
                     DAnCEX11::Config_Handlers::convert_resource);

      return dest;
    }
  }
}
