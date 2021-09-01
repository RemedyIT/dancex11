/**
 * @file    xml_file_intf.h
 * @author  Marijke Hengstmengel
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_CONFIG_XML_FILE_INTF_H
#define DANCEX11_CONFIG_XML_FILE_INTF_H

#pragma once

#include "config_handlers_export.h"
#include <string>
#include <memory>
#include "ace/XML_Utils/XML_Helper.h"
#include "ace/XML_Utils/XML_Schema_Resolver.h"
#include "config_xml_error_handler.h"

namespace Deployment
{
  class DeploymentPlan;
  class Domain;
}

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    class Config_Handlers_Export XML_File_Intf
    {
    public:
      XML_File_Intf (std::string file);

      ~XML_File_Intf ();

      ::Deployment::DeploymentPlan const *get_plan ();

      ::Deployment::Domain const *get_domain ();

      void add_search_path (const std::string& environment,
                            const std::string& relpath);

    protected:
      bool read_process_plan (const std::string& file);

      bool read_process_domain (const std::string& file);

    private:
      std::string const file_;
      std::unique_ptr< ::Deployment::DeploymentPlan> idl_dp_;
      std::unique_ptr< ::Deployment::Domain> idl_domain_;

      using XML_RESOLVER = ::XML::XML_Schema_Resolver< ::XML::Environment_Resolver>;
      using XML_HELPER = ::XML::XML_Helper<XML_RESOLVER, ::XML::DANCEX11_XML_Error_Handler>;

      XML_HELPER xml_helper_;
    };
  }
}

#endif /*DANCEX11_CONFIG_XML_FILE_INTF_H*/
