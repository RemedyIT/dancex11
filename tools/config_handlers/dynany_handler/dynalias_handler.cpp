/**
 * @file    dynalias_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dynalias_handler.h"
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
    DynAlias_Handler::extract_into_dynany (const DataType &type,
                                           const DataValue &value,
                                           IDL::traits<CORBA::TypeCode>::ref_type req_tc)
    {
      DANCEX11_LOG_TRACE("DynAlias_Handler::extract_into_dynany");

      try
      {
        IDL::traits<CORBA::TypeCode>::ref_type tc;

        if (req_tc)
          tc = req_tc;
        else
          tc = DynAlias_Handler::create_typecode (type);

        IDL::traits<DynamicAny::DynAny>::ref_type alias = DYNANY_HANDLER->extract_into_dynany (
          type.alias ().elementType (), value, tc);
        return alias;
      }
      catch (const DynamicAny::DynAny::InvalidValue&)
      {
        DANCEX11_LOG_ERROR("Invalid value provided in XML when trying to "
                           "initialize an instance of enumerated type " << type.alias ().typeId ());
        throw Config_Error (type.alias ().typeId (),"Invalid value provided in XML");
      }
      catch (const Config_Error &ex)
      {
        DANCEX11_LOG_ERROR ("DynAlias_Handler caught Config_Error");
        if (type.alias_p ())
          throw Config_Error (ex.add_name(type.alias ().typeId ()),
                              ex.get_error());
        else
          throw ex;
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("DynAlias_Handler caught unknown exception");
        throw Config_Error (type.alias ().typeId (),"Unknown exception");
      }
    }

    IDL::traits<CORBA::TypeCode>::ref_type
    DynAlias_Handler::create_typecode (const DataType &type)
    {
      DANCEX11_LOG_TRACE("DynAlias_Handler::create_typecode");

      if (!type.alias_p ())
      {
        DANCEX11_LOG_ERROR ("ERROR: Alias type description required");
        throw Config_Error ("","Did not find expected alias type description, tk_kind may be wrong.");
      }

      IDL::traits<CORBA::TypeCode>::ref_type tc =
        DYNANY_HANDLER->orb ()->create_alias_tc
        (type.alias ().typeId ().c_str (),
         type.alias ().name ().c_str (),
         DYNANY_HANDLER->create_typecode (type.alias ().elementType ()));

      DYNANY_HANDLER->register_typecode (type.alias ().typeId (),
                                         tc);

      return tc;
    }

  }
}

