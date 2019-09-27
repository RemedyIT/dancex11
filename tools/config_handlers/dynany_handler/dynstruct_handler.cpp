/**
 * @file    dynstruct_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dynstruct_handler.h"
#include "dynany_handler.h"
#include "dancex11/logger/log.h"
#include "Basic_Deployment_Data.hpp"
#include "tao/x11/ifr_client/IFR_BaseC.h"
#include "common.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    using DAnCE::Config_Handlers::StructType;

    void create_type_map (const DataType &type,
                          std::map <std::string, DataType const *> &dt_map)
    {
      DANCEX11_LOG_TRACE("Config_Handlers::create_type_map");

      for (StructType::member_const_iterator i =
             type.struct_ ().begin_member ();
           i != type.struct_ ().end_member (); ++i)
      {
        dt_map[i->name ()] = &(i->type ());
      }
    }

    IDL::traits<DynamicAny::DynAny>::ref_type
    DynStruct_Handler::extract_into_dynany (const DataType &type,
                                            const DataValue &value,
                                            IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynStruct_Handler::extract_into_dynany");

      try
      {
        IDL::traits<CORBA::TypeCode>::ref_type tc;

        if (req_tc)
          tc = req_tc;
        else
          tc = DynStruct_Handler::create_typecode (type);

        std::map <std::string, DataType const*> dt_map;
        create_type_map (type, dt_map);

        // Make the actual DynStruct
        IDL::traits<DynamicAny::DynAny>::ref_type temp =
          DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
        IDL::traits<DynamicAny::DynStruct>::ref_type retval =
            IDL::traits<DynamicAny::DynStruct>::narrow (temp);

        uint32_t pos {};
        DynamicAny::NameDynAnyPairSeq values;
        values.resize (value.count_member ());

        for (DataValue::member_const_iterator i = value.begin_member ();
             i != value.end_member (); ++i)
        {
          values[pos].id(i->name ());
          values[pos].value (DYNANY_HANDLER->extract_into_dynany (*dt_map[i->name ()],
                                                 i->value ()));
          ++pos;
        }

        retval->set_members_as_dyn_any (values);

        return retval;
      }
      catch (const Config_Error &ex)
      {
        if (type.struct_p ())
          throw Config_Error (ex.add_name(type.struct_ ().typeId ()),
                              ex.get_error());
        else
          throw ex;
      }
      catch (...)
      {
        throw Config_Error (type.struct_ ().typeId (),
                            "Unknown exception");
      }
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynStruct_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynStruct_Handler::create_typecode");

      if (!type.struct_p ())
        {
          DANCEX11_LOG_ERROR ("ERROR: Struct type description required");
          throw Config_Error ("","Expected struct type information, tc_kind may be"\
                                 "incorrect");
        }
      std::string  rid (type.struct_ ().typeId ());
      std::string  name (type.struct_ ().name ());

      CORBA::StructMemberSeq members;

      for (StructType::member_const_iterator i =
        type.struct_ ().begin_member ();
           i != type.struct_ ().end_member (); ++i)
      {
        CORBA::StructMember member =
           CORBA::StructMember(i->name (),
                               DYNANY_HANDLER->create_typecode (i->type ()),
                               nullptr);
          members.push_back (member);
      }

      IDL::traits<CORBA::TypeCode>::ref_type tc {};
      tc =
        DYNANY_HANDLER->orb ()->create_struct_tc (
          rid,
          name ,
          members);

      DYNANY_HANDLER->register_typecode (type.struct_ ().typeId (), tc);

      return tc;
    }
  }
}



