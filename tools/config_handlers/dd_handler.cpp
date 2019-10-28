/**
 * @file    dd_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dd_handler.h"
#include "node_handler.h"
#include "Deployment.hpp"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    std::unique_ptr<Deployment::Domain>
    DD_Handler::resolve_domain (const DAnCE::Config_Handlers::Domain &domain) const
    {
      DANCEX11_LOG_TRACE("DD_Handler::resolve_domain");

      std::unique_ptr<Deployment::Domain> idl_dd = std::make_unique<Deployment::Domain> ();

      if (domain.label_p ())
      {
        idl_dd->label(domain.label ());
      }

      if (domain.UUID_p ())
      {
        idl_dd->UUID (domain.UUID ());
      }

      std::transform(domain.begin_node (),
                     domain.end_node (),
                     std::back_inserter(idl_dd->node()),
                     DAnCEX11::Config_Handlers::convert_node);

      return idl_dd;
    }
  }
}

