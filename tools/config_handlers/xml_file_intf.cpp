/**
 * @file    xml_file_intf.cpp
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "xml_file_intf.h"

#include "Deployment.hpp"
#include "dp_handler.h"
#include "dd_handler.h"
#include "common.h"
#include "ace/XML_Utils/XML_Helper.h"
#include "ace/XML_Utils/XML_Schema_Resolver.h"
#include "config_xml_error_handler.h"

#include "dancex11/deployment/deployment_dataC.h"
#include "dancex11/deployment/deployment_targetdataC.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    XML_File_Intf::XML_File_Intf (std::string file)
      : file_ (std::move(file))
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::constructor");
    }

    XML_File_Intf::~XML_File_Intf ()
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::destructor");
      // XML_Helper::XML_HELPER.terminate_parser();
    }

    bool
    XML_File_Intf::read_process_plan (const std::string& file)
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::read_process_plan - " << file);

      try
      {
        if (!xml_helper_.is_initialized ())
          return false;

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_plan - Constructing DOM");

        XERCES_CPP_NAMESPACE::DOMDocument *dom = xml_helper_.create_dom (file.c_str());

        if (!dom)
        {
          DANCEX11_LOG_ERROR ("XML_File_Intf::read_process_plan - "
                              "Failed to open file <" << file << ">");
          return false;
        }

        XERCES_CPP_NAMESPACE::DOMElement *foo = dom->getDocumentElement ();

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_plan - "
                           "DOMElement pointer: " << foo);

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_plan - "
                            "Parsing XML file with XSC");

        DAnCE::Config_Handlers::deploymentPlan dp =
          DAnCE::Config_Handlers::reader::DeploymentPlan (dom);

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_plan - "
                            "Processing using config handlers");

        DAnCEX11::Config_Handlers::DP_Handler dp_handler;

        this->idl_dp_ = dp_handler.resolve_plan (dp);

        if (!this->idl_dp_)
        {
          DANCEX11_LOG_ERROR ("XML_File_Intf::read_process_plan - "
                              "Error converting XML plan to IDL");
        }
        else
        {
          return true;
        }
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("XML_File_Intf::caught - "
                            "CORBA Exception while parsing XML into IDL" << ex);
        throw Config_Error (this->file_, ex.what ());
      }
      catch (const Config_Error &ex)
      {
        throw ex;
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("XML_File_Intf::caught - "
                            "Unexpected exception while parsing XML into IDL.");
        throw Config_Error (this->file_,
                            "Unexpected C++ exception while parsing XML");
      }
      return false;
    }

    bool
    XML_File_Intf::read_process_domain (const std::string& file)
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::read_process_domain");

      try
      {
        if (!xml_helper_.is_initialized ())
          return false;

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_domain - Constructing DOM");
        XERCES_CPP_NAMESPACE::DOMDocument *dom = xml_helper_.create_dom (file.c_str());

        if (!dom)
        {
           DANCEX11_LOG_ERROR ("XML_File_Intf::read_process_domain - "
                               "Failed to open file <" << file << ">");
           return false;
        }

        XERCES_CPP_NAMESPACE::DOMElement *foo = dom->getDocumentElement ();
        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_domain - DOMElement pointer:"
                               << foo);

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_domain - "
                            "Parsing XML file with XSC");

        DAnCE::Config_Handlers::Domain dd = DAnCE::Config_Handlers::reader::domain (dom);

        DANCEX11_LOG_DEBUG ("XML_File_Intf::read_process_domain - "
                             "Processing using config handlers");

        DAnCEX11::Config_Handlers::DD_Handler dd_handler;

        this->idl_domain_ = dd_handler.resolve_domain (dd);

        if (!this->idl_domain_)
        {
          DANCEX11_LOG_ERROR ("XML_File_Intf::read_process_domain - "
                              "Error converting XML plan to IDL");
        }
        else
        {
          return true;
        }
      }
      catch (const CORBA::Exception &ex)
      {
         DANCEX11_LOG_ERROR ("XML_File_Intf::caught - CORBA Exception while parsing XML into IDL"
                             << ex);
         throw Config_Error (this->file_, ex.what());
      }
      catch (const Config_Error &ex)
      {
        throw ex;
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("Unexpected exception while parsing XML into IDL");
        throw Config_Error (this->file_,
                           "Unexpected C++ exception while parsing XML");
      }

      return false;
    }

    ::Deployment::DeploymentPlan const *
    XML_File_Intf::get_plan ()
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::get_plan");

      if (!this->idl_dp_ && !this->read_process_plan (this->file_))
        return {};

      return this->idl_dp_.get ();
    }

    ::Deployment::Domain const *
    XML_File_Intf::get_domain ()
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::get_domain " << this->file_);

      if (!this->idl_domain_ && !this->read_process_domain (this->file_))
        return {};

      return this->idl_domain_.get ();
    }

    void
    XML_File_Intf::add_search_path (const std::string& environment,
                                    const std::string& relpath)
    {
      DANCEX11_LOG_TRACE("XML_File_Intf::add_search_path");
      xml_helper_.get_resolver ().get_resolver ().add_path (environment.c_str (), relpath.c_str());
    }

  }
}
