// -*- C++ -*-
/**
 * @file deployment_manager_exec.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "deployment_manager_module.h"

int
main (int argc, char *argv[])
{
  DANCEX11_LOG_TRACE ("DeploymentManager::main");

  DANCEX11_LOG_DEBUG ("Starting deployment_manager.");

  int retval = 0;

  DANCEX11_DEFINE_DEPLOYMENT_STATE (Deployment_Manager);

  try
  {
    DANCEX11_LOG_TRACE ("DeploymentManager - initializing module instance");

    IDL::traits<DAnCEX11::DeploymentManager_Module>::ref_type nm =
        CORBA::make_reference<DAnCEX11::DeploymentManager_Module> ();

    if (nm->init (argc, argv))
    {
      DANCEX11_LOG_TRACE ("DeploymentManager - running module instance");
      if (!nm->run ())
        retval = 1;
    }
    else
    {
      retval = 1;
    }

    DANCEX11_LOG_TRACE ("DeploymentManager - exiting");
  }
  catch (const CORBA::Exception& ex)
  {
    DANCEX11_LOG_PANIC ("DeploymentManager - Error, CORBA Exception: " << ex);
    retval = 1;
  }
  catch (const std::exception& ex)
  {
    DANCEX11_LOG_PANIC ("DeploymentManager - Error, std::exception: " << ex);
    retval = 1;
  }
  catch (...)
  {
    DANCEX11_LOG_PANIC ("DeploymentManager - Error: Unknown exception.");
    retval = 1;
  }

  DANCEX11_LOG_TRACE ("DeploymentManager - return code [" << retval << "]");

  return retval;
}
