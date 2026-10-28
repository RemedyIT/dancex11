/**
 * @file    pspe_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "pspe_handler.h"
#include "idd_handler.h"
#include "common.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::PlanSubcomponentPortEndpoint
    convert_plansubcomponentportendpoint (
      const DAnCE::Config_Handlers::PlanSubcomponentPortEndpoint& src,
      const std::unordered_map<std::string, size_t>& instancedeploymentdescription_map)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_plansubcomponentportendpoint");

      Deployment::PlanSubcomponentPortEndpoint dest {};

      dest.portName (src.portName ());

      // Lookup the reference for our mdd, when we find it we use the index, if we can't find
      // it we set it to max to indicate that we failed
      ::XMLSchema::IDREF<char> const* idref = src.instance ().idref_if_set ();
      if (!idref)
        throw Config_Error ("", "Missing idref for instance endpoint");

      std::unordered_map<std::string, size_t>::const_iterator const result =
        instancedeploymentdescription_map.find (idref->id ());
      if (result != instancedeploymentdescription_map.end ())
        dest.instanceRef(static_cast<uint32_t>(result->second));
      else
        throw Config_Error (idref->id (), "Instance reference does not identify an instance");

      ::XMLSchema::string<char> const* provider = src.provider_if_set ();
      if (provider)
        {
          if (*provider == "true")
            dest.provider (true);
          else if (*provider == "false")
            dest.provider (false);
          else
            throw Config_Error ("provider", "Expected 'true' or 'false'");
        }
      else
        {
          dest.provider () = false;
        }

      switch (src.kind ().integral ())
        {
        case DAnCE::Config_Handlers::CCMComponentPortKind::Facet:
          dest.kind (Deployment::CCMComponentPortKind::Facet);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::SimplexReceptacle:
          dest.kind(Deployment::CCMComponentPortKind::SimplexReceptacle);
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
          dest.kind ( Deployment::CCMComponentPortKind::EventConsumer);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::ExtendedPort:
          dest.kind ( Deployment::CCMComponentPortKind::ExtendedPort);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::MirrorPort:
          dest.kind ( Deployment::CCMComponentPortKind::MirrorPort);
          break;
       }

      return dest;
    }
  }
}
