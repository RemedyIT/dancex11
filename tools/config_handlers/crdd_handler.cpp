/**
 * @file    crdd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "crdd_handler.h"
#include "property_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ConnectionResourceDeploymentDescription
    convert_connectionresourcedeploymentdescription (const DAnCE::Config_Handlers::ConnectionResourceDeploymentDescription& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_connectionresourcedeploymentdescription");

      Deployment::ConnectionResourceDeploymentDescription dest {};

      dest.targetName (src.targetName ());
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
