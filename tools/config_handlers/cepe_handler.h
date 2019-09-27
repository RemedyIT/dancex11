/**
 * @file    cepe_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_CEPE_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_CEPE_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ComponentExternalPortEndpoint
    convert_componentexternalportendpoint (const DAnCE::Config_Handlers::ComponentExternalPortEndpoint& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_CEPE_HANDLER_H*/
