/**
 * @file    ere_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
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

      if (src.portName_p ())
      {
        dest.portName (src.portName ());
      }

      std::copy(src.begin_supportedType (),
                src.end_supportedType (),
                std::back_inserter(dest.supportedType()));

      return dest;
    }
  }
}
