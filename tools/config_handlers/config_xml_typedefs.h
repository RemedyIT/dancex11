/**
 * @file    config_xml_typedefs.h
 * @author  Marijke Hengstmengel
 *
 * @brief  DANCEX11 version of the typedefs from the XML Utilities.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_XML_TYPEDEFS_H
#define DANCEX11_XML_TYPEDEFS_H

#include "config_handlers_export.h"
#include "ace/XML_Utils/XML_Helper.h"
#include "ace/XML_Utils/XML_Schema_Resolver.h"
#include "config_xml_error_handler.h"

namespace XML
{
  class Config_Handlers_Export DANCEX11_XML_Typedef
  {
  public:
    using XML_RESOLVER = ::XML::XML_Schema_Resolver< ::XML::Environment_Resolver>;
    using ERROR_HANDLER = ::XML::DANCEX11_XML_Error_Handler;
    using HELPER = ::XML::XML_Helper<XML_RESOLVER, ERROR_HANDLER>;

    static HELPER XML_HELPER;
  };
}

#endif /* DANCEX11_XML_TYPEDEFS_H */
