/**
 * @file    cpu_affinity.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#include "cpu_affinity.h"

#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/deployment/deployment_starterrorC.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

#include "ace/OS_NS_unistd.h"
#include "ace/os_include/os_sched.h"

namespace DAnCEX11
{
  std::string
  CPU_Affinity::type ()
  {
    return DAnCEX11::DANCE_LM_CPUAFFINITY;
  }

  void
  CPU_Affinity::configure (const ::Deployment::Property & prop)
  {
#if defined (ACE_HAS_SCHED_SETAFFINITY)
    std::string affinity;

    if (!(prop.value () >>= affinity))
      {
        DANCEX11_LOG_ERROR ("CPU_Affinity::configure - " <<
                            "Unable to extract CPU affinity string");
        throw ::Deployment::StartError (prop.name (),
                                        "Unable to extract CPU affinity string");
      }

    cpu_set_t mask;
    CPU_ZERO (std::addressof(mask));
    std::vector< std::string > tokens =
      DAnCEX11::Utility::tokenize (affinity.c_str(), ',');
    for (const std::string &token : tokens)
      {
        int i = atoi (token.c_str ());
        if (i >= 0)
          {
            DANCEX11_LOG_DEBUG ("CPU_Affinity::configure - " <<
                                "Toggling affinity for CPU " << i);
            CPU_SET (i, &mask);
          }
        else
          {
            DANCEX11_LOG_ERROR ("CPU_Affinity::configure - " <<
                                "All affinity values should be greater than 0");
            throw ::Deployment::StartError (prop.name (),
                                            "All affinity values should be greater than 0");
          }
      }
    pid_t const pid = ACE_OS::getpid ();

    int retval = ::sched_setaffinity (pid,
                                      sizeof (cpu_set_t),
                                      &mask);

    if (retval != 0)
      {
        std::string str =
          "Unable to set CPU affinity to <" + affinity + ">: "
            + std::strerror (errno);

        DANCEX11_LOG_ERROR ("CPU_Affinity::configure - " << str);

        throw ::Deployment::StartError (prop.name(), str);
      }
#else
    X11_UNUSED_ARG (prop);
    DANCEX11_LOG_WARNING ("CPU Affinity not supported on this platform");
#endif
  }

  void
  CPU_Affinity::close ()
  {
  }
}

extern "C"
{
  void create_cpu_affinity (
    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type& plugin)
  {
    plugin = CORBA::make_reference <DAnCEX11::CPU_Affinity> ();
  }
}
