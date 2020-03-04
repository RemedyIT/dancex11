// -*- C++ -*-
/**
 * @file artifact_installation_handler_svc.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_ARTIFACT_INSTALLATION_HANDLER_SVC_H
#define DANCEX11_ARTIFACT_INSTALLATION_HANDLER_SVC_H

#include "ace/Service_Object.h"
#include "dance_artifact_installation_handler.h"

#include "dance_artifact_installation_export.h"

namespace DAnCEX11
  {
    class DAnCE_Artifact_Installation_Export ArtifactInstallationHandlerSvc
      : public ACE_Service_Object
      {
        public:
          ArtifactInstallationHandlerSvc ();
          virtual ~ArtifactInstallationHandlerSvc ();

          /// Initialization hook.
          int init (int argc, ACE_TCHAR *argv[]) override;
          /// Deinitialization hook.
          int fini () override;

          virtual ArtifactInstallationHandler* handler_instance () = 0;
      };
  }

#endif /* DANCEX11_ARTIFACT_INSTALLATION_HANDLER_SVC_H */
