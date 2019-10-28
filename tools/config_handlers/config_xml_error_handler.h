/**
 * @file    config_xml_error_handler.h
 * @author  Marijke Hengstmengel
 *
 * @brief  DANCEX11 version  error handler for Xerces
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_XML_ERROR_HANDLER_H
#define DANCEX11_XML_ERROR_HANDLER_H

#pragma once

#include "config_handlers_export.h"

#include <xercesc/sax/ErrorHandler.hpp>

using xercesc::SAXParseException;

namespace XML
{
  /**
    * @class DANCEX11_XML_Error_Hander
    *
    * @brief DANCEX11 error handler for XERCES
    *
    */
  class Config_Handlers_Export DANCEX11_XML_Error_Handler final
    : public xercesc::ErrorHandler
  {
  public:
    DANCEX11_XML_Error_Handler () = default;
    ~DANCEX11_XML_Error_Handler () = default;

    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();
    bool getErrors () const;
  private :
    DANCEX11_XML_Error_Handler (const DANCEX11_XML_Error_Handler&) = delete;
    DANCEX11_XML_Error_Handler& operator= (const DANCEX11_XML_Error_Handler&) = delete;

    bool errors_ {false};
  };
}

#endif /* DANCEX11_XML_ERROR_HANDLER_H */
