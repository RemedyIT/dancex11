// -*- C++ -*-
/**
 * @file dance_artifact_installation_handler.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dance_artifact_installation_handler.h"

namespace DAnCEX11
{
  const std::string ArtifactInstallationProperties::PROPERTY_BASE = "nl.remedy.it.DnCX11.artifact.";
  const std::string ArtifactInstallationProperties::PROP_KEEP_INTERMEDIATES = "keep_intermediates";
  const std::string ArtifactInstallationProperties::PROP_NAME = "name";
  const std::string ArtifactInstallationProperties::PROP_PATH = "path";
  const std::string ArtifactInstallationProperties::PROP_EXCLUSIVE = "exclusive";

  const std::string ArtifactInstallationProperties::empty_;

  ArtifactInstallationProperties::ArtifactInstallationProperties (const TPropertyMap& propmap)
    : propmap_ (propmap)
    {
    }

  ArtifactInstallationProperties::ArtifactInstallationProperties (const TPropertyMap& propmap, const std::string& protocol)
    : propmap_ (propmap),
      protocol_ (protocol)
    {
      if (!this->protocol_.empty ())
        this->protocol_ += '.';
    }

}
