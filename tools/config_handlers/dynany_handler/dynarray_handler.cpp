/**
 * @file    dynarray_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dynarray_handler.h"
#include "dyncharsequence.h"
#include "dynany_handler.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"
#include "tao/x11/anytypecode/typecode.h"

#include <algorithm>
#include <string>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    IDL::traits<DynamicAny::DynAny>::ref_type
    DynArray_Handler::extract_into_dynany (const DataType &type,
                                           const DataValue &value,
                                          IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynArray_Handler::extract_into_dynany");
      ::DAnCE::Config_Handlers::ArrayType const* array_type = type.array_if_set ();
      if (!array_type)
        throw Config_Error ("", "Array type description required");

      if (value.element ().size () != array_type->length () &&
          array_type->elementType ().kind ().integral () != TCKind::tk_char &&
          array_type->elementType ().kind ().integral () != TCKind::tk_wchar)
        throw Config_Error ("", "Array value does not match the declared length");

      IDL::traits<CORBA::TypeCode>::ref_type tc;
      if (req_tc)
        tc = req_tc;
      else
        tc = DynArray_Handler::create_typecode (type);

      // Make the actual DynArray
      IDL::traits<DynamicAny::DynAny>::ref_type temp =
        DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
      IDL::traits<DynamicAny::DynArray>::ref_type retval =
          IDL::traits<DynamicAny::DynArray>::narrow (temp);

      DynamicAny::DynAnySeq dynseq;
      uint32_t pos {};

      switch (array_type->elementType ().kind ().integral ())
        {
          // ========== BASIC TYPES
        case TCKind::tk_null:
        case TCKind::tk_void:
        case TCKind::tk_objref:
        case TCKind::tk_fixed:
        case TCKind::tk_component:
        case TCKind::tk_home:

          DANCEX11_LOG_ERROR ("Null and void element types are not supported in arrays");
          throw Config_Error ("", "Null or void element types are not supported in arrays");

        case TCKind::tk_short:
        case TCKind::tk_long:
        case TCKind::tk_ushort:
        case TCKind::tk_ulong:
        case TCKind::tk_float:
        case TCKind::tk_double:
        case TCKind::tk_boolean:
        case TCKind::tk_octet:
        case TCKind::tk_string:
        case TCKind::tk_longlong:
        case TCKind::tk_ulonglong:
        case TCKind::tk_longdouble:
        case TCKind::tk_wstring:
        case TCKind::tk_enum:
        case TCKind::tk_struct:
        case TCKind::tk_sequence:
        case TCKind::tk_array:
          try
          {
            for (DataValue const& element : value.element ())
            {
              IDL::traits<DynamicAny::DynAny>::ref_type dynany (
                DYNANY_HANDLER->extract_into_dynany (array_type->elementType (), element));
              dynseq.push_back (dynany->copy ());
              ++pos;
            }

            retval->set_elements_as_dyn_any (dynseq);
            return retval;
          }
          catch (const DynamicAny::DynAny::InvalidValue&)
          {
             DANCEX11_LOG_ERROR ("Invalid value provided in XML when trying to "
                                 "populate " << pos << " with element of an array");
              throw Config_Error ("", "Invalid value while populating the array.");
            }

        case TCKind::tk_char:
        case TCKind::tk_wchar:
          {
            std::string const characters = extract_char_sequence (value);
            uint32_t const expected_length = array_type->length ();
            if (characters.size () != expected_length)
              throw Config_Error ("", "Character array value does not match the declared length");
            if (array_type->elementType ().kind ().integral () == TCKind::tk_wchar &&
                std::any_of (characters.begin (), characters.end (), [] (unsigned char c) { return c > 0x7f; }))
              throw Config_Error ("", "Non-ASCII wchar values are not supported");

            try
            {
              for (char c : characters)
              {
                IDL::traits<DynamicAny::DynAny>::ref_type character =
                  DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (
                    array_type->elementType ().kind ().integral () == TCKind::tk_char
                      ? CORBA::_tc_char : CORBA::_tc_wchar);
                if (array_type->elementType ().kind ().integral () == TCKind::tk_char)
                  character->insert_char (c);
                else
                  character->insert_wchar (static_cast<wchar_t> (static_cast<unsigned char> (c)));
                dynseq.push_back (character->copy ());
              }

              retval->set_elements_as_dyn_any (dynseq);
              return retval;
            }
            catch (const DynamicAny::DynAny::InvalidValue&)
            {
              throw Config_Error ("", "Invalid value while populating the character array.");
            }
          }

        case TCKind::tk_any:
        case TCKind::tk_TypeCode:
        case TCKind::tk_Principal:
        case TCKind::tk_union:
        case TCKind::tk_alias:
        case TCKind::tk_except:
        case TCKind::tk_value:
        case TCKind::tk_value_box:
        case TCKind::tk_native:
        case TCKind::tk_abstract_interface:
        case TCKind::tk_local_interface:
        case TCKind::tk_event:
          // Special case where element association in datavalue contains another datavalue.
          DANCEX11_LOG_ERROR("DynArray_Handler::extract_into_dynany - Type not supported");
          throw Config_Error ("", "Type not supported");
        }

      return retval;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynArray_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynArray_Handler::create_typecode");

      ::DAnCE::Config_Handlers::ArrayType const* array_type = type.array_if_set ();
      if (!array_type)
        {
        DANCEX11_LOG_ERROR("ERROR: Array type description required");
        throw Config_Error ("", "Expected <array> element, incorrect tc_kind.");
        }

      IDL::traits<CORBA::TypeCode>::ref_type etc =
        DYNANY_HANDLER->create_typecode (array_type->elementType ());

      uint32_t const length =  array_type->length ();

      IDL::traits<CORBA::TypeCode>::ref_type tc = DYNANY_HANDLER->orb ()->create_array_tc (length, etc);

      return tc;
    }
  }
}



