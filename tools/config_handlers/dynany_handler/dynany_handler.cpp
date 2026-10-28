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

      daf_ = IDL::traits<DynamicAny::DynAnyFactory>::narrow  (temp);

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

    IDL::traits<DynamicAny::DynAnyFactory>::ref_type
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

      if (value._cxx_short ().size () > 1 || value._cxx_long ().size () > 1 ||
          value.ushort ().size () > 1 || value.ulong ().size () > 1 ||
          value._cxx_float ().size () > 1 || value._cxx_double ().size () > 1 ||
          value.boolean ().size () > 1 || value.octet ().size () > 1 ||
          value.longlong ().size () > 1 || value.ulonglong ().size () > 1 ||
          value.longdouble ().size () > 1 || value._cxx_enum ().size () > 1 ||
          value.string ().size () > 1)
      {
        throw Config_Error ("", "Multiple scalar values provided for data type");
      }

      if (tkind != TCKind::tk_alias &&
          ((!value._cxx_short ().empty () && tkind != TCKind::tk_short) ||
          (!value._cxx_long ().empty () && tkind != TCKind::tk_long) ||
          (!value.ushort ().empty () && tkind != TCKind::tk_ushort ) ||
          (!value.ulong ().empty () && tkind != TCKind::tk_ulong) ||
          (!value._cxx_float ().empty () && tkind != TCKind::tk_float) ||
          (!value._cxx_double ().empty () && tkind != TCKind::tk_double) ||
          (!value.boolean ().empty () && tkind != TCKind::tk_boolean) ||
          (!value.octet ().empty () && tkind != TCKind::tk_octet) ||
          (!value.longlong ().empty () && tkind != TCKind::tk_longlong) ||
          (!value.ulonglong ().empty () && tkind != TCKind::tk_ulonglong) ||
          (!value.longdouble ().empty () && tkind != TCKind::tk_longdouble) ||
          (!value._cxx_enum ().empty () && tkind != TCKind::tk_enum) ||
          (!value.element ().empty () && tkind != TCKind::tk_sequence &&
                                          tkind != TCKind::tk_array) ||
          (!value.member ().empty () && tkind != TCKind::tk_struct) ||
          (!value.string ().empty () && !(tkind == TCKind::tk_string ||
                                      tkind == TCKind::tk_char ||
                                      tkind == TCKind::tk_wchar))))
        {
          throw Config_Error ("", "Wrong value type for data type");
        }

      switch (tkind)
        {
          // ========== BASIC TYPES
        case TCKind::tk_null:
        case TCKind::tk_void:
          DANCEX11_LOG_ERROR ("DynAny_Handler::extract_into_dynany - Don't know how to handle null or void types");
          throw Config_Error ("", "Null or void types not supported");
          break;
        case TCKind::tk_short:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_short);
          }
          int16_t s {};
          if (!value._cxx_short ().empty ())
          {
            s = value._cxx_short ().front ();
          }
          retval->insert_short (s);
        }
        break;
        case TCKind::tk_long:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_long);
          }
          int32_t l {};
          if (!value._cxx_long ().empty ())
          {
            l = value._cxx_long ().front ();
          }
          retval->insert_long (l);
        }
        break;
        case TCKind::tk_ushort:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ushort);
          }
          uint16_t us {};
          if (!value.ushort ().empty ())
          {
            us = value.ushort ().front ();
          }
          retval->insert_ushort (us);
        }
        break;
        case TCKind::tk_ulong:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ulong);
          }
          uint32_t ul {};
          if (!value.ulong ().empty ())
          {
            ul = value.ulong ().front ();
          }
          retval->insert_ulong (ul);
        }
        break;
        case TCKind::tk_float:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_float);
          }
          float f {};
          if (!value._cxx_float ().empty ())
          {
            f = value._cxx_float ().front ();
          }
          retval->insert_float (f);
        }
        break;
        case TCKind::tk_double:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_double);
          }
          double d {};
          if (!value._cxx_double ().empty ())
          {
            d = value._cxx_double ().front ();
          }
          retval->insert_double (d);
        }
        break;
        case TCKind::tk_boolean:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_boolean);
              }
            bool b { false };
            if (!value.boolean ().empty ())
              {
                b = value.boolean ().front ();
              }
            retval->insert_boolean (b);
          }
          break;
        case TCKind::tk_octet:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_octet);
          }
          uint8_t o {};
          if (!value.octet ().empty ())
          {
            o = value.octet ().front ();
          }
          retval->insert_octet (o);
        }
          break;
        case TCKind::tk_longlong:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_longlong);
          }
          int64_t ll {};
          if (!value.longlong ().empty ())
          {
            ll = value.longlong ().front ();
          }
          retval->insert_longlong (ll);
        }
        break;
        case TCKind::tk_ulonglong:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ulonglong);
          }
        uint64_t ull {};
        if (!value.ulonglong ().empty ())
          {
            ull = value.ulonglong ().front ();
          }
          retval->insert_ulonglong (ull);
        }
        break;
        case TCKind::tk_string:
          return DynString_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_char:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_char);
          }
          char c {};
          if (!value.string ().empty ())
          {
            ::XMLSchema::string<char> const& character_value = value.string ().front ();
            if (character_value.size () != 1)
              throw Config_Error ("", "A char value must contain exactly one character");
            c = character_value.front ();
          }
          retval->insert_char (c);
        }
        break;
        case TCKind::tk_wchar:
        {
          if (!req_tc)
          {
            retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_wchar);
          }
          wchar_t wc{};
          if (!value.string ().empty ())
          {
            ::XMLSchema::string<char> const& character_value = value.string ().front ();
            if (character_value.size () != 1 ||
                static_cast<unsigned char> (character_value.front ()) > 0x7f)
            {
              throw Config_Error ("", "Non-ASCII wchar values are not supported");
            }
            wc = character_value.front ();
          }
          retval->insert_wchar (wc);
        }
        break;
        case TCKind::tk_enum:
          return DynEnum_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_sequence:
          return DynSequence_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_struct:
          return DynStruct_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_alias:
          return DynAlias_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_array:
          return DynArray_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_longdouble:
          // Not supported since a longdouble is defined in the xsd as double.
          // We are then creating a long double initialized with a regular
          // double. This is a very tricky conversion and doesn't work in
          // combination with certain (versions of) compilers.
        case TCKind::tk_wstring:
        case TCKind::tk_fixed:
        case TCKind::tk_any:
        case TCKind::tk_TypeCode:
        case TCKind::tk_Principal:
        case TCKind::tk_objref:
        case TCKind::tk_union:
        case TCKind::tk_except:
        case TCKind::tk_value:
        case TCKind::tk_value_box:
        case TCKind::tk_native:
        case TCKind::tk_abstract_interface:
        case TCKind::tk_local_interface:
        case TCKind::tk_component:
        case TCKind::tk_home:
        case TCKind::tk_event:
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
        case TCKind::tk_null:
          return CORBA::_tc_null;
        case TCKind::tk_void:
          return CORBA::_tc_void;

        case TCKind::tk_short:
          return CORBA::_tc_short;

        case TCKind::tk_long:
          return CORBA::_tc_long;

        case TCKind::tk_ushort:
          return CORBA::_tc_ushort;

        case TCKind::tk_ulong:
          return CORBA::_tc_ulong;

        case TCKind::tk_float:
          return CORBA::_tc_float;

        case TCKind::tk_double:
          return CORBA::_tc_double;

        case TCKind::tk_boolean:
          return CORBA::_tc_boolean;

        case TCKind::tk_char:
          return CORBA::_tc_char;

        case TCKind::tk_octet:
          return CORBA::_tc_octet;

        case TCKind::tk_string:
          return DynString_Handler::create_typecode (type);

        case TCKind::tk_longlong:
          return CORBA::_tc_longlong;

        case TCKind::tk_ulonglong:
          return CORBA::_tc_ulonglong;

        case TCKind::tk_longdouble:
          return CORBA::_tc_longdouble;

        case TCKind::tk_wchar:
          return CORBA::_tc_wchar;

        case TCKind::tk_wstring:
          {
            uint32_t bound {};
            ::DAnCE::Config_Handlers::BoundedStringType const* bounded = type.boundedString_if_set ();
            if (bounded && bounded->bound ().size () > 1)
              throw Config_Error ("", "A bounded wide string type can have at most one bound");
            if (bounded && !bounded->bound ().empty ()) bound = bounded->bound ().front ();
            return this->orb_->create_wstring_tc (bound);
          }

        case TCKind::tk_enum:
          return DynEnum_Handler::create_typecode (type);

        case TCKind::tk_struct:
          return DynStruct_Handler::create_typecode (type);

        case TCKind::tk_sequence:
          return DynSequence_Handler::create_typecode (type);

        case TCKind::tk_alias:
          return DynAlias_Handler::create_typecode (type);

        case TCKind::tk_array:
          return DynArray_Handler::create_typecode (type);

        case TCKind::tk_any:
          return CORBA::_tc_any;
        case TCKind::tk_TypeCode:
          return CORBA::_tc_TypeCode;
        case TCKind::tk_Principal:
          return CORBA::_tc_Principal;
        case TCKind::tk_objref:
          return CORBA::_tc_Object;
        case TCKind::tk_fixed:
        case TCKind::tk_union:
        case TCKind::tk_except:
        case TCKind::tk_value:
        case TCKind::tk_value_box:
        case TCKind::tk_native:
        case TCKind::tk_abstract_interface:
        case TCKind::tk_local_interface:
        case TCKind::tk_component:
        case TCKind::tk_home:
        case TCKind::tk_event:
          DANCEX11_LOG_DEBUG("DynAny_Handler::create_typecode - Type not supported");
          throw Config_Error ("", "Type not supported");
        }

      DANCEX11_LOG_DEBUG("DynAny_Handler::create_typecode - Unknown type");
      throw Config_Error ("", "Unknown type");
    }

    void
    DynAny_Handler::register_typecode (
      std::string_view typeID,
       IDL::traits<CORBA::TypeCode>::ref_type tc)
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::register_typecode");

      this->typecode_map_.insert_or_assign (std::string (typeID), tc);
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynAny_Handler::get_typecode (std::string_view typeID)
    {
      DANCEX11_LOG_TRACE("DynAny_Handler::get_typecode");

      const TypecodeMap::const_iterator pos = this->typecode_map_.find (typeID);
      return pos == this->typecode_map_.end () ? IDL::traits<CORBA::TypeCode>::ref_type {} : pos->second;
    }
  }
}
