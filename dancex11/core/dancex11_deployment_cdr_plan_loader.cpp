// -*- C++ -*-
/**
 * @file dancex11_deployment_cdr_plan_loader.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "ace/Service_Config.h"

#include "tao/CDR.h"

#include "tao/x11/tao_corba.h"

#include "dancex11_deployment_cdr_plan_loader.h"
#include "dancex11/logger/log.h"

#include <fstream>

// from deployment_deploymentplanCP.h ; normally hidden for the user
TAO_BEGIN_VERSIONED_NAMESPACE_DECL

DEPLOYMENT_STUB_Export TAO_CORBA::Boolean operator>> (TAO_InputCDR &, Deployment::DeploymentPlan&);
DEPLOYMENT_STUB_Export TAO_CORBA::Boolean operator>> (TAO_InputCDR &, Deployment::Domain&);

TAO_END_VERSIONED_NAMESPACE_DECL

namespace DAnCEX11
{
  CDRPlan_Loader* CDRPlan_Loader::instance ()
  {
    static CDRPlan_Loader instance_;

    return std::addressof(instance_);
  }

  bool
  CDRPlan_Loader::read_plan (const std::string& filename, Deployment::DeploymentPlan& plan)
  {
    DANCEX11_LOG_TRACE ("CDRPlan_Loader::read_plan");

    if (filename.empty ())
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_plan - "
                            "Passed an empty filename, returning.");
        return false;
      }

    std::ifstream fis (filename, std::ifstream::binary);

    if (!fis)
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_plan - " \
                            "Error: Unable to open file <" << filename
                            << "> for reading");
        return false;
      }

    char byte_order;

    if (!(fis >> byte_order))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_plan - "
                            "Byte order not written to file");
        return false;
      }

    uint32_t bufsize {};

    // C++ streams are not ideal for binary IO but this works
    if (!fis.read(reinterpret_cast<char *> (std::addressof(bufsize)), sizeof(bufsize)))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_plan - "
                            "Plan size not written to file");
        return false;
      }

    if (byte_order != ACE_CDR_BYTE_ORDER)
      {
        DANCEX11_LOG_DEBUG ("CDRPlan_Loader::read_plan - "
                            "Plan appears encoded in a different endian.");
        ACE_CDR::swap_4 (reinterpret_cast<char *> (std::addressof(bufsize)),
                         reinterpret_cast<char *> (std::addressof(bufsize)));
      }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufsize);

#if defined (ACE_INITIALIZE_MEMORY_BEFORE_USE)
    ACE_OS::memset(buffer.get (), 0, bufsize);
#endif /* ACE_INITIALIZE_MEMORY_BEFORE_USE */

    if (!fis.read(buffer.get (), bufsize))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_plan - " <<
                            "read less than expected " << bufsize << " bytes");
        return false;
      }

    fis.close ();

    ACE_Message_Block mb (ACE_CDR::MAX_ALIGNMENT + bufsize);
    ACE_CDR::mb_align (std::addressof(mb));
    mb.copy (buffer.get (), bufsize);

    TAO_InputCDR input_cdr (std::addressof(mb));
    input_cdr.reset_byte_order ((int) byte_order);

    DANCEX11_LOG_DEBUG ("CDRPlan_Loader::read_plan - " <<
                        "Reading file " << filename <<
                        " in " << (ACE_CDR_BYTE_ORDER ? "little" : "big") <<
                        " endian format with size " << bufsize);

    if (!(input_cdr >> plan))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_plan - "
                            "Failed to demarshal plan\n");
        return false;
      }

    return true;
  }

  bool
  CDRPlan_Loader::read_domain (
      const std::string& filename,
      Deployment::Domain& dom)
  {
    DANCEX11_LOG_TRACE ("CDRPlan_Loader::read_domain");

    if (filename.empty ())
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_domain - "
                            "Passed an empty filename, returning.");
        return false;
      }

    std::ifstream fis (filename);

    if (!fis)
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_domain - " \
                            "Error: Unable to open file <" << filename
                            << "> for reading");
        return false;
      }

    char byte_order;

    if (!(fis >> byte_order))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_domain - "
                            "Byte order not written to file");
        return false;
      }

    uint32_t bufsize {};

    // C++ streams are not ideal for binary IO but this works
    if (!fis.read(reinterpret_cast<char *> (std::addressof(bufsize)), sizeof(bufsize)))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_domain - "
                            "Plan size not written to file");
        return false;
      }

    if (byte_order != ACE_CDR_BYTE_ORDER)
      {
        DANCEX11_LOG_DEBUG ("CDRPlan_Loader::read_domain - "
                            "Plan appears encoded in a different endian.");
        ACE_CDR::swap_4 (reinterpret_cast<char *> (std::addressof(bufsize)),
                         reinterpret_cast<char *> (std::addressof(bufsize)));
      }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufsize);

#if defined (ACE_INITIALIZE_MEMORY_BEFORE_USE)
    ACE_OS::memset(buffer.get (), 0, bufsize);
#endif /* ACE_INITIALIZE_MEMORY_BEFORE_USE */

    if (!fis.read(buffer.get (), bufsize))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_domain - " <<
                            "read less than expected " << bufsize << " bytes");
        return false;
      }

    fis.close ();

    ACE_Message_Block mb (ACE_CDR::MAX_ALIGNMENT + bufsize);
    ACE_CDR::mb_align (std::addressof(mb));
    mb.copy (buffer.get (), bufsize);

    TAO_InputCDR input_cdr (std::addressof(mb));
    input_cdr.reset_byte_order ((int) byte_order);

    DANCEX11_LOG_DEBUG ("CDRPlan_Loader::read_domain - " <<
                        "Reading file " << filename <<
                        " in " << (ACE_CDR_BYTE_ORDER ? "little" : "big") <<
                        " endian format with size " << bufsize);


    if (!(input_cdr >> dom))
      {
        DANCEX11_LOG_ERROR ("CDRPlan_Loader::read_domain - "
                            "Failed to demarshal domain\n");
        return false;
      }

    return true;
  }

  ACE_STATIC_SVC_DEFINE (CDRPlan_Loader_Svc,
                         ACE_TEXT ("DAnCEX11_CDRPlan_Loader"),
                         ACE_SVC_OBJ_T,
                         &ACE_SVC_NAME (CDRPlan_Loader_Svc),
                         ACE_Service_Type::DELETE_THIS
                          | ACE_Service_Type::DELETE_OBJ,
                         0)
  ACE_FACTORY_DEFINE (DANCEX11_CDRPLAN_LOADER, CDRPlan_Loader_Svc)

  CDRPlan_Loader_Svc::~CDRPlan_Loader_Svc ()
  {
    DANCEX11_LOG_TRACE ("CDRPlan_Loader::~CDRPlan_Loader");
  }

  int
  CDRPlan_Loader_Svc::init (int /*argc*/, ACE_TCHAR * /*argv*/[])
  {
    DANCEX11_LOG_TRACE ("CDRPlan_Loader::init");
    Plan_Loader::register_plan_loader ("cdr", CDRPlan_Loader::instance ());
    Plan_Loader::register_plan_loader ("ddcdr", CDRPlan_Loader::instance ());
    Plan_Loader::register_plan_loader ("dpcdr", CDRPlan_Loader::instance ());
    return 0;
  }

  /// Terminates object when dynamic unlinking occurs.
  int
  CDRPlan_Loader_Svc::fini ()
  {
    DANCEX11_LOG_TRACE ("CDRPlan_Loader::fini");

    Plan_Loader::deregister_plan_loader ("cdr");
    Plan_Loader::deregister_plan_loader ("ddcdr");
    Plan_Loader::deregister_plan_loader ("dpcdr");
    return 0;
  }

  int
  CDRPlan_Loader_Svc::Initializer ()
  {
    DANCEX11_LOG_TRACE ("CDRPlan_Loader::Initializer");
    if (ACE_Service_Config::process_directive (ace_svc_desc_CDRPlan_Loader_Svc) == 0)
    {
      Plan_Loader::register_plan_loader ("cdr", CDRPlan_Loader::instance ());
      Plan_Loader::register_plan_loader ("ddcdr", CDRPlan_Loader::instance ());
      Plan_Loader::register_plan_loader ("dpcdr", CDRPlan_Loader::instance ());
      return 0;
    }
    return 1;
  }

};
