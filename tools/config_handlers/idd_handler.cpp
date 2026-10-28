/**
 * @file    idd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "idd_handler.h"
#include "irdd_handler.h"
#include "mdd_handler.h"
#include "property_handler.h"
#include "cdp.hpp"
#include "common.h"
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
      ::XMLSchema::IDREF<char> const* idref = src.implementation ().idref_if_set ();
      if (!idref)
        throw Config_Error ("", "Missing idref for implementation");

      std::unordered_map<std::string, size_t>::const_iterator const result =
        monolithicdeploymentdescription_map.find (idref->id ());
      if (result != monolithicdeploymentdescription_map.end ())
        dest.implementationRef(static_cast<uint32_t>(result->second));
      else
        throw Config_Error (idref->id (), "Implementation reference does not identify an implementation");

      std::transform(src.configProperty ().begin (),
                    src.configProperty ().end (),
                    std::back_inserter(dest.configProperty()),
                    DAnCEX11::Config_Handlers::convert_property);

      std::transform(src.deployedResource ().begin (),
                    src.deployedResource ().end (),
                    std::back_inserter(dest.deployedResource()),
                    DAnCEX11::Config_Handlers::convert_instanceresourcedeploymentdescription);

      std::transform(src.deployedSharedResource ().begin (),
                    src.deployedSharedResource ().end (),
                    std::back_inserter(dest.deployedSharedResource()),
                    DAnCEX11::Config_Handlers::convert_instanceresourcedeploymentdescription);

      return dest;
    }
  }
}
