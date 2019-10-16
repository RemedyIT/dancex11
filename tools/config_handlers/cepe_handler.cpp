/**
 * @file    cepe_handler.cpp
 * @author  Marijke Hengstmengel
 *
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "cepe_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ComponentExternalPortEndpoint
    convert_componentexternalportendpoint (const DAnCE::Config_Handlers::ComponentExternalPortEndpoint& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_componentexternalportendpoint");

      Deployment::ComponentExternalPortEndpoint dest {};

      dest.portName (src.portName ());

      return dest;
    }
  }
}
