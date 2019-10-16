/**
 * @file    satisfierproperty_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_HANDLERS_SATIS_PROPERTY_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_SATIS_PROPERTY_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::SatisfierProperty
    convert_satisfierproperty (const DAnCE::Config_Handlers::SatisfierProperty& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_SATIS_PROPERTY_HANDLER_H*/
