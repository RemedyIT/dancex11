/**
 * @file    pl_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_HANDLERS_PL_HANDLER_
#define DANCEX11_CONFIG_HANDLERS_PL_HANDLER_

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include <unordered_map>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::PlanLocality
    convert_planlocality (
      const DAnCE::Config_Handlers::PlanLocality& src,
      const std::unordered_map<std::string, size_t>& instancedeploymentdescription_map);
  }
}

#endif
