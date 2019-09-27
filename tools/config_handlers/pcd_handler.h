/**
 * @file    pcd_handler.h
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_PCD_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_PCD_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include <unordered_map>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::PlanConnectionDescription
    convert_planconnectiondescription (
      const DAnCE::Config_Handlers::PlanConnectionDescription& src,
      const std::unordered_map<std::string, size_t>& instancedeploymentdescription_map);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_PCD_HANDLER_H*/
