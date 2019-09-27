/**
 * @file    any_handler.h
 * @author  Marijke Hengstmengel
 *
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_ANY_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_ANY_HANDLER_H

#pragma once

#include "tao/x11/anytypecode/any.h"
#include "Basic_Deployment_Data.hpp"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
     /*
      * @class Any_Handler
     *
     * @brief Handler class for ComponentInterfaceDescription types.
     *
     * This class defines handler methods to map values from
     * XSC Any objects, parsed from the descriptor files, to the
     * corresponding CORBA IDL Any type.
     */
    class Any_Handler final
    {
    public:
      Any_Handler () = default;
      ~Any_Handler () = default;

//X11_FUZZ: disable check_taox11_namespaces
      static void extract_into_any (const DAnCE::Config_Handlers::Any& desc, TAOX11_CORBA::Any& toconfig);
//X11_FUZZ: enable check_taox11_namespaces
    };
  }
}
#endif /* DANCEX11_CONFIG_HANDLERS_ANY_HANDLER_H*/
