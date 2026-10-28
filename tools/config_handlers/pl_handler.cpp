/**
 * @file    pl_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "pl_handler.h"
#include "common.h"
#include "idd_handler.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::PlanLocality
    convert_planlocality (
      const DAnCE::Config_Handlers::PlanLocality& src,
      const std::unordered_map<std::string, size_t>& instancedeploymentdescription_map)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_planlocality");

      Deployment::PlanLocality dest {};

      switch (src.constraint ().integral ())
        {
          case DAnCE::Config_Handlers::PlanLocalityKind::SameProcess:
            dest.constraint (Deployment::PlanLocalityKind::PlanSameProcess);
            break;
          case DAnCE::Config_Handlers::PlanLocalityKind::DifferentProcess:
            dest.constraint (Deployment::PlanLocalityKind::PlanDifferentProcess);
            break;
          case DAnCE::Config_Handlers::PlanLocalityKind::NoConstraint:
            dest.constraint(Deployment::PlanLocalityKind::PlanNoConstraint);
            break;
        }

      std::transform(src.constrainedInstance ().begin (),
                     src.constrainedInstance ().end (),
                     std::back_inserter(dest.constrainedInstanceRef()),
                     [&](const DAnCE::Config_Handlers::IdRef& id)
                     {
                       ::XMLSchema::IDREF<char> const* idref = id.idref_if_set ();
                       if (!idref)
                         throw Config_Error ("", "Missing idref for constrained instance");

                       std::unordered_map<std::string, size_t>::const_iterator const result =
                         instancedeploymentdescription_map.find (idref->id ());
                       if (result != instancedeploymentdescription_map.end ())
                         return static_cast<uint32_t>(result->second);
                       else
                         throw Config_Error (idref->id (),
                                             "Constrained instance reference does not identify an instance");
                     });

      return dest;
    }
  }
}
