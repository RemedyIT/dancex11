/**
 * @file    property_handler.cpp
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "property_handler.h"
#include "any_handler.h"
#include "dancex11/logger/log.h"
#include "tools/config_handlers/common.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::Property
    convert_property (const DAnCE::Config_Handlers::Property& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_property");

      Deployment::Property dest {};
      try
        {
          dest.name(src.name ());

          Any_Handler::extract_into_any (src.value (), dest.value());
        }
      catch (const Config_Error &ex)
        {
          throw Config_Error (src.name (), ex.get_error());
        }

      return dest;
    }
  }
}
