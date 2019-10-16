/**
 * @file    any_handler.cpp
 * @author  Marijke Hengstmengel
 *
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "any_handler.h"
#include "Basic_Deployment_Data.hpp"
#include "tao/x11/anytypecode/any.h"
#include "dancex11/logger/log.h"
#include "dynany_handler/dynany_handler.h"
#include "tools/config_handlers/common.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
//X11_FUZZ: disable check_taox11_namespaces
    void
    Any_Handler::extract_into_any (const DAnCE::Config_Handlers::Any& desc,
                                   CORBA::Any& toconfig)
    {
//X11_FUZZ: enable check_taox11_namespaces
      DANCEX11_LOG_TRACE("Any_Handler::extract_into_any");
      try
      {
        IDL::traits<DynamicAny::DynAny>::ref_type dyn =
            DYNANY_HANDLER->extract_into_dynany (desc.type (), desc.value ());
        toconfig = dyn->to_any ();

        dyn->destroy ();
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("Any_Handler::extract_into_any -"
                           " Caught CORBA Exception while extracting into dynany:" << ex);
        throw Config_Error ("",
                            "CORBA Exception while extracting into dynany");
      }
      catch (const Config_Error &ex)
      {
        throw ex;
      }
      catch (...)
      {
        throw Config_Error ("", "Caught error whilst parsing XML into Any");
      }
    }
  }
}
