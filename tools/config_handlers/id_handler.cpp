/**
 * @file    id_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "id_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ImplementationDependency
    convert_implementationdependency (const DAnCE::Config_Handlers::ImplementationDependency& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_implementationdependency");

      Deployment::ImplementationDependency dest {};

      dest.requiredType (src.requiredType ());

      return dest;
    }
  }
}
