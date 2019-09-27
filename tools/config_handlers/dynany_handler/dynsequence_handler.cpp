/**
 * @file    dynsequence_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dynsequence_handler.h"
#include "dynany_handler.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"

#include "ace/Null_Mutex.h"

#include "tao/x11/anytypecode/typecode.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    IDL::traits<DynamicAny::DynAny>::ref_type
    DynSequence_Handler::extract_into_dynany (const DataType &type,
                                              const DataValue &value,
                                              IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynSequence_Handler::extract_into_dynany");

      IDL::traits<CORBA::TypeCode>::ref_type tc;
      if (req_tc)
        tc = req_tc;
      else
        tc = DynSequence_Handler::create_typecode (type);

      // Make the actual DynSequence
      IDL::traits<DynamicAny::DynAny>::ref_type temp =
        DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
      IDL::traits<DynamicAny::DynSequence>::ref_type retval =
          IDL::traits<DynamicAny::DynSequence>::narrow (temp);

      DynamicAny::DynAnySeq dynseq;
      uint32_t pos {};

      switch (type.sequence ().elementType ().kind ().integral ())
        {
          // ========== BASIC TYPES
        case TCKind::tk_null_l:
        case TCKind::tk_void_l:
        case TCKind::tk_objref_l:
        case TCKind::tk_fixed_l:
        case TCKind::tk_component_l:
        case TCKind::tk_home_l:

          DANCEX11_LOG_ERROR ("I don't know how to handle null or void types");
          throw 1;

        case TCKind::tk_short_l:
        case TCKind::tk_long_l:
        case TCKind::tk_ushort_l:
        case TCKind::tk_ulong_l:
        case TCKind::tk_float_l:
        case TCKind::tk_double_l:
        case TCKind::tk_boolean_l:
        case TCKind::tk_octet_l:
        case TCKind::tk_string_l:
        case TCKind::tk_longlong_l:
        case TCKind::tk_ulonglong_l:
        case TCKind::tk_longdouble_l:
        case TCKind::tk_wstring_l:
        case TCKind::tk_enum_l:
        case TCKind::tk_struct_l:
        case TCKind::tk_sequence_l:
        case TCKind::tk_array_l:
          try
          {
            dynseq.resize (value.count_element ());
            retval->set_length (static_cast<uint32_t>(value.count_element ()));

            for (DataValue::element_const_iterator i = value.begin_element ();
                 i != value.end_element ();
                 ++i)
            {
              IDL::traits<DynamicAny::DynAny>::ref_type dynany
                (DYNANY_HANDLER->extract_into_dynany (type.sequence ().elementType (),
                                                      *i));
              dynseq[pos++] = dynany->copy ();
            }

            retval->set_elements_as_dyn_any (dynseq);
            return retval;
          }
          catch (const DynamicAny::DynAny::InvalidValue&)
          {
            DANCEX11_LOG_ERROR ("Invalid value provided in XML when trying to "
                                "populate element " << pos << " of a sequence");
            throw Config_Error ("", "Invalid value whilst populating the sequence.");
          }

        case TCKind::tk_char_l:
        case TCKind::tk_wchar_l:
          // special case where value iterator multiplicity should be one, and should
          // represent a string, each character of which becomes a element of the sequence.

        case TCKind::tk_any_l:
        case TCKind::tk_TypeCode_l:
        case TCKind::tk_Principal_l:
        case TCKind::tk_union_l:
        case TCKind::tk_alias_l:
        case TCKind::tk_except_l:
        case TCKind::tk_value_l:
        case TCKind::tk_value_box_l:
        case TCKind::tk_native_l:
        case TCKind::tk_abstract_interface_l:
        case TCKind::tk_local_interface_l:
        case TCKind::tk_event_l:
          // Special case where element association in datavalue contains another datavalue.

          DANCEX11_LOG_ERROR ("DynSequence_Handler::extract_into_dynany - Type not supported");
          throw Config_Error ("", "Type not supported");
        }

      return retval;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynSequence_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynSequence_Handler::create_typecode");

      if (!type.sequence_p ())
        {
        DANCEX11_LOG_ERROR ("ERROR: Sequence type description required");
          throw Config_Error ("",
                              "Expected <sequence> element, incorrect tc_kind.");
        }

      IDL::traits<CORBA::TypeCode>::ref_type etc =
        DYNANY_HANDLER->create_typecode (type.sequence ().elementType ());

      uint32_t bound {};
      if (type.sequence ().bound_p ())
        bound = type.sequence ().bound ();

      IDL::traits<CORBA::TypeCode>::ref_type tc =
        DYNANY_HANDLER->orb ()->create_sequence_tc (bound, etc);

      return tc;
    }
  }
}



