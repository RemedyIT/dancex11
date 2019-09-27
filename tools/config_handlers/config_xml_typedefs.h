/**
 * @file    config_xml_typedefs.h
 * @author  Marijke Hengstmengel
 *
 * @brief  DANCEX11 version of the typedefs from the XML Utilities.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
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
    typedef ::XML::Environment_Resolver PATH_RESOLVER;
    typedef ::XML::XML_Schema_Resolver< ::XML::Environment_Resolver > XML_RESOLVER;
    typedef ::XML::DANCEX11_XML_Error_Handler DANCEX11_ERROR_HANDLER;
    typedef ::XML::XML_Helper< XML_RESOLVER, DANCEX11_ERROR_HANDLER > HELPER;

    static DANCEX11_ERROR_HANDLER _xml_error_handler;

  public:
    static HELPER XML_HELPER;
  };
}

#endif /* DANCEX11_XML_TYPEDEFS_H */
