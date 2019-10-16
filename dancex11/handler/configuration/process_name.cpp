/**
 * @file    process_name.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "process_name.h"

#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/deployment/deployment_starterrorC.h"
#include "dancex11/logger/log.h"

#if defined (LINUX_VERSION_CODE) && defined (KERNEL_VERSION)
# if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,9))
#  include <sys/prctl.h>
#  include <stdio.h>
#  include <string.h>
#  include <errno.h>
#  define DANCEX11_HAS_PRCTL
# endif
#endif

namespace DAnCEX11
{
  std::string
  Process_Name::type ()
  {
    return DAnCEX11::DANCE_LM_PROCESSNAME;
  }

  void
  Process_Name::configure (const Deployment::Property & prop)
  {
#if defined (DANCEX11_HAS_PRCTL)
    std::string pname;
    if (prop.value () >>= pname)
    {
      if (pname.length () > 15)
        {
          pname.resize (15);
        }
      unsigned long const arg = reinterpret_cast<unsigned long> (pname.c_str ());

      int const retval = ::prctl (PR_SET_NAME, arg);

      if (retval != 0)
        {
          DANCEX11_LOG_WARNING ("Unable to set process name to <" << pname << ">:<" << strerror(errno) << ">");
        }
    }
    else
    {
      DANCEX11_LOG_ERROR ("Process_Name::configure - " <<
                          "Unable to extract process name from config property");
      throw ::Deployment::PlanError (prop.name (),
                                      "Unable to extract process name from config property");
    }
#else
    X11_UNUSED_ARG (prop);
    DANCEX11_LOG_WARNING ("Setting process name not supported on this platform");
#endif /* DANCEX11_HAS_PRCTL */
  }

  void
  Process_Name::close ()
  {
  }
}

extern "C"
{
  void
  create_process_name (
    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type& plugin)
  {
    plugin = CORBA::make_reference <DAnCEX11::Process_Name> ();
  }
}
