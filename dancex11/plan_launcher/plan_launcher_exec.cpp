// -*- C++ -*-
/**
 * @file   plan_launcher_exec.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "plan_launcher_module.h"

int
main (int argc, char *argv[])
{
  DANCEX11_LOG_TRACE ("PlanLauncher::main");

  DANCEX11_LOG_DEBUG ("Starting plan_launcher.");

  int retval = 0;

  DANCEX11_DEFINE_DEPLOYMENT_STATE (Plan_Launcher);

  try
  {

    DANCEX11_LOG_TRACE ("PlanLauncher - initializing module instance");

    DAnCEX11::PlanLauncher_Module nm;

    if (nm.init (argc, argv))
    {
      DANCEX11_LOG_TRACE ("PlanLauncher - running module instance");
      if (!nm.run ())
        retval = 1;
    }
    else
    {
      retval = 1;
    }

    DANCEX11_LOG_TRACE ("PlanLauncher - exiting");
  }
  catch (const CORBA::Exception& ex)
  {
    DANCEX11_LOG_PANIC ("PlanLauncher - Error, CORBA Exception: " << ex);
    retval = 1;
  }
  catch (const std::exception& ex)
  {
    DANCEX11_LOG_PANIC ("PlanLauncher - Error, std::exception: " << ex.what ());
    retval = 1;
  }
  catch (...)
  {
    DANCEX11_LOG_PANIC ("PlanLauncher - Error: Unknown exception.");
    retval = 1;
  }

  return retval;
}
