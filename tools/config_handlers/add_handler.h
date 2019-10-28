/**
 * @file    add_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_CONFIG_HANDLERS_ADD_Handler_H
#define DANCEX11_CONFIG_HANDLERS_ADD_Handler_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ArtifactDeploymentDescription
    convert_artifactdeploymentdescription (const DAnCE::Config_Handlers::ArtifactDeploymentDescription& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_ADD_Handler_H */
