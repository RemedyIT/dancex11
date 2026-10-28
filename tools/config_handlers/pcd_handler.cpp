/**
 * @file    pcd_handler.cpp
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "pcd_handler.h"
#include "req_handler.h"
#include "cepe_handler.h"
#include "pspe_handler.h"
#include "ere_handler.h"
#include "crdd_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::PlanConnectionDescription
    convert_planconnectiondescription (
      const DAnCE::Config_Handlers::PlanConnectionDescription& src,
      const std::unordered_map<std::string, size_t>& instancedeploymentdescription_map)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_planconnectiondescription");

      Deployment::PlanConnectionDescription dest {};

      dest.name (src.name ());

      ::XMLSchema::string<char> const* source = src.source_if_set ();
      if (source)
        {
          dest.source().push_back (*source);
        }

      std::transform(src.deployRequirement ().begin (),
                     src.deployRequirement ().end (),
                     std::back_inserter(dest.deployRequirement()),
                     DAnCEX11::Config_Handlers::convert_requirement);

      std::transform(src.externalEndpoint ().begin (),
                     src.externalEndpoint ().end (),
                     std::back_inserter(dest.externalEndpoint()),
                     DAnCEX11::Config_Handlers::convert_componentexternalportendpoint);

      std::transform(src.internalEndpoint ().begin (),
                     src.internalEndpoint ().end (),
                     std::back_inserter(dest.internalEndpoint()),
                     [&](const DAnCE::Config_Handlers::PlanSubcomponentPortEndpoint& src)
                     {
                       return DAnCEX11::Config_Handlers::convert_plansubcomponentportendpoint(src, instancedeploymentdescription_map);
                     });

      std::transform(src.externalReference ().begin (),
                     src.externalReference ().end (),
                     std::back_inserter(dest.externalReference()),
                     DAnCEX11::Config_Handlers::convert_externalreferenceendpoint);

      std::transform(src.deployedResource ().begin (),
                     src.deployedResource ().end (),
                     std::back_inserter(dest.deployedResource()),
                     DAnCEX11::Config_Handlers::convert_connectionresourcedeploymentdescription);

      return dest;
    }
  }
}
