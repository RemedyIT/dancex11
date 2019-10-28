/**
 * @file    rdd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "rdd_handler.h"
#include "property_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ResourceDeploymentDescription
    convert_resourcedeploymentdescription (const DAnCE::Config_Handlers::ResourceDeploymentDescription& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_resourcedeploymentdescription");

      Deployment::ResourceDeploymentDescription dest {};

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
