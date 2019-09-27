/**
 * @file    dynstring_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
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
      if (value.count_string ())
      {
        s = (*value.begin_string ());
      }

      DANCEX11_LOG_TRACE("DynString_Handler::extract_into_dynany - inserting string: [" << s << "]");

      retval->insert_string (s);

      return retval;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynString_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynString_Handler::create_typecode");

      if (type.kind ().integral () != TCKind::tk_string_l)
      {
        DANCEX11_LOG_ERROR ("ERROR: String type description required");
        throw Config_Error ("","Expected <string> element, incorrect tc_kind.");
      }

      uint32_t bound {} ;
      if (type.boundedString_p ())
      {
        if (type.boundedString ().count_bound ())
        {
          bound = *type.boundedString ().begin_bound ();
        }
      }

      IDL::traits<CORBA::TypeCode>::ref_type tc =
        DYNANY_HANDLER->orb ()->create_string_tc (bound);

      return tc;
    }
  }
}



