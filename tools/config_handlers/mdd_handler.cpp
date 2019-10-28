/**
 * @file    mdd_handler.cpp
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "mdd_handler.h"
#include "add_handler.h"
#include "property_handler.h"
#include "req_handler.h"
#include "cdp.hpp"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::MonolithicDeploymentDescription
    convert_monolithicdeploymentdescription (const DAnCE::Config_Handlers::MonolithicDeploymentDescription& src,
                                             const std::unordered_map<std::string, size_t>& artifact_map)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_monolithicdeploymentdescription");

      Deployment::MonolithicDeploymentDescription dest {};

      dest.name(src.name ());

      std::copy(src.begin_source (),
                src.end_source (),
                std::back_inserter(dest.source()));

      std::transform(src.begin_artifact (),
                     src.end_artifact (),
                     std::back_inserter(dest.artifactRef()),
                     [&](const DAnCE::Config_Handlers::IdRef& id)
                     {
                       auto result = artifact_map.find (id.idref().id ());
                       if (result != artifact_map.end ())
                         return static_cast<uint32_t>(result->second);
                       else
                         return std::numeric_limits<uint32_t>::max();
                     });

      std::transform(src.begin_execParameter (),
                     src.end_execParameter (),
                     std::back_inserter(dest.execParameter()),
                     DAnCEX11::Config_Handlers::convert_property);

      std::transform(src.begin_deployRequirement (),
                     src.end_deployRequirement (),
                     std::back_inserter(dest.deployRequirement()),
                     DAnCEX11::Config_Handlers::convert_requirement);

      return dest;
    }
  }
}
