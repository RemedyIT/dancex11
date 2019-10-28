/**
 * @file    datatype_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "datatype_handler.h"
#include "tao/x11/anytypecode/typecode.h"
#include "tao/x11/anytypecode/typecode_constants.h"
#include "dancex11/logger/log.h"
namespace DAnCEX11
{
  namespace Config_Handlers
  {
    IDL::traits<CORBA::TypeCode>::ref_type
    convert_datatype (const DAnCE::Config_Handlers::DataType& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_datatype");

      IDL::traits<CORBA::TypeCode>::ref_type dest;

      switch (src.kind().integral ())
      {
      case DAnCE::Config_Handlers::TCKind::tk_null_l:
        dest = CORBA::_tc_null;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_short_l:
        dest = CORBA::_tc_short;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_long_l:
        dest = CORBA::_tc_long;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_ushort_l:
        dest = CORBA::_tc_ushort;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_ulong_l:
        dest = CORBA::_tc_ulong;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_float_l:
        dest = CORBA::_tc_float;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_double_l:
        dest = CORBA::_tc_double;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_boolean_l:
        dest = CORBA::_tc_boolean;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_char_l:
        dest = CORBA::_tc_char;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_octet_l:
        dest = CORBA::_tc_octet;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_string_l:
        dest = CORBA::_tc_string;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_longlong_l:
        dest = CORBA::_tc_longlong;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_ulonglong_l:
        dest = CORBA::_tc_ulonglong;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_longdouble_l:
        dest = CORBA::_tc_longdouble;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_wchar_l:
        dest = CORBA::_tc_wchar;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_wstring_l:
        dest = CORBA::_tc_wstring;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_any_l:
        dest = CORBA::_tc_any;
        break;
      case DAnCE::Config_Handlers::TCKind::tk_TypeCode_l:
        dest = CORBA::_tc_TypeCode;
        break;
      default:
        DANCEX11_LOG_ERROR ("DAnCEX11::Config_Handlers::convert_datatype, invalid typecode in any");
        throw 1;
      }

      return dest;
    }
  }
}
