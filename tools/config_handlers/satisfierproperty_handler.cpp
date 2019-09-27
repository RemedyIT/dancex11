/**
 * @file    satisfierproperty_handler.cpp
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "any_handler.h"
#include "property_handler.h"
#include "dancex11/logger/log.h"
#include "tools/config_handlers/common.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::SatisfierProperty
    convert_satisfierproperty (const DAnCE::Config_Handlers::SatisfierProperty& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_satisfierproperty");

      Deployment::SatisfierProperty dest {};

      dest.name (src.name ());
      dest.dynamic (src.dynamic ());

      switch (src.kind ().integral ())
      {
        case DAnCE::Config_Handlers::SatisfierPropertyKind::Quantity_l:
          dest.kind( Deployment::SatisfierPropertyKind::Quantity);
          break;
        case DAnCE::Config_Handlers::SatisfierPropertyKind::Capacity_l:
          dest.kind (Deployment::SatisfierPropertyKind::Capacity);
          break;
        case DAnCE::Config_Handlers::SatisfierPropertyKind::Minimum_l:
          dest.kind (Deployment::SatisfierPropertyKind::Minimum);
          break;
        case DAnCE::Config_Handlers::SatisfierPropertyKind::Maximum_l:
          dest.kind (Deployment::SatisfierPropertyKind::Maximum);
          break;
        case DAnCE::Config_Handlers::SatisfierPropertyKind::Attribute_l:
          dest.kind (Deployment::SatisfierPropertyKind::Attribute);
          break;
        case DAnCE::Config_Handlers::SatisfierPropertyKind::Selection_l:
          dest.kind (Deployment::SatisfierPropertyKind::Selection);
          break;
      }

      try
        {
          Any_Handler::extract_into_any (src.value (), dest.value ());
        }
      catch (const Config_Error &ex)
        {
          throw Config_Error (src.name (), ex.get_error() );
        }

      return dest;
    }
  }
}
