// -*- C++ -*-
/**
 * @file artifact_installation_handler_svc.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "artifact_installation_handler_svc.h"
#include "artifact_installation_impl.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  /*
   *  ArtifactInstallationHandlerSvc
   */

  ArtifactInstallationHandlerSvc::ArtifactInstallationHandlerSvc ()
    {
    }

  ArtifactInstallationHandlerSvc::~ArtifactInstallationHandlerSvc ()
    {
    }

  /// Initialization hook.
  int ArtifactInstallationHandlerSvc::init (int /*argc*/, ACE_TCHAR * /*argv*/[])
    {
      DANCEX11_LOG_TRACE ( "ArtifactInstallationHandlerSvc::init");
      return ArtifactInstallation_Impl::register_handler (this->handler_instance ());
    }

  /// Deinitialization hook.
  int ArtifactInstallationHandlerSvc::fini ()
    {
      DANCEX11_LOG_TRACE ( "ArtifactInstallationHandlerSvc::fini");
      return ArtifactInstallation_Impl::deregister_handler (this->handler_instance ());
    }
}
