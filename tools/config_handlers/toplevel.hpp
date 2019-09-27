/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#ifndef TOPLEVEL_HPP
#define TOPLEVEL_HPP

#include "xsc_xml_handlers_export.h"
// Forward declarations.
namespace DAnCE
{
  namespace Config_Handlers
  {
    class TopLevelPackageDescription;
  }
}

#include <memory>
#include <string>
#include <vector>
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XMLSchema/id_map.hpp"
#include "tao/x11/stddef.h"
#include "pcd.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    class XSC_XML_Handlers_Export TopLevelPackageDescription : public ::XSCRT::Type
    {
      using Base = ::XSCRT::Type;

      // package
      public:
      using package_container_type = std::vector< ::DAnCE::Config_Handlers::PackageConfiguration>;
      using package_const_iterator = package_container_type::const_iterator;
      package_const_iterator begin_package () const;
      package_const_iterator end_package () const;
      size_t count_package () const;

      protected:
      package_container_type package_;

      public:
      TopLevelPackageDescription ();

      explicit TopLevelPackageDescription (::XSCRT::XML::Element<char> const&);
      TopLevelPackageDescription (TopLevelPackageDescription const& s);
      TopLevelPackageDescription& operator= (TopLevelPackageDescription const& s);

      TopLevelPackageDescription (TopLevelPackageDescription&&) = default;
      TopLevelPackageDescription& operator= (TopLevelPackageDescription&&) = default;
    };
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
  }
}

#endif // TOPLEVEL_HPP
