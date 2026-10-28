/**
 * @file    ere_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "ere_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ExternalReferenceEndpoint
    convert_externalreferenceendpoint (const DAnCE::Config_Handlers::ExternalReferenceEndpoint& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_externalreferenceendpoint");

      Deployment::ExternalReferenceEndpoint dest {};

      dest.location (src.location ());
      dest.provider (src.provider ());

      ::XMLSchema::string<char> const* port_name = src.portName_if_set ();
      if (port_name)
      {
        dest.portName (*port_name);
      }

      std::copy(src.supportedType ().begin (),
                src.supportedType ().end (),
                std::back_inserter(dest.supportedType()));

      return dest;
    }
  }
}
