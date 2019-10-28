/**
 * @file    crdd_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_HANDLERS_CRDD_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_CRDD_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{

  namespace Config_Handlers
  {
    Deployment::ConnectionResourceDeploymentDescription
    convert_connectionresourcedeploymentdescription (const DAnCE::Config_Handlers::ConnectionResourceDeploymentDescription& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_CRDD_HANDLER_H*/
