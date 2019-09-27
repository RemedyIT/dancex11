// -*- C++ -*-
/**
 * @file dancex11_deployment_cdp_plan_loader.cpp
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "ace/Service_Config.h"
#include "dancex11_deployment_cdp_plan_loader.h"
#include "xml_file_intf.h"
#include "common.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/logger/log.h"
#include "dynany_handler/dynany_handler.h"

#include <fstream>

namespace DAnCEX11
{
  CDPPlan_Loader* CDPPlan_Loader::instance ()
  {
    static CDPPlan_Loader instance_;

    return &instance_;
  }

  bool
  CDPPlan_Loader::read_plan (const std::string& filename, Deployment::DeploymentPlan& plan)
  {
    DANCEX11_LOG_TRACE ("CDPPlan_Loader::read_plan");

    if (filename.empty ())
    {
      DANCEX11_LOG_ERROR ("CDPPlan_Loader::read_plan - "
                          "Passed an empty filename, returning.");
      return false;
    }

    DAnCEX11::Config_Handlers::XML_File_Intf xml_intf (filename);
    xml_intf.add_search_path ("DANCEX11_ROOT", "/etc/schema/");
    xml_intf.add_search_path ("DNC_SCHEMA_ROOT", "/schema/");

    try
    {
      Deployment::DeploymentPlan const *plan_ = xml_intf.get_plan ();
      if (!plan_)
      {
        DANCEX11_LOG_ERROR ("CDPPlan_Loader::read_plan - "
                            "xml_intf.get_plan returned null plan.");
        throw Deployment::PlanError ("CDPPlan_Loader::read_plan", "null plan");
      }
      plan = std::move(*plan_);
      return true;
    }
    catch (const DAnCEX11::Config_Handlers::Config_Error &ex)
    {
      DANCEX11_LOG_ERROR ("CDPPlan_Loader::read_plan - "
                          "xml_intf.get_plan throws an error.");
      throw Deployment::PlanError (ex.get_name(), ex.get_error());
    }
    return false;
  }

  bool
  CDPPlan_Loader::read_domain (const std::string& filename, Deployment::Domain& domain)
  {
    DANCEX11_LOG_TRACE ("CDPPlan_Loader::read_domain ");

    if (filename.empty ())
    {
      DANCEX11_LOG_ERROR ("CDPPlan_Loader::read_domain - "
                          "Passed an empty filename, returning.");
      return false;
    }

    DAnCEX11::Config_Handlers::XML_File_Intf xml_intf (filename);
    xml_intf.add_search_path ("DANCEX11_ROOT", "/etc/schema/");
    xml_intf.add_search_path ("DNC_SCHEMA_ROOT", "/schema/");

    try
    {
      Deployment::Domain const *domain_ = xml_intf.get_domain ();
      if (!domain_)
      {
        DANCEX11_LOG_ERROR ("CDPPlan_Loader::read_domain - "
                            "xml_intf.get_plan returned null domain.");
        throw Deployment::PlanError ("CDPPlan_Loader::read_domain", "null domain");
      }
      domain = std::move(*domain_);
      return true;
    }
    catch (const DAnCEX11::Config_Handlers::Config_Error &ex)
    {
      DANCEX11_LOG_ERROR ("CDPPlan_Loader::read_domain - "
                          "xml_intf.get_domain throws an error.");
      throw Deployment::PlanError (ex.get_name(), ex.get_error());
    }
    return false;
  }

  ACE_STATIC_SVC_DEFINE (CDPPlan_Loader_Svc,
                         ACE_TEXT ("DAnCEX11_CDPPlan_Loader"),
                         ACE_SVC_OBJ_T,
                         &ACE_SVC_NAME (CDPPlan_Loader_Svc),
                         ACE_Service_Type::DELETE_THIS
                          | ACE_Service_Type::DELETE_OBJ,
                         0)
  ACE_FACTORY_DEFINE (Config_Handlers, CDPPlan_Loader_Svc)

  CDPPlan_Loader_Svc::~CDPPlan_Loader_Svc ()
  {
  }

  int
  CDPPlan_Loader_Svc::init (int /*argc*/, ACE_TCHAR* /*argv*/[])
  {
    DANCEX11_LOG_TRACE ("CDPPlan_Loader_Svc::init");
    Plan_Loader::register_plan_loader ("cdp", CDPPlan_Loader::instance ());
    Plan_Loader::register_plan_loader ("xml", CDPPlan_Loader::instance ());
    Plan_Loader::register_plan_loader ("cdd", CDPPlan_Loader::instance ());
    return 0;
  }

  /// Terminates object when dynamic unlinking occurs.
  int
  CDPPlan_Loader_Svc::fini ()
  {
    DANCEX11_LOG_TRACE ("CDPPlan_Loader_Svc::fini");

    Plan_Loader::deregister_plan_loader ("cdp");
    Plan_Loader::deregister_plan_loader ("xml");
    Plan_Loader::deregister_plan_loader ("cdd");
    return 0;
  }

  int
  CDPPlan_Loader_Svc::Initializer ()
  {
    DANCEX11_LOG_TRACE ("CDPPlan_Loader_Svc::Initializer");
    if (ACE_Service_Config::process_directive (ace_svc_desc_CDPPlan_Loader_Svc) == 0)
    {
      Plan_Loader::register_plan_loader ("cdp", CDPPlan_Loader::instance ());
      Plan_Loader::register_plan_loader ("xml", CDPPlan_Loader::instance ());
      Plan_Loader::register_plan_loader ("cdd", CDPPlan_Loader::instance ());
      return 0;
    }
    return 1;
  }

};
