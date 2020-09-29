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

namespace Deployment
{
  class DeploymentPlan;
  class Domain;
}

namespace XML
{
  class DANCEX11_XML_Typedef;
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
      std::string file_;
      std::unique_ptr< ::Deployment::DeploymentPlan> idl_dp_;
      std::unique_ptr< ::Deployment::Domain> idl_domain_;
      using XML_Helper_type = ::XML::DANCEX11_XML_Typedef;
    };
  }
}

#endif /*DANCEX11_CONFIG_XML_FILE_INTF_H*/
