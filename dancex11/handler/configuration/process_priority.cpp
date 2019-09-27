/**
 * @file    process_priority.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#include "process_priority.h"

#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/deployment/deployment_starterrorC.h"
#include "dancex11/logger/log.h"

#include <cstring>
#include <string>

namespace DAnCEX11
{
  std::string
  Process_Priority::type ()
  {
    return DAnCEX11::DANCE_LM_PROCESSPRIORITY;
  }

  void
  Process_Priority::configure (const ::Deployment::Property &prop)
  {
    int32_t prio;

    if (!(prop.value () >>= prio))
      {
        DANCEX11_LOG_ERROR ("Process_Priority::configure - " <<
                            "Unable to extract priority value from config property");
        throw ::Deployment::PlanError (prop.name (),
                                       "Unable to extract priority value from config property");
      }

    ACE_hthread_t handle;

    ACE_OS::thr_self (handle);

    int retval = ACE_OS::thr_setprio (handle,
                                      static_cast<int> (prio),
                                      -1);

    if (retval != 0)
      {
        std::string str =
          "Unable to set process priority to <" + std::to_string(prio) + ">: "
            + std::strerror (errno);

        DANCEX11_LOG_ERROR ("Process_Priority::configure - " << str);

        throw ::Deployment::StartError (prop.name (), str);

      }
  }

  void
  Process_Priority::close ()
  {
  }
}

extern "C"
{
  void create_process_priority (
    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type& plugin)
  {
    plugin =  CORBA::make_reference <DAnCEX11::Process_Priority> ();
  }
}
