/**
 * @file    dp_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dp_handler.h"
#include "ccd_handler.h"
#include "add_handler.h"
#include "mdd_handler.h"
#include "idd_handler.h"
#include "id_handler.h"
#include "pl_handler.h"
#include "property_handler.h"
#include "common.h"
#include "cdp.hpp"
#include "pcd_handler.h"
#include "dancex11/logger/log.h"
#include <unordered_map>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    std::unique_ptr<Deployment::DeploymentPlan>
    DP_Handler::resolve_plan (const DAnCE::Config_Handlers::deploymentPlan &xsc_dp) const
    {
      DANCEX11_LOG_TRACE ("DP_Handler::resolve_plan");

      std::unique_ptr<Deployment::DeploymentPlan> idl_dp = std::make_unique<Deployment::DeploymentPlan> ();

      ::XMLSchema::string<char> const* label = xsc_dp.label_if_set ();
      if (label)
        {
          idl_dp->label (*label);
        }

      ::XMLSchema::string<char> const* uuid = xsc_dp.UUID_if_set ();
      if (uuid)
        {
          idl_dp->UUID (*uuid);
        }

      std::transform(xsc_dp.dependsOn ().begin (),
                     xsc_dp.dependsOn ().end (),
                     std::back_inserter(idl_dp->dependsOn()),
                     DAnCEX11::Config_Handlers::convert_implementationdependency);

      std::transform(xsc_dp.infoProperty ().begin (),
                     xsc_dp.infoProperty ().end (),
                     std::back_inserter(idl_dp->infoProperty()),
                     DAnCEX11::Config_Handlers::convert_property);

      ::DAnCE::Config_Handlers::ComponentInterfaceDescription const* realizes = xsc_dp.realizes_if_set ();
      if (realizes)
        {
          idl_dp->realizes(DAnCEX11::Config_Handlers::convert_componentinterfacedescription (*realizes));
        }

      std::unordered_map<std::string, size_t> artifact_map;
      for (DAnCE::Config_Handlers::ArtifactDeploymentDescription const& src : xsc_dp.artifact ())
        {
          size_t const index = idl_dp->artifact ().size ();
          ::XMLSchema::ID<char> const* id = src.id_if_set ();
          if (id)
            artifact_map.insert ({*id, index});
          idl_dp->artifact ().push_back (
            DAnCEX11::Config_Handlers::convert_artifactdeploymentdescription (src));
        }

      std::unordered_map<std::string, size_t> monolithicdeploymentdescription_map;
      for (DAnCE::Config_Handlers::MonolithicDeploymentDescription const& src : xsc_dp.implementation ())
        {
          size_t const index = idl_dp->implementation ().size ();
          ::XMLSchema::ID<char> const* id = src.id_if_set ();
          if (id)
            monolithicdeploymentdescription_map.insert ({*id, index});
          idl_dp->implementation ().push_back (
            DAnCEX11::Config_Handlers::convert_monolithicdeploymentdescription (src, artifact_map));
        }

      std::unordered_map<std::string, size_t> instancedeploymentdescription_map;
      for (DAnCE::Config_Handlers::InstanceDeploymentDescription const& src : xsc_dp.instance ())
        {
          size_t const index = idl_dp->instance ().size ();
          ::XMLSchema::ID<char> const* id = src.id_if_set ();
          if (id)
            instancedeploymentdescription_map.insert ({*id, index});
          idl_dp->instance ().push_back (
            DAnCEX11::Config_Handlers::convert_instancedeploymentdescription (
              src, monolithicdeploymentdescription_map));
        }

      for (DAnCE::Config_Handlers::PlanPropertyMapping const& src : xsc_dp.externalProperty ())
        {
          Deployment::PlanSubcomponentPropertyReferences delegates;
          for (DAnCE::Config_Handlers::PlanSubcomponentPropertyReference const& delegate : src.delegatesTo ())
            {
              DAnCE::Config_Handlers::InstanceDeploymentDescription const& nested = delegate.instance ();
              size_t instance_index = idl_dp->instance ().size ();
              ::XMLSchema::ID<char> const* id = nested.id_if_set ();
              if (id)
                {
                  std::unordered_map<std::string, size_t>::const_iterator const found =
                    instancedeploymentdescription_map.find (*id);
                  if (found != instancedeploymentdescription_map.end ())
                    instance_index = found->second;
                }

              if (instance_index == idl_dp->instance ().size ())
                {
                  ::XMLSchema::IDREF<char> const* implementation = nested.implementation ().idref_if_set ();
                  for (DAnCE::Config_Handlers::InstanceDeploymentDescription const& candidate : xsc_dp.instance ())
                    {
                      ::XMLSchema::IDREF<char> const* candidate_implementation =
                        candidate.implementation ().idref_if_set ();
                      if (candidate.name () == nested.name () && candidate.node () == nested.node () &&
                          implementation && candidate_implementation && *implementation == *candidate_implementation)
                        {
                          ::XMLSchema::ID<char> const* candidate_id = candidate.id_if_set ();
                          if (!candidate_id)
                            throw Config_Error (nested.name (), "Delegated instance has no XMI ID");
                          std::unordered_map<std::string, size_t>::const_iterator const found =
                            instancedeploymentdescription_map.find (*candidate_id);
                          if (found != instancedeploymentdescription_map.end ())
                            {
                              if (instance_index != idl_dp->instance ().size ())
                                throw Config_Error (nested.name (), "Ambiguous delegated instance reference");
                              instance_index = found->second;
                            }
                        }
                    }
                }

              if (instance_index == idl_dp->instance ().size ())
                throw Config_Error (nested.name (), "Unable to resolve delegated instance reference");

              delegates.emplace_back (delegate.propertyName (), static_cast<uint32_t>(instance_index));
            }

          CORBA::StringSeq source;
          std::copy (src.source ().begin (), src.source ().end (), std::back_inserter (source));
          idl_dp->externalProperty ().emplace_back (
            src.name (), std::move (source), src.externalName (), std::move (delegates));
        }

      std::transform(xsc_dp.connection ().begin (),
                     xsc_dp.connection ().end (),
                     std::back_inserter(idl_dp->connection()),
                     [&](const DAnCE::Config_Handlers::PlanConnectionDescription& src)
                     {
                       return DAnCEX11::Config_Handlers::convert_planconnectiondescription (
                         src, instancedeploymentdescription_map);
                     });

      std::transform(xsc_dp.localityConstraint ().begin (),
                     xsc_dp.localityConstraint ().end (),
                     std::back_inserter(idl_dp->localityConstraint()),
                     [&](const DAnCE::Config_Handlers::PlanLocality& src)
                     {
                       return DAnCEX11::Config_Handlers::convert_planlocality(src, instancedeploymentdescription_map);
                     });

      return idl_dp;
    }
  }
}
