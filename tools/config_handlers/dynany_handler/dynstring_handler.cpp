/**
 * @file    dynstring_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dynstring_handler.h"
#include "dynany_handler.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"
#include "tao/x11/anytypecode/typecode.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    IDL::traits<DynamicAny::DynAny>::ref_type
    DynString_Handler::extract_into_dynany (const DataType &type,
                                            const DataValue &value,
                                            IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynString_Handler::extract_into_dynany");

      IDL::traits<CORBA::TypeCode>::ref_type tc;
      if (req_tc)
        tc = req_tc;
      else
        tc = DynString_Handler::create_typecode (type);

      // Make the actual DynAny
      IDL::traits<DynamicAny::DynAny>::ref_type retval =
        DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);

      DANCEX11_LOG_TRACE("DynString_Handler::extract_into_dynany - created DynAny: " << retval);

      std::string s;
      if (!value.string ().empty ())
      {
        s = value.string ().front ();
      }

      DANCEX11_LOG_TRACE("DynString_Handler::extract_into_dynany - inserting string: [" << s << "]");

      retval->insert_string (s);

      return retval;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynString_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynString_Handler::create_typecode");

      if (type.kind ().integral () != TCKind::tk_string)
      {
        DANCEX11_LOG_ERROR ("ERROR: String type description required");
        throw Config_Error ("", "Expected <string> element, incorrect tc_kind.");
      }

      uint32_t bound {} ;
      ::DAnCE::Config_Handlers::BoundedStringType const* bounded_string = type.boundedString_if_set ();
      if (bounded_string)
      {
        if (bounded_string->bound ().size () > 1)
          throw Config_Error ("", "A bounded string type can have at most one bound");

        if (!bounded_string->bound ().empty ())
        {
          bound = bounded_string->bound ().front ();
        }
      }

      IDL::traits<CORBA::TypeCode>::ref_type tc = DYNANY_HANDLER->orb ()->create_string_tc (bound);

      return tc;
    }
  }
}



