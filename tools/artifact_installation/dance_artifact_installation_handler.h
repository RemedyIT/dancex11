// -*- C++ -*-
/**
 * @file dance_artifact_installation_handler.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_ARTIFACT_INSTALLATION_HANDLER_H
#define DANCEX11_ARTIFACT_INSTALLATION_HANDLER_H

#include "dance_artifact_installation_handlerC.h"
#include "dance_artifact_installation_export.h"
#include <map>
#include <string>

namespace DAnCEX11
{
  class DAnCE_Artifact_Installation_Export ArtifactInstallationHandler
  {
    public:
      typedef std::map<std::string,
                       std::string> TPropertyMap;

      virtual ~ArtifactInstallationHandler () = default;

      virtual const std::string& protocol_prefix () = 0;

      virtual void initialize () = 0;

      virtual void clear () = 0;

      virtual void install (const std::string& plan_uuid,
                            std::string& location,
                            const TPropertyMap& properties) = 0;

      virtual void remove (const std::string& plan_uuid,
                           const std::string& location) = 0;

    protected:
      std::string basename (const std::string& location, char dirsep = ACE_DIRECTORY_SEPARATOR_CHAR_A);
  };

  class DAnCE_Artifact_Installation_Export ArtifactInstallationProperties
  {
    public:
      using TPropertyMap = ArtifactInstallationHandler::TPropertyMap;

      ArtifactInstallationProperties (const TPropertyMap& propmap);
      ArtifactInstallationProperties (const TPropertyMap& propmap, const std::string& protocol);

      static const std::string PROPERTY_BASE; // nl.remedy.it.DnCX11.artifact.
      static const std::string PROP_KEEP_INTERMEDIATES;
      static const std::string PROP_NAME;
      static const std::string PROP_PATH;
      static const std::string PROP_EXCLUSIVE;

      bool has_property (const std::string& propname);
      const std::string& get_property (const std::string& propname);

    private:
      static const std::string empty_;

      const TPropertyMap& propmap_;
      std::string protocol_;
  };

  inline
  std::string ArtifactInstallationHandler::basename (const std::string& location, char dirsep)
  {
    std::string::size_type const n = location.rfind (dirsep);
    return (n == std::string::npos) ? location : location.substr (n+1);
  }

  inline
  bool ArtifactInstallationProperties::has_property (const std::string& propname)
  {
    TPropertyMap::const_iterator it = this->propmap_.find (PROPERTY_BASE+propname);
    if (it == this->propmap_.end () && !this->protocol_.empty ())
    {
      it = this->propmap_.find (PROPERTY_BASE+this->protocol_+propname);
    }
    return it != this->propmap_.end ();
  }

  inline
  const std::string& ArtifactInstallationProperties::get_property (const std::string& propname)
  {
    TPropertyMap::const_iterator it = this->propmap_.find (PROPERTY_BASE+propname);
    if (it == this->propmap_.end () && !this->protocol_.empty ())
    {
      it = this->propmap_.find (PROPERTY_BASE+this->protocol_+propname);
    }
    if (it != this->propmap_.end ())
      return it->second;
    else
      return empty_;
  }

}

#endif /* DANCEX11_ARTIFACT_INSTALLATION_HANDLER_H */
