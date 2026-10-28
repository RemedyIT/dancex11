/**
 * @file    datatype_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "datatype_handler.h"
#include "common.h"
#include "dynany_handler/dynany_handler.h"
#include "tao/x11/anytypecode/typecode.h"
#include "tao/x11/anytypecode/ValueModifierC.h"
#include "tao/x11/anytypecode/VisibilityC.h"
#include "tao/x11/ifr_client/IFR_ExtendedC.h"
#include "dancex11/logger/log.h"
namespace DAnCEX11
{
  namespace Config_Handlers
  {
    IDL::traits<CORBA::TypeCode>::ref_type
    convert_datatype (const DAnCE::Config_Handlers::DataType& src)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Config_Handlers::convert_datatype");

      if (src.kind ().integral () != DAnCE::Config_Handlers::TCKind::tk_value)
        return DYNANY_HANDLER->create_typecode (src);

      DAnCE::Config_Handlers::ValueType const* value = src.value_if_set ();
      if (!value)
        throw Config_Error ("", "Value type description required");

      CORBA::ValueModifier modifier {};
      if (value->modifier () == "none") modifier = CORBA::VM_NONE;
      else if (value->modifier () == "custom") modifier = CORBA::VM_CUSTOM;
      else if (value->modifier () == "abstract") modifier = CORBA::VM_ABSTRACT;
      else if (value->modifier () == "truncatable") modifier = CORBA::VM_TRUNCATABLE;
      else throw Config_Error (value->name (), "Invalid value type modifier");

      CORBA::ValueMemberSeq members;
      for (DAnCE::Config_Handlers::ValueMemberType const& source_member : value->member ())
      {
        CORBA::Visibility visibility {};
        if (source_member.visibility () == "public") visibility = CORBA::PUBLIC_MEMBER;
        else if (source_member.visibility () == "private") visibility = CORBA::PRIVATE_MEMBER;
        else throw Config_Error (source_member.name (), "Invalid value member visibility");

        members.emplace_back (source_member.name (), "", value->typeId (), "1.0",
                              DYNANY_HANDLER->create_typecode (source_member.type ()),
                              IDL::traits<CORBA::IDLType>::ref_type {}, visibility);
      }

      IDL::traits<CORBA::TypeCode>::ref_type dest = DYNANY_HANDLER->orb ()->create_value_tc (
        value->typeId (), value->name (), modifier,
        DYNANY_HANDLER->create_typecode (value->baseType ()), members);
      DYNANY_HANDLER->register_typecode (value->typeId (), dest);
      return dest;
    }
  }
}
