/**
 * @file    dynenum_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_DYNENUM_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_DYNENUM_HANDLER_H

#pragma once

#include "dynany_handler_export.h"

#include "tao/x11/orb.h"
#include "tao/x11/dynamic_any/dynamicany.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    using DAnCE::Config_Handlers::DataType;
    using DAnCE::Config_Handlers::DataValue;
    using DAnCE::Config_Handlers::Any;
    /**
     * @class DynEnum_Handler
     * @brief Translates enums specified in deployment plans into Dynamic Anys
     */
    class DynEnum_Handler
    {
    public:
      static IDL::traits<DynamicAny::DynAny>::ref_type
      extract_into_dynany (const DataType &type,
                           const DataValue &value,
                          IDL::traits<CORBA::TypeCode>::ref_type = {});

      static IDL::traits<CORBA::TypeCode>::ref_type
      create_typecode (const DataType &type);
    };

  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_DYNENUM_HANDLER_H*/
