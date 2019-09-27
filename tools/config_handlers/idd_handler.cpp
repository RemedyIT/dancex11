/**
 * @file    idd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "idd_handler.h"
#include "irdd_handler.h"
#include "mdd_handler.h"
#include "property_handler.h"
#include "cdp.hpp"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::InstanceDeploymentDescription
    convert_instancedeploymentdescription (const DAnCE::Config_Handlers::InstanceDeploymentDescription& src,
                                           const std::unordered_map<std::string, size_t>& monolithicdeploymentdescription_map)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_instancedeploymentdescription");

      Deployment::InstanceDeploymentDescription dest {};

      dest.name (src.name ());
      dest.node (src.node ());
      dest.source().push_back (src.source ());

      // Lookup the reference for our mdd, when we find it we use the index, if we can't find
      // it we set it to max to indicate that we failed
      auto result = monolithicdeploymentdescription_map.find (src.implementation ().idref ().id ());
      if (result != monolithicdeploymentdescription_map.end ())
        dest.implementationRef(static_cast<uint32_t>(result->second));
      else
        dest.implementationRef(std::numeric_limits<uint32_t>::max());

      std::transform(src.begin_configProperty (),
                    src.end_configProperty (),
                    std::back_inserter(dest.configProperty()),
                    DAnCEX11::Config_Handlers::convert_property);

      std::transform(src.begin_deployedResource (),
                    src.end_deployedResource (),
                    std::back_inserter(dest.deployedResource()),
                    DAnCEX11::Config_Handlers::convert_instanceresourcedeploymentdescription);

      std::transform(src.begin_deployedSharedResource (),
                    src.end_deployedSharedResource (),
                    std::back_inserter(dest.deployedSharedResource()),
                    DAnCEX11::Config_Handlers::convert_instanceresourcedeploymentdescription);

      return dest;
    }
  }
}
