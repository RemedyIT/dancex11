/**
 * @file    req_handler.h
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_HANDLERS_REQ_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_REQ_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::Requirement
    convert_requirement (const DAnCE::Config_Handlers::Requirement& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_REQ_HANDLER_H*/
