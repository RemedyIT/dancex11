/**
 * @file    config_xml_error_handler.cpp
 * @author  Marijke Hengstmengel
 *
 * @brief  DANCEX11 error handler for Xerces
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "config_xml_error_handler.h"
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include "ace/XML_Utils/XercesString.h"
#include "dancex11/logger/log.h"

namespace XML
{
  void DANCEX11_XML_Error_Handler::warning(const xercesc::SAXParseException& toCatch)
  {
    XStr file(toCatch.getSystemId ());
    XStr msg (toCatch.getMessage ());

    DANCEX11_LOG_DEBUG ("Warning: " << file << ':' << toCatch.getLineNumber ()
                  << ':' << toCatch.getColumnNumber () << " - "
                  << msg );
  }

  void DANCEX11_XML_Error_Handler::error(const xercesc::SAXParseException& toCatch)
  {
    XStr file (toCatch.getSystemId ());
    XStr msg (toCatch.getMessage ());

    DANCEX11_LOG_ERROR ("Error: " << file << ':' << toCatch.getLineNumber ()
                  << ':' << toCatch.getColumnNumber () << " - "
                  << msg );

    this->errors_ = true;
  }

  void DANCEX11_XML_Error_Handler::fatalError(const xercesc::SAXParseException& toCatch)
  {
    XStr file (toCatch.getSystemId ());
    XStr msg (toCatch.getMessage ());

    DANCEX11_LOG_PANIC ("Fatal Error: " << file << ':' << toCatch.getLineNumber ()
                  << ':' << toCatch.getColumnNumber () << " - "
                  << msg );

    this->errors_ = true;
  }

  void DANCEX11_XML_Error_Handler::resetErrors()
  {
    this->errors_ = false;
  }

  bool
  DANCEX11_XML_Error_Handler::getErrors () const
  {
    return this->errors_;
  }
}
