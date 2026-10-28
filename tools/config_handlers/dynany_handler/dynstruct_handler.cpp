/**
 * @file    dynstruct_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dynstruct_handler.h"
#include "dynany_handler.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "tao/x11/ifr_client/IFR_BaseC.h"
#include "common.h"

#include <map>
#include <memory>
#include <set>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    void create_type_map (const DataType &type,
                          std::map<std::string, DataType const*>& dt_map)
    {
      DANCEX11_LOG_TRACE("Config_Handlers::create_type_map");
      ::DAnCE::Config_Handlers::StructType const* struct_type = type._cxx_struct_if_set ();
      if (!struct_type)
        throw Config_Error ("", "Struct type description required");

      for (::DAnCE::Config_Handlers::StructMemberType const& member : struct_type->member ())
        dt_map[member.name ()] = std::addressof (member.type ());
    }

    IDL::traits<DynamicAny::DynAny>::ref_type
    DynStruct_Handler::extract_into_dynany (const DataType &type,
                                            const DataValue &value,
                                            IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynStruct_Handler::extract_into_dynany");
      ::DAnCE::Config_Handlers::StructType const* struct_type = type._cxx_struct_if_set ();
      if (!struct_type)
        throw Config_Error ("", "Struct type description required");

      try
      {
        IDL::traits<CORBA::TypeCode>::ref_type tc;

        if (req_tc)
          tc = req_tc;
        else
          tc = DynStruct_Handler::create_typecode (type);

        std::map<std::string, DataType const*> dt_map;
        create_type_map (type, dt_map);

        // Make the actual DynStruct
        IDL::traits<DynamicAny::DynAny>::ref_type temp =
          DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
        IDL::traits<DynamicAny::DynStruct>::ref_type retval =
            IDL::traits<DynamicAny::DynStruct>::narrow (temp);

        uint32_t pos {};
        DynamicAny::NameDynAnyPairSeq values;
        values.resize (value.member ().size ());
        std::set<std::string> seen_members;

        for (::DAnCE::Config_Handlers::NamedValue const& member : value.member ())
        {
          std::map<std::string, DataType const*>::const_iterator const data_type = dt_map.find (member.name ());
          if (data_type == dt_map.end ())
            throw Config_Error (member.name (), "Unknown member in struct value");
          if (!seen_members.insert (member.name ()).second)
            throw Config_Error (member.name (), "Duplicate member in struct value");

          values[pos].id(member.name ());
          values[pos].value (DYNANY_HANDLER->extract_into_dynany (*data_type->second, member.value ()));
          ++pos;
        }

        if (seen_members.size () != dt_map.size ())
          throw Config_Error (struct_type->typeId (), "Missing member in struct value");

        retval->set_members_as_dyn_any (values);

        return retval;
      }
      catch (const Config_Error &ex)
      {
        throw Config_Error (ex.add_name (struct_type->typeId ()), ex.get_error ());
      }
      catch (...)
      {
        throw Config_Error (struct_type->typeId (), "Unknown exception");
      }
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynStruct_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynStruct_Handler::create_typecode");

      ::DAnCE::Config_Handlers::StructType const* struct_type = type._cxx_struct_if_set ();
      if (!struct_type)
      {
        DANCEX11_LOG_ERROR ("ERROR: Struct type description required");
        throw Config_Error ("", "Expected struct type information, tc_kind may be incorrect");
      }
      std::string rid (struct_type->typeId ());
      std::string name (struct_type->name ());

      CORBA::StructMemberSeq members;

      for (::DAnCE::Config_Handlers::StructMemberType const& source_member : struct_type->member ())
      {
        CORBA::StructMember member =
          CORBA::StructMember (source_member.name (),
                               DYNANY_HANDLER->create_typecode (source_member.type ()),
                               nullptr);
        members.push_back (member);
      }

      IDL::traits<CORBA::TypeCode>::ref_type tc = DYNANY_HANDLER->orb ()->create_struct_tc (
        rid,
        name,
        members);

      DYNANY_HANDLER->register_typecode (struct_type->typeId (), tc);

      return tc;
    }
  }
}



