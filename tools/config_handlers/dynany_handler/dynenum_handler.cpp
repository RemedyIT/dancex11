/**
 * @file    dynenum_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dynenum_handler.h"
#include "dynany_handler.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"

#include "tao/x11/anytypecode/typecode.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    using DAnCE::Config_Handlers::EnumType;

    IDL::traits<DynamicAny::DynAny>::ref_type
    DynEnum_Handler::extract_into_dynany (const DataType &type,
                                          const DataValue &value,
                                          IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynEnum_Handler::extract_into_dynany");
      ::DAnCE::Config_Handlers::EnumType const* enum_type = type._cxx_enum_if_set ();
      if (!enum_type)
        throw Config_Error ("", "Enum type description required");

      ::DAnCE::Config_Handlers::DataValue::enum_container_type const& enum_values = value._cxx_enum ();
      if (enum_values.empty ())
        throw Config_Error (enum_type->typeId (), "Enum value required");

      try
      {
        IDL::traits<CORBA::TypeCode>::ref_type tc;

        if (req_tc)
          tc = req_tc;
        else
          tc = DynEnum_Handler::create_typecode (type);

        // Make the actual DynEnum
        IDL::traits<DynamicAny::DynAny>::ref_type temp =
          DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
        IDL::traits<DynamicAny::DynEnum>::ref_type retval = IDL::traits<DynamicAny::DynEnum>::narrow (
          temp);

        retval->set_as_string (enum_values.front ());

        return retval;
      }
      catch (const DynamicAny::DynAny::InvalidValue &)
      {
        DANCEX11_LOG_ERROR ("Invalid value provided in XML when trying to "
                            "initialize an instance of enumerated type " << enum_type->typeId ());
        throw Config_Error (enum_type->typeId (), "Invalid value provided in XML");
      }
      catch (const Config_Error &ex)
      {
        DANCEX11_LOG_ERROR ("DynEnum_Handler caught Config_Error");
        throw Config_Error (ex.add_name (enum_type->typeId ()), ex.get_error ());
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("DynEnum_Handler caught unknown exception");
        throw Config_Error (enum_type->typeId (), "Unknown exception");
      }
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynEnum_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynEnum_Handler::create_typecode");

      ::DAnCE::Config_Handlers::EnumType const* enum_type = type._cxx_enum_if_set ();
      if (!enum_type)
      {
        DANCEX11_LOG_ERROR ("ERROR: Enum type description required");
        throw Config_Error ("", "Did not find expected enum type description, tk_kind may be wrong.");
      }

      // Construct TypeCode for the enum
      CORBA::EnumMemberSeq members;

      for (::XMLSchema::string<char> const& member : enum_type->member ())
        members.push_back (member);

      IDL::traits<CORBA::TypeCode>::ref_type tc = DYNANY_HANDLER->orb ()->create_enum_tc (
        enum_type->typeId (),
        enum_type->name (),
        members);

      DYNANY_HANDLER->register_typecode (enum_type->typeId (), tc);

      return tc;
    }

  }
}


