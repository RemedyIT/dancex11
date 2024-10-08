/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#ifndef IAD_HPP
#define IAD_HPP

#include "xsc_xml_handlers_export.h"
// Forward declarations.
namespace DAnCE
{
  namespace Config_Handlers
  {
    class NamedImplementationArtifact;
    class ImplementationArtifactDescription;
  }
}

#include <memory>
#include <string>
#include <vector>
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XMLSchema/id_map.hpp"
#include "tao/x11/base/stddef.h"
#include "Basic_Deployment_Data.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    class XSC_XML_Handlers_Export NamedImplementationArtifact : public ::XSCRT::Type
    {
      using Base = ::XSCRT::Type;

      // name
      public:
      ::XMLSchema::string<char> const& name () const;
      void name (::XMLSchema::string<char> const&);

      protected:
      using name_type = std::unique_ptr<::XMLSchema::string<char>>;
      name_type name_;

      // referencedArtifact
      public:
      ::DAnCE::Config_Handlers::ImplementationArtifactDescription const& referencedArtifact () const;
      void referencedArtifact (::DAnCE::Config_Handlers::ImplementationArtifactDescription const&);

      protected:
      using referencedArtifact_type = std::unique_ptr<::DAnCE::Config_Handlers::ImplementationArtifactDescription>;
      referencedArtifact_type referencedArtifact_;

      public:
      NamedImplementationArtifact (::XMLSchema::string<char> const& name__,
                                   ::DAnCE::Config_Handlers::ImplementationArtifactDescription const& referencedArtifact__);

      explicit NamedImplementationArtifact (::XSCRT::XML::Element<char> const&);
      NamedImplementationArtifact (NamedImplementationArtifact const& s);
      NamedImplementationArtifact& operator= (NamedImplementationArtifact const& s);

      NamedImplementationArtifact (NamedImplementationArtifact&&) = default;
      NamedImplementationArtifact& operator= (NamedImplementationArtifact&&) = default;
    };


    class XSC_XML_Handlers_Export ImplementationArtifactDescription : public ::XSCRT::Type
    {
      using Base = ::XSCRT::Type;

      // label
      public:
      bool label_p () const;
      ::XMLSchema::string<char> const& label () const;
      void label (::XMLSchema::string<char> const&);

      protected:
      using label_type = std::unique_ptr<::XMLSchema::string<char>>;
      label_type label_;

      // UUID
      public:
      bool UUID_p () const;
      ::XMLSchema::string<char> const& UUID () const;
      void UUID (::XMLSchema::string<char> const&);

      protected:
      using UUID_type = std::unique_ptr<::XMLSchema::string<char>>;
      UUID_type UUID_;

      // location
      public:
      using location_container_type = std::vector<::XMLSchema::string<char>>;
      using location_const_iterator = location_container_type::const_iterator;
      location_const_iterator begin_location () const;
      location_const_iterator end_location () const;
      size_t count_location () const;

      protected:
      location_container_type location_;

      // dependsOn
      public:
      using dependsOn_container_type = std::vector<::DAnCE::Config_Handlers::NamedImplementationArtifact>;
      using dependsOn_const_iterator = dependsOn_container_type::const_iterator;
      dependsOn_const_iterator begin_dependsOn () const;
      dependsOn_const_iterator end_dependsOn () const;
      size_t count_dependsOn () const;

      protected:
      dependsOn_container_type dependsOn_;

      // execParameter
      public:
      using execParameter_container_type = std::vector<::DAnCE::Config_Handlers::Property>;
      using execParameter_const_iterator = execParameter_container_type::const_iterator;
      execParameter_const_iterator begin_execParameter () const;
      execParameter_const_iterator end_execParameter () const;
      size_t count_execParameter () const;

      protected:
      execParameter_container_type execParameter_;

      // infoProperty
      public:
      using infoProperty_container_type = std::vector<::DAnCE::Config_Handlers::Property>;
      using infoProperty_const_iterator = infoProperty_container_type::const_iterator;
      infoProperty_const_iterator begin_infoProperty () const;
      infoProperty_const_iterator end_infoProperty () const;
      size_t count_infoProperty () const;

      protected:
      infoProperty_container_type infoProperty_;

      // deployRequirement
      public:
      using deployRequirement_container_type = std::vector<::DAnCE::Config_Handlers::Requirement>;
      using deployRequirement_const_iterator = deployRequirement_container_type::const_iterator;
      deployRequirement_const_iterator begin_deployRequirement () const;
      deployRequirement_const_iterator end_deployRequirement () const;
      size_t count_deployRequirement () const;

      protected:
      deployRequirement_container_type deployRequirement_;

      // contentLocation
      public:
      bool contentLocation_p () const;
      ::XMLSchema::string<char> const& contentLocation () const;
      void contentLocation (::XMLSchema::string<char> const&);

      protected:
      using contentLocation_type = std::unique_ptr<::XMLSchema::string<char>>;
      contentLocation_type contentLocation_;

      // href
      public:
      bool href_p () const;
      ::XMLSchema::string<char> const& href () const;
      ::XMLSchema::string<char>& href ();
      void href (::XMLSchema::string<char> const&);

      protected:
      using href_type = std::unique_ptr<::XMLSchema::string<char>>;
      href_type href_;

      public:
      ImplementationArtifactDescription ();

      explicit ImplementationArtifactDescription (::XSCRT::XML::Element<char> const&);
      ImplementationArtifactDescription (ImplementationArtifactDescription const& s);
      ImplementationArtifactDescription& operator= (ImplementationArtifactDescription const& s);

      ImplementationArtifactDescription (ImplementationArtifactDescription&&) = default;
      ImplementationArtifactDescription& operator= (ImplementationArtifactDescription&&) = default;
    };
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
  }
}

#endif // IAD_HPP
