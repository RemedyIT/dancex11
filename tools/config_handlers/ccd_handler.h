/**
 * @file    ccd_handler.h
 * @author  Marijke Hengstmengel
 *
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_CCD_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_CCD_HANDLER_H

#pragma once

#include "ccd.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::ComponentInterfaceDescription
    convert_componentinterfacedescription (const DAnCE::Config_Handlers::ComponentInterfaceDescription& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_CCD_HANDLER_H */
