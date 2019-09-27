/**
 * @file    property_handler.h
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_PROPERTY_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_PROPERTY_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::Property
    convert_property (const DAnCE::Config_Handlers::Property& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_PROPERTY_HANDLER_H*/
