/**
 * @file    componentpropertydescription_handler.cpp
 * @author  Marijke Hengstmengel
 *
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "componentpropertydescription_handler.h"
#include "datatype_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ComponentPropertyDescription
    convert_componentpropertydescription (const DAnCE::Config_Handlers::ComponentPropertyDescription& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_componentpropertydescription");

      Deployment::ComponentPropertyDescription dest {};

      dest.name (src.name ());
      dest.type(DAnCEX11::Config_Handlers::convert_datatype (src.type ()));

      return dest;
    }
  }
}
