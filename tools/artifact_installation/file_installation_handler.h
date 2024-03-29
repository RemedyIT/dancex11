// -*- C++ -*-
/**
 * @file file_installation_handler.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_FILE_INSTALLATION_HANDLER_H
#define DANCEX11_FILE_INSTALLATION_HANDLER_H

#include "artifact_installation_handler_svc.h"
#include "artifact_installation_handler_export.h"
#include <string>

namespace DAnCEX11
  {
    class Artifact_Installation_Handler_Export FileInstallationHandler
          : public ArtifactInstallationHandler
      {
        public:
          static const std::string protocol;

          ~FileInstallationHandler () override = default;

          const std::string& protocol_prefix () override;

          void initialize () override;

          void clear () override;

          void install (const std::string& plan_uuid,
                        std::string& location,
                        const TPropertyMap& properties) override;

          void remove (const std::string& plan_uuid,
                       const std::string& location) override;

          static FileInstallationHandler* getInstance ();
        private:
          FileInstallationHandler () = default;
      };

    class Artifact_Installation_Handler_Export FileInstallationHandlerSvc
          : public ArtifactInstallationHandlerSvc
      {
        public:
          FileInstallationHandlerSvc () = default;
          ~FileInstallationHandlerSvc () override = default;

          ArtifactInstallationHandler* handler_instance () override;

          static int Initializer ();
      };

    static int DAnCE_Requires_FileInstallationHandler_Service_Initializer =
      FileInstallationHandlerSvc::Initializer ();

    ACE_STATIC_SVC_DECLARE (FileInstallationHandlerSvc)
    ACE_FACTORY_DECLARE (Artifact_Installation_Handler, FileInstallationHandlerSvc)
  }

#endif /* DANCEX11_FILE_INSTALLATION_HANDLER_H */
