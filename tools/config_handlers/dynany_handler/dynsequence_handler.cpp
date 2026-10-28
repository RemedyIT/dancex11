/**
 * @file    dynsequence_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dynsequence_handler.h"
#include "dynany_handler.h"
#include "dyncharsequence.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"

#include "ace/Null_Mutex.h"

#include "tao/x11/anytypecode/typecode.h"

#include <algorithm>
#include <limits>
#include <string>

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
      ::DAnCE::Config_Handlers::SequenceType const* sequence_type = type.sequence_if_set ();
      if (!sequence_type)
        throw Config_Error ("", "Sequence type description required");

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

      switch (sequence_type->elementType ().kind ().integral ())
        {
          // ========== BASIC TYPES
        case TCKind::tk_null:
        case TCKind::tk_void:
        case TCKind::tk_objref:
        case TCKind::tk_fixed:
        case TCKind::tk_component:
        case TCKind::tk_home:

          DANCEX11_LOG_ERROR ("Null and void element types are not supported in sequences");
          throw Config_Error ("", "Null or void element types are not supported in sequences");

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
          if (value.element ().size () > std::numeric_limits<uint32_t>::max ())
            throw Config_Error ("", "Sequence value exceeds the supported length");

          try
          {
            dynseq.resize (value.element ().size ());
            retval->set_length (static_cast<uint32_t>(value.element ().size ()));

            for (DataValue const& element : value.element ())
            {
              IDL::traits<DynamicAny::DynAny>::ref_type dynany (
                DYNANY_HANDLER->extract_into_dynany (sequence_type->elementType (), element));
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

        case TCKind::tk_char:
        case TCKind::tk_wchar:
          {
            std::string const characters = extract_char_sequence (value);
            if (characters.size () > std::numeric_limits<uint32_t>::max ())
              throw Config_Error ("", "Character sequence exceeds the supported length");

            if (sequence_type->elementType ().kind ().integral () == TCKind::tk_wchar &&
                std::any_of (characters.begin (), characters.end (), [] (unsigned char c) { return c > 0x7f; }))
              throw Config_Error ("", "Non-ASCII wchar values are not supported");

            try
            {
              retval->set_length (static_cast<uint32_t> (characters.size ()));
              dynseq.resize (characters.size ());
              for (size_t i {}; i < characters.size (); ++i)
              {
                IDL::traits<DynamicAny::DynAny>::ref_type character =
                  DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (
                    sequence_type->elementType ().kind ().integral () == TCKind::tk_char
                      ? CORBA::_tc_char : CORBA::_tc_wchar);
                if (sequence_type->elementType ().kind ().integral () == TCKind::tk_char)
                  character->insert_char (characters[i]);
                else
                  character->insert_wchar (static_cast<wchar_t> (static_cast<unsigned char> (characters[i])));
                dynseq[i] = character->copy ();
              }
              retval->set_elements_as_dyn_any (dynseq);
              return retval;
            }
            catch (const DynamicAny::DynAny::InvalidValue&)
            {
              throw Config_Error ("", "Invalid value whilst populating the character sequence.");
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

          DANCEX11_LOG_ERROR ("DynSequence_Handler::extract_into_dynany - Type not supported");
          throw Config_Error ("", "Type not supported");
        }

      return retval;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynSequence_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynSequence_Handler::create_typecode");

      ::DAnCE::Config_Handlers::SequenceType const* sequence_type = type.sequence_if_set ();
      if (!sequence_type)
        {
        DANCEX11_LOG_ERROR ("ERROR: Sequence type description required");
        throw Config_Error ("", "Expected <sequence> element, incorrect tc_kind.");
        }

      IDL::traits<CORBA::TypeCode>::ref_type etc =
        DYNANY_HANDLER->create_typecode (sequence_type->elementType ());

      uint32_t bound {};
      ::XMLSchema::unsignedInt const* sequence_bound = sequence_type->bound_if_set ();
      if (sequence_bound)
        bound = *sequence_bound;

      IDL::traits<CORBA::TypeCode>::ref_type tc = DYNANY_HANDLER->orb ()->create_sequence_tc (bound, etc);

      return tc;
    }
  }
}



