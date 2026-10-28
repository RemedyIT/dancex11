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

      ::XMLSchema::string<char> const* specific_type = src.specificType_if_set ();
      if (specific_type)
      {
        dest.specificType (*specific_type);
      }

      std::copy(src.supportedType ().begin (),
                src.supportedType ().end (),
                std::back_inserter(dest.supportedType()));

      std::copy(src.templateParam ().begin (),
                src.templateParam ().end (),
                std::back_inserter(dest.templateParam()));

      switch (src.kind ().integral ())
      {
        case DAnCE::Config_Handlers::CCMComponentPortKind::Facet:
          dest.kind (Deployment::CCMComponentPortKind::Facet);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::SimplexReceptacle:
          dest.kind (Deployment::CCMComponentPortKind::SimplexReceptacle);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::MultiplexReceptacle:
          dest.kind (Deployment::CCMComponentPortKind::MultiplexReceptacle);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::EventEmitter:
          dest.kind (Deployment::CCMComponentPortKind::EventEmitter);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::EventPublisher:
          dest.kind (Deployment::CCMComponentPortKind::EventPublisher);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::EventConsumer:
          dest.kind (Deployment::CCMComponentPortKind::EventConsumer);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::ExtendedPort:
          dest.kind (Deployment::CCMComponentPortKind::ExtendedPort);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::MirrorPort:
          dest.kind (Deployment::CCMComponentPortKind::MirrorPort);
          break;
      }

      return dest;
    }
  }
}
