// -*- C++ -*-
/**
 * @file http_installation_handler.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_HTTP_INSTALLATION_HANDLER_H
#define DANCEX11_HTTP_INSTALLATION_HANDLER_H

#include "artifact_installation_handler_svc.h"
#include "artifact_installation_handler_export.h"
#include <string>

namespace DAnCEX11
  {
    class Artifact_Installation_Handler_Export HttpInstallationHandler
      : public ArtifactInstallationHandler
      {
        public:
          static const std::string protocol;

          virtual ~HttpInstallationHandler () = default;

          const std::string& protocol_prefix () override;

          void initialize () override;

          void clear () override;

          void install (const std::string& plan_uuid,
                        std::string& location,
                        const TPropertyMap& properties) override;

          void remove (const std::string& plan_uuid,
                       const std::string& location) override;

          static HttpInstallationHandler* getInstance ();

        private:
          HttpInstallationHandler () = default;
      };

    class Artifact_Installation_Handler_Export HttpInstallationHandlerSvc
      : public ArtifactInstallationHandlerSvc
      {
        public:
          HttpInstallationHandlerSvc () = default;
          virtual ~HttpInstallationHandlerSvc () = default;

          ArtifactInstallationHandler* handler_instance () override;

          static int Initializer ();
      };

    static int DAnCE_Requires_HttpInstallationHandler_Service_Initializer =
      HttpInstallationHandlerSvc::Initializer ();

    ACE_STATIC_SVC_DECLARE (HttpInstallationHandlerSvc)
    ACE_FACTORY_DECLARE (Artifact_Installation_Handler, HttpInstallationHandlerSvc)

  }

#endif /* DANCEX11_HTTP_INSTALLATION_HANDLER_H */
