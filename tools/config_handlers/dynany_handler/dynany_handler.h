/**
 * @file    dynany_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_HANDLERS_DYNANY_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_DYNANY_HANDLER_H

#pragma once

#include "dynany_handler_export.h"
#include "tao/x11/orb.h"
#include "tao/x11/dynamic_any/dynamicany.h"
#include "Basic_Deployment_Data.hpp"

#include <map>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    using DAnCE::Config_Handlers::DataType;
    using DAnCE::Config_Handlers::DataValue;
    using DAnCE::Config_Handlers::TCKind;
    using DAnCE::Config_Handlers::Any;

    /**
    * @class DynAny_Handler
    *
    * @brief Handler class for ComponentInterfaceDescription types.
    *
    * This class defines handler methods to map values from
    * XSC Any objects, parsed from the descriptor files, to the
    * corresponding CORBA IDL Any type.
    *
    */
    class DynAny_Handler_Export DynAny_Handler
    {
    public:
      ~DynAny_Handler ();

      IDL::traits<DynamicAny::DynAny>::ref_type
      extract_into_dynany (const DataType &type,
                           const DataValue &value,
                           IDL::traits<CORBA::TypeCode>::ref_type = {});

      IDL::traits<CORBA::ORB>::ref_type orb ();

      IDL::traits<DynamicAny::DynAnyFactory>::ref_type daf ();

      IDL::traits<CORBA::TypeCode>::ref_type create_typecode (const DataType &type);

      void register_typecode (const std::string& typeID,
                              IDL::traits<CORBA::TypeCode>::ref_type tc);

      IDL::traits<CORBA::TypeCode>::ref_type
      get_typecode (const std::string& typeID);

      static DynAny_Handler* getInstance ();

    private:
      DynAny_Handler ();

      IDL::traits<CORBA::ORB>::ref_type orb_ {};

      IDL::traits<DynamicAny::DynAnyFactory>::ref_type daf_ {};

      std::map <std::string, IDL::traits<CORBA::TypeCode>::ref_type> typecode_map_;
    };
  }
}

#define DYNANY_HANDLER ::DAnCEX11::Config_Handlers::DynAny_Handler::getInstance()

#endif /* DANCEX11_CONFIG_HANDLERS_ANY_HANDLER_H*/
