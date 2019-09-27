// -*- C++ -*-
/**
 * @file http_installation_handler.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
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

          virtual const std::string& protocol_prefix ();

          virtual void initialize ();

          virtual void clear ();

          virtual void install (const std::string& plan_uuid,
                                std::string& location,
                                const TPropertyMap& properties);

          virtual void remove (const std::string& plan_uuid,
                               const std::string& location);

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

          virtual ArtifactInstallationHandler* handler_instance ();

          static int Initializer ();
      };

    static int DAnCE_Requires_HttpInstallationHandler_Service_Initializer =
      HttpInstallationHandlerSvc::Initializer ();

    ACE_STATIC_SVC_DECLARE (HttpInstallationHandlerSvc)
    ACE_FACTORY_DECLARE (Artifact_Installation_Handler, HttpInstallationHandlerSvc)

  }

#endif /* DANCEX11_HTTP_INSTALLATION_HANDLER_H */
