/**
 * @file    cpd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "cpd_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ComponentPortDescription
    convert_componentportdescription (const DAnCE::Config_Handlers::ComponentPortDescription& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_componentportdescription");

      Deployment::ComponentPortDescription dest {};

      dest.name (src.name ());
      dest.provider (src.provider ());
      dest.exclusiveProvider (src.exclusiveProvider ());
      dest.exclusiveUser (src.exclusiveUser ());
      dest.optional (src.optional ());

      if (src.specificType_p ())
      {
        dest.specificType (src.specificType ());
      }

      std::copy(src.begin_supportedType (),
                src.end_supportedType (),
                std::back_inserter(dest.supportedType()));

      std::copy(src.begin_templateParam (),
                src.end_templateParam (),
                std::back_inserter(dest.templateParam()));

      switch (src.kind ().integral ())
      {
        case DAnCE::Config_Handlers::CCMComponentPortKind::Facet_l:
          dest.kind (Deployment::CCMComponentPortKind::Facet);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::SimplexReceptacle_l:
          dest.kind (Deployment::CCMComponentPortKind::SimplexReceptacle);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::MultiplexReceptacle_l:
          dest.kind (Deployment::CCMComponentPortKind::MultiplexReceptacle);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::EventEmitter_l:
          dest.kind (Deployment::CCMComponentPortKind::EventEmitter);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::EventPublisher_l:
          dest.kind (Deployment::CCMComponentPortKind::EventPublisher);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::EventConsumer_l:
          dest.kind (Deployment::CCMComponentPortKind::EventConsumer);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::ExtendedPort_l:
          dest.kind (Deployment::CCMComponentPortKind::ExtendedPort);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::MirrorPort_l:
          dest.kind (Deployment::CCMComponentPortKind::MirrorPort);
          break;
      }

      return dest;
    }
  }
}
