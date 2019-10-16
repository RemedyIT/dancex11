/**
 * @file    req_handler.cpp
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "req_handler.h"
#include "property_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::Requirement convert_requirement (const DAnCE::Config_Handlers::Requirement& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_requirement");

      Deployment::Requirement dest {};

      dest.name (src.name ());
      dest.resourceType (src.resourceType ());

      std::transform(src.begin_property (),
                     src.end_property (),
                     std::back_inserter(dest.property()),
                     DAnCEX11::Config_Handlers::convert_property);

      return dest;
    }
  }
}
