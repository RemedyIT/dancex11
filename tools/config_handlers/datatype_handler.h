/**
 * @file   datatype_handler.h
 * @author  Marijke Hengstmengel
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_DATATYPE_HANDLER_H
#define DANCEX11_CONFIG_DATATYPE_HANDLER_H

#pragma once

#include "Basic_Deployment_Data.hpp"
#include "dancex11/deployment/deployment_deploymentplanC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    IDL::traits<CORBA::TypeCode>::ref_type
    convert_datatype (const DAnCE::Config_Handlers::DataType& src);
  }
}

#endif /* DANCEX11_CONFIG_DATATYPE_HANDLER_H */
