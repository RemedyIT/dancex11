/**
 * @file    dynany_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dynany_handler.h"
#include "dynenum_handler.h"
#include "dynsequence_handler.h"
#include "dynstruct_handler.h"
#include "dynalias_handler.h"
#include "dynarray_handler.h"
#include "dynstring_handler.h"
#include "Basic_Deployment_Data.hpp"
#include "common.h"

// to force linking with typecodefactory
#include "tao/x11/typecodefactory/typecodefactory.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    using DAnCE::Config_Handlers::DataValue;
    using DAnCE::Config_Handlers::DataType;
    using DAnCE::Config_Handlers::TCKind;

    DynAny_Handler* DynAny_Handler::getInstance ()
    {
      static DynAny_Handler instance_;

      return std::addressof(instance_);
    }

    DynAny_Handler::DynAny_Handler ()
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::constructor");

      orb_ = DAnCEX11::State::instance ()->orb ();

      IDL::traits<CORBA::Object>::ref_type temp = orb_->resolve_initial_references (
        "DynAnyFactory");

      daf_ = IDL::traits< DynamicAny::DynAnyFactory>::narrow  (temp);

      if (!daf_)
      {
        DANCEX11_LOG_ERROR ("Unable to narrow Dynamic Any factory");
        throw Config_Error ("", "Unable to narrow DynAny factory");
      }
    }

    DynAny_Handler::~DynAny_Handler ()
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::destructor");
      this->orb_.reset ();
      this->daf_.reset ();
      this->typecode_map_.clear ();
      DANCEX11_LOG_TRACE("DynAny_Handler::destructor - end");
    }

    IDL::traits<CORBA::ORB>::ref_type
    DynAny_Handler::orb ()
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::orb");

      return orb_;
    }

    IDL::traits< DynamicAny::DynAnyFactory>::ref_type
    DynAny_Handler::daf ()
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::daf");

      return daf_;
    }


    IDL::traits<DynamicAny::DynAny>::ref_type
    DynAny_Handler::extract_into_dynany (const DataType &type,
                                         const DataValue &value,
                                          IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::extract_into_dynany");

      IDL::traits<DynamicAny::DynAny>::ref_type retval;

      if (req_tc)
      {
        retval = this->daf_->create_dyn_any_from_type_code (req_tc);
      }

      TCKind::Value const tkind = type.kind ().integral ();

      if ((value.count_short () && tkind != TCKind::tk_short_l) ||
          (value.count_long () && tkind != TCKind::tk_long_l) ||
          (value.count_ushort () && tkind != TCKind::tk_ushort_l ) ||
          (value.count_ulong () && tkind != TCKind::tk_ulong_l) ||
          (value.count_float () && tkind != TCKind::tk_float_l) ||
          (value.count_double () && tkind != TCKind::tk_double_l) ||
          (value.count_boolean () && tkind != TCKind::tk_boolean_l) ||
          (value.count_octet () && tkind != TCKind::tk_octet_l) ||
          (value.count_longlong () && tkind != TCKind::tk_longlong_l) ||
          (value.count_ulonglong () && tkind != TCKind::tk_ulonglong_l) ||
          (value.count_string () && !(tkind == TCKind::tk_string_l ||
                                      tkind == TCKind::tk_char_l ||
                                      tkind == TCKind::tk_wchar_l)))
        {
          throw Config_Error ("", "Wrong value type for data type");
        }

      switch (tkind)
        {
          // ========== BASIC TYPES
        case TCKind::tk_null_l:
        case TCKind::tk_void_l:
          DANCEX11_LOG_ERROR ("DynAny_Handler::extract_into_dynany - Don't know how to handle null or void types");
          throw Config_Error ("","Null or void types not supported");
          break;

        case TCKind::tk_short_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_short);
          }
          int16_t s {};
          if (value.count_short ())
          {
            s = *value.begin_short ();
          }
          retval->insert_short (s);
        }
        break;
        case TCKind::tk_long_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_long);
          }
          int32_t l {};
          if (value.count_long ())
          {
            l = *value.begin_long ();
          }
          retval->insert_long (l);
        }
        break;

        case TCKind::tk_ushort_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ushort);
          }
          uint16_t us {};
          if (value.count_ushort ())
          {
            us = *value.begin_ushort ();
          }
          retval->insert_ushort (us);
        }
        break;

        case TCKind::tk_ulong_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ulong);
          }
          uint32_t ul {};
          if (value.count_ulong ())
          {
            ul = *value.begin_ulong ();
          }
          retval->insert_ulong (ul);
        }
        break;

        case TCKind::tk_float_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_float);
          }
          float f {};
          if (value.count_float ())
          {
            f = *value.begin_float ();
          }
          retval->insert_float (f);
        }
        break;

        case TCKind::tk_double_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_double);
          }
          double d {};
          if (value.count_double ())
          {
            d = *value.begin_double ();
          }
          retval->insert_double (d);
        }
        break;

        case TCKind::tk_boolean_l:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_boolean);
              }
            bool b { false };
            if (value.count_boolean ())
              {
                b = *value.begin_boolean ();
              }
            retval->insert_boolean (b);
          }
          break;

        case TCKind::tk_octet_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_octet);
          }
          uint8_t o {};
          if (value.count_octet ())
          {
            o = *(value.begin_octet ());
          }
          retval->insert_octet (o);
        }
          break;

        case TCKind::tk_longlong_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_longlong);
          }
          int64_t ll {};
          if (value.count_longlong ())
          {
            ll = *value.begin_longlong ();
          }
          retval->insert_longlong (ll);
        }
        break;

        case TCKind::tk_ulonglong_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ulonglong);
          }
        uint64_t ull = {};
        if (value.count_ulonglong ())
          {
            ull = *value.begin_ulonglong ();
          }
          retval->insert_ulonglong (ull);
        }
        break;

        case TCKind::tk_string_l:
          return DynString_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_char_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_char);
          }
          char c = 0;
          if (value.count_string ())
          {
            c = *value.begin_string ()->c_str();
          }
          retval->insert_char (c);
        }
        break;

        case TCKind::tk_wchar_l:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_wchar);
          }
          wchar_t wc{};
          if (value.count_string ())
          {
            wc = *value.begin_string ()->c_str();
          }
          retval->insert_wchar (wc);
        }
        break;

        case TCKind::tk_enum_l:
          return DynEnum_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_sequence_l:
          return DynSequence_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_struct_l:
          return DynStruct_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_alias_l:
          return DynAlias_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_array_l:
          return DynArray_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_longdouble_l:
          // Not supported since a longdouble is defined in the xsd as double.
          // We are then creating a long double initialized with a regular
          // double. This is a very tricky conversion and doesn't work in
          // combination with certain (versions of) compilers.
        case TCKind::tk_wstring_l:
        case TCKind::tk_fixed_l:
        case TCKind::tk_any_l:
        case TCKind::tk_TypeCode_l:
        case TCKind::tk_Principal_l:
        case TCKind::tk_objref_l:
        case TCKind::tk_union_l:
        case TCKind::tk_except_l:
        case TCKind::tk_value_l:
        case TCKind::tk_value_box_l:
        case TCKind::tk_native_l:
        case TCKind::tk_abstract_interface_l:
        case TCKind::tk_local_interface_l:
        case TCKind::tk_component_l:
        case TCKind::tk_home_l:
        case TCKind::tk_event_l:
          DANCEX11_LOG_ERROR ("DynAny_Handler::extract_into_dynany - Type not supported");
          throw Config_Error ("", "Type not supported");
        }

      return retval;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynAny_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::create_typecode");

      switch (type.kind ().integral ())
        {
          // ========== BASIC TYPES
        case TCKind::tk_null_l:
        case TCKind::tk_void_l:
          DANCEX11_LOG_DEBUG ("I don't know how to handle null or void typesn");
          throw Config_Error ("","Null or void types not supported");
          break;

        case TCKind::tk_short_l:
          return CORBA::_tc_short;

        case TCKind::tk_long_l:
          return CORBA::_tc_long;

        case TCKind::tk_ushort_l:
          return CORBA::_tc_ushort;

        case TCKind::tk_ulong_l:
          return CORBA::_tc_ulong;

        case TCKind::tk_float_l:
          return CORBA::_tc_float;

        case TCKind::tk_double_l:
          return CORBA::_tc_double;

        case TCKind::tk_boolean_l:
          return CORBA::_tc_boolean;

        case TCKind::tk_char_l:
          return CORBA::_tc_char;

        case TCKind::tk_octet_l:
          return CORBA::_tc_octet;

        case TCKind::tk_string_l:
          return DynString_Handler::create_typecode (type);

        case TCKind::tk_longlong_l:
          return CORBA::_tc_longlong;

        case TCKind::tk_ulonglong_l:
          return CORBA::_tc_ulonglong;

        case TCKind::tk_longdouble_l:
          // Disabled since a longdouble is defined in the xsd as double.
          // We are then creating a long double initialized from a regular
          // double. This is a very tricky conversion and doesn't work in
          // combination with certain (versions of) compilers.
          break;

        case TCKind::tk_wchar_l:
          return CORBA::_tc_wchar;

        case TCKind::tk_wstring_l:
          break;

        case TCKind::tk_enum_l:
          return DynEnum_Handler::create_typecode (type);

        case TCKind::tk_struct_l:
          return DynStruct_Handler::create_typecode (type);

        case TCKind::tk_sequence_l:
          return DynSequence_Handler::create_typecode (type);

        case TCKind::tk_alias_l:
          return DynAlias_Handler::create_typecode (type);

        case TCKind::tk_array_l:
          return DynArray_Handler::create_typecode (type);

        case TCKind::tk_fixed_l:
        case TCKind::tk_any_l:
        case TCKind::tk_TypeCode_l:
        case TCKind::tk_Principal_l:
        case TCKind::tk_objref_l:
        case TCKind::tk_union_l:
        case TCKind::tk_except_l:
        case TCKind::tk_value_l:
        case TCKind::tk_value_box_l:
        case TCKind::tk_native_l:
        case TCKind::tk_abstract_interface_l:
        case TCKind::tk_local_interface_l:
        case TCKind::tk_component_l:
        case TCKind::tk_home_l:
        case TCKind::tk_event_l:
          DANCEX11_LOG_DEBUG("DynAny_Handler::create_typecode - Type not supported");
          throw Config_Error ("", "Type not supported");
        }

      return {};
    }

    void
    DynAny_Handler::register_typecode (
      const std::string& typeID,
       IDL::traits<CORBA::TypeCode>::ref_type tc)
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::register_typecode");

      this->typecode_map_[typeID] = tc;
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynAny_Handler::get_typecode (const std::string& typeID)
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::get_typecode");

      try
      {
        return this->typecode_map_ [typeID];
      }
      catch (...)
      {
        return {};
      }
    }
  }
}
