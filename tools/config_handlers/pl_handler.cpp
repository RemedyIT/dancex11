/**
 * @file    pl_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "pl_handler.h"
#include "idd_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::PlanLocality
    convert_planlocality (const DAnCE::Config_Handlers::PlanLocality& src, const std::unordered_map<std::string, size_t>& instancedeploymentdescription_map)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_planlocality");

      Deployment::PlanLocality dest {};

      switch (src.constraint ().integral ())
        {
          case DAnCE::Config_Handlers::PlanLocalityKind::SameProcess_l:
            dest.constraint (Deployment::PlanLocalityKind::PlanSameProcess);
            break;
          case DAnCE::Config_Handlers::PlanLocalityKind::DifferentProcess_l:
            dest.constraint (Deployment::PlanLocalityKind::PlanDifferentProcess);
            break;
          case DAnCE::Config_Handlers::PlanLocalityKind::NoConstraint_l:
            dest.constraint(Deployment::PlanLocalityKind::PlanNoConstraint);
            break;
        }

      std::transform(src.begin_constrainedInstance (),
                     src.end_constrainedInstance (),
                     std::back_inserter(dest.constrainedInstanceRef()),
                     [&](const DAnCE::Config_Handlers::IdRef& id)
                     {
                       auto result = instancedeploymentdescription_map.find (id.idref().id ());
                       if (result != instancedeploymentdescription_map.end ())
                         return static_cast<uint32_t>(result->second);
                       else
                         return std::numeric_limits<uint32_t>::max();
                     });

      return dest;
    }
  }
}
