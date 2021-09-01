/**
 * @file    config_xml_error_handler.h
 * @author  Marijke Hengstmengel
 *
 * @brief  DANCEX11 error handler for Xerces
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_XML_ERROR_HANDLER_H
#define DANCEX11_XML_ERROR_HANDLER_H

#pragma once

#include "config_handlers_export.h"
#include "ace/XML_Utils/XML_Error_Handler.h"

namespace XML
{
  /**
    * @class DANCEX11_XML_Error_Hander
    *
    * @brief DANCEX11 error handler for XERCES
    */
  class Config_Handlers_Export DANCEX11_XML_Error_Handler final
#if ACE_VERSION_CODE <= 0x70003
    : public xercesc::ErrorHandler
#else
    : public XML::XML_Error_Handler
#endif
  {
  public:
    DANCEX11_XML_Error_Handler () = default;
    ~DANCEX11_XML_Error_Handler () override = default;

    void warning(const xercesc::SAXParseException& toCatch) override;
    void error(const xercesc::SAXParseException& toCatch) override;
    void fatalError(const xercesc::SAXParseException& toCatch) override;
#if ACE_VERSION_CODE <= 0x70003
    void resetErrors() override;
    bool getErrors () const;
#endif
  private :
    DANCEX11_XML_Error_Handler (const DANCEX11_XML_Error_Handler&) = delete;
    DANCEX11_XML_Error_Handler& operator= (const DANCEX11_XML_Error_Handler&) = delete;
    DANCEX11_XML_Error_Handler (DANCEX11_XML_Error_Handler&&) = delete;
    DANCEX11_XML_Error_Handler& operator= (DANCEX11_XML_Error_Handler&&) = delete;
#if ACE_VERSION_CODE <= 0x70003
    bool errors_ {};
#endif
  };
}

#endif /* DANCEX11_XML_ERROR_HANDLER_H */
