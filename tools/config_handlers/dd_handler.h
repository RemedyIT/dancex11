/**
 * @file    dd_handler.h
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_HANDLERS_DD_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_DD_HANDLER_H

#pragma once

#include "config_handlers_export.h"
#include <memory>
#include "cdd.hpp"
#include "dancex11/deployment/deployment_targetdataC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    /*
    * @class DD_Handler
    *
    * @brief Handler class for Domain
    */

    class Config_Handlers_Export DD_Handler final
    {
    public:
      DD_Handler() = default;
      ~DD_Handler () = default;

      /// Generates the XSC->IDL mapping
      std::unique_ptr<Deployment::Domain> resolve_domain (const DAnCE::Config_Handlers::Domain &dm) const;

    private:
      DD_Handler (const DD_Handler&) = delete;
      DD_Handler (DD_Handler&&) = delete;
      DD_Handler& operator= (const DD_Handler&) = delete;
      DD_Handler& operator= (DD_Handler&&) = delete;
    };
  }
}

#endif /* DD_HANDLER_H */
