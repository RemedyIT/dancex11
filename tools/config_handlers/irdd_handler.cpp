/**
 * @file    irdd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "irdd_handler.h"
#include "property_handler.h"
#include "dancex11/logger/log.h"


namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::InstanceResourceDeploymentDescription
    convert_instanceresourcedeploymentdescription (const DAnCE::Config_Handlers::InstanceResourceDeploymentDescription& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_instanceresourcedeploymentdescription");

      Deployment::InstanceResourceDeploymentDescription dest {};

      switch (src.resourceUsage ().integral ())
      {
        case DAnCE::Config_Handlers::ResourceUsageKind::None:
          dest.resourceUsage (Deployment::ResourceUsageKind::None);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::InstanceUsesResource:
          dest.resourceUsage (Deployment::ResourceUsageKind::InstanceUsesResource);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::ResourceUsesInstance:
          dest.resourceUsage (Deployment::ResourceUsageKind::ResourceUsesInstance);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::PortUsesResource:
          dest.resourceUsage (Deployment::ResourceUsageKind::PortUsesResource);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::ResourceUsesPort:
          dest.resourceUsage (Deployment::ResourceUsageKind::ResourceUsesPort);
          break;
      }

      dest.requirementName (src.requirementName ());
      dest.resourceName (src.resourceName ());

      std::transform(src.property ().begin (),
                     src.property ().end (),
                     std::back_inserter(dest.property()),
                     DAnCEX11::Config_Handlers::convert_property);

      return dest;
    }
  }
}
