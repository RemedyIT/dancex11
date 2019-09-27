/**
 * @file    dp_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_DP_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_DP_HANDLER_H

#pragma once

#include "cdp.hpp"
#include "dancex11/deployment/deployment_dataC.h"
#include "config_handlers_export.h"
#include <memory>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    /*
     * @class DP_Handler
     *
     * @brief Handler for Deployment::DeploymentPlan
     */
    class Config_Handlers_Export DP_Handler final
    {
    public:
      DP_Handler () = default;
      ~DP_Handler () = default;

      /// Generates the XSC->IDL mapping
      std::unique_ptr<Deployment::DeploymentPlan> resolve_plan (const DAnCE::Config_Handlers::deploymentPlan &dp) const;

    private:
      DP_Handler (const DP_Handler&) = delete;
      DP_Handler (DP_Handler&&) = delete;
      DP_Handler& operator= (const DP_Handler&) = delete;
      DP_Handler& operator= (DP_Handler&&) = delete;
    };
  }
}

#endif /*DANCEX11_CONFIG_HANDLERS_DP_HANDLER_H*/
