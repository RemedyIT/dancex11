/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#ifndef CCD_HPP
#define CCD_HPP

#include "xsc_xml_handlers_export.h"
// Forward declarations.
namespace DAnCE
{
  namespace Config_Handlers
  {
    class ComponentInterfaceDescription;
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
    class XSC_XML_Handlers_Export ComponentInterfaceDescription : public ::XSCRT::Type
    {
      using Base = ::XSCRT::Type;

      // label
      public:
      bool label_p () const;
      ::XMLSchema::string<char> const& label () const;
      void label (::XMLSchema::string<char> const& e);

      protected:
      using label_type = std::unique_ptr<::XMLSchema::string<char>>;
      label_type label_;

      // UUID
      public:
      bool UUID_p () const;
      ::XMLSchema::string<char> const& UUID () const;
      void UUID (::XMLSchema::string<char> const& e);

      protected:
      using UUID_type = std::unique_ptr<::XMLSchema::string<char>>;
      UUID_type UUID_;

      // specificType
      public:
      bool specificType_p () const;
      ::XMLSchema::string<char> const& specificType () const;
      void specificType (::XMLSchema::string<char> const& e);

      protected:
      using specificType_type = std::unique_ptr<::XMLSchema::string<char>>;
      specificType_type specificType_;

      // supportedType
      public:
      using supportedType_container_type = std::vector< ::XMLSchema::string<char>>;
      using supportedType_const_iterator = supportedType_container_type::const_iterator;
      supportedType_const_iterator begin_supportedType () const;
      supportedType_const_iterator end_supportedType () const;
      size_t count_supportedType () const;

      protected:
      supportedType_container_type supportedType_;

      // idlFile
      public:
      using idlFile_container_type = std::vector< ::XMLSchema::string<char>>;
      using idlFile_const_iterator = idlFile_container_type::const_iterator;
      idlFile_const_iterator begin_idlFile () const;
      idlFile_const_iterator end_idlFile () const;
      size_t count_idlFile () const;

      protected:
      idlFile_container_type idlFile_;

      // configProperty
      public:
      using configProperty_container_type = std::vector< ::DAnCE::Config_Handlers::Property>;
      using configProperty_const_iterator = configProperty_container_type::const_iterator;
      configProperty_const_iterator begin_configProperty () const;
      configProperty_const_iterator end_configProperty () const;
      size_t count_configProperty () const;

      protected:
      configProperty_container_type configProperty_;

      // port
      public:
      using port_container_type = std::vector< ::DAnCE::Config_Handlers::ComponentPortDescription>;
      using port_const_iterator = port_container_type::const_iterator;
      port_const_iterator begin_port () const;
      port_const_iterator end_port () const;
      size_t count_port () const;

      protected:
      port_container_type port_;

      // property
      public:
      using property_container_type = std::vector< ::DAnCE::Config_Handlers::ComponentPropertyDescription>;
      using property_const_iterator = property_container_type::const_iterator;
      property_const_iterator begin_property () const;
      property_const_iterator end_property () const;
      size_t count_property () const;

      protected:
      property_container_type property_;

      // infoProperty
      public:
      using infoProperty_container_type = std::vector< ::DAnCE::Config_Handlers::Property>;
      using infoProperty_const_iterator = infoProperty_container_type::const_iterator;
      infoProperty_const_iterator begin_infoProperty () const;
      infoProperty_const_iterator end_infoProperty () const;
      size_t count_infoProperty () const;

      protected:
      infoProperty_container_type infoProperty_;

      // contentLocation
      public:
      bool contentLocation_p () const;
      ::XMLSchema::string<char> const& contentLocation () const;
      void contentLocation (::XMLSchema::string<char> const& e);

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
      ComponentInterfaceDescription ();

      explicit ComponentInterfaceDescription (::XSCRT::XML::Element<char> const&);
      ComponentInterfaceDescription (ComponentInterfaceDescription const& s);
      ComponentInterfaceDescription& operator= (ComponentInterfaceDescription const& s);

      ComponentInterfaceDescription (ComponentInterfaceDescription&&) = default;
      ComponentInterfaceDescription& operator= (ComponentInterfaceDescription&&) = default;
    };
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
  }
}

#endif // CCD_HPP
