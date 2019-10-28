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
        case DAnCE::Config_Handlers::ResourceUsageKind::None_l:
          dest.resourceUsage (Deployment::ResourceUsageKind::None);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::InstanceUsesResource_l:
          dest.resourceUsage (Deployment::ResourceUsageKind::InstanceUsesResource);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::ResourceUsesInstance_l:
          dest.resourceUsage (Deployment::ResourceUsageKind::ResourceUsesInstance);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::PortUsesResource_l:
          dest.resourceUsage (Deployment::ResourceUsageKind::PortUsesResource);
          break;
        case DAnCE::Config_Handlers::ResourceUsageKind::ResourceUsesPort_l:
          dest.resourceUsage (Deployment::ResourceUsageKind::ResourceUsesPort);
          break;
      }

      dest.requirementName (src.requirementName ());
      dest.resourceName (src.resourceName ());

      std::transform(src.begin_property (),
                     src.end_property (),
                     std::back_inserter(dest.property()),
                     DAnCEX11::Config_Handlers::convert_property);

      return dest;
    }
  }
}
