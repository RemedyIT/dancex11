/**
 * @file    pspe_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "pspe_handler.h"
#include "idd_handler.h"
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
      auto result = instancedeploymentdescription_map.find (src.instance ().idref ().id ());
      if (result != instancedeploymentdescription_map.end ())
        dest.instanceRef(static_cast<uint32_t>(result->second));
      else
        dest.instanceRef(std::numeric_limits<uint32_t>::max());

      if (src.provider_p ())
        {
          dest.provider (src.provider () == "true");
        }
      else
        {
          dest.provider () = false;
        }

      switch (src.kind ().integral ())
        {
        case DAnCE::Config_Handlers::CCMComponentPortKind::Facet_l:
          dest.kind (Deployment::CCMComponentPortKind::Facet);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::SimplexReceptacle_l:
          dest.kind(Deployment::CCMComponentPortKind::SimplexReceptacle);
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
          dest.kind ( Deployment::CCMComponentPortKind::EventConsumer);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::ExtendedPort_l:
          dest.kind ( Deployment::CCMComponentPortKind::ExtendedPort);
          break;
        case DAnCE::Config_Handlers::CCMComponentPortKind::MirrorPort_l:
          dest.kind ( Deployment::CCMComponentPortKind::MirrorPort);
          break;
       }

      return dest;
    }
  }
}
