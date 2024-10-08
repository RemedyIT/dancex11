/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#ifndef XMI_HPP
#define XMI_HPP

#include "xsc_xml_handlers_export.h"
// Forward declarations.
namespace XMI
{
  class Extension;
}

#include <memory>
#include <string>
#include <vector>
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XMLSchema/id_map.hpp"
#include "tao/x11/base/stddef.h"
namespace XMI
{
  class XSC_XML_Handlers_Export Extension : public ::XSCRT::Type
  {
    using Base = ::XSCRT::Type;

    // id
    public:
    bool id_p () const;
    ::XMLSchema::ID<char> const& id () const;
    ::XMLSchema::ID<char>& id ();
    void id (::XMLSchema::ID<char> const&);

    protected:
    using id_type = std::unique_ptr<::XMLSchema::ID<char>>;
    id_type id_;

    // label
    public:
    bool label_p () const;
    ::XMLSchema::string<char> const& label () const;
    ::XMLSchema::string<char>& label ();
    void label (::XMLSchema::string<char> const&);

    protected:
    using label_type = std::unique_ptr<::XMLSchema::string<char>>;
    label_type label_;

    // uuid
    public:
    bool uuid_p () const;
    ::XMLSchema::string<char> const& uuid () const;
    ::XMLSchema::string<char>& uuid ();
    void uuid (::XMLSchema::string<char> const&);

    protected:
    using uuid_type = std::unique_ptr<::XMLSchema::string<char>>;
    uuid_type uuid_;

    // href
    public:
    bool href_p () const;
    ::XMLSchema::string<char> const& href () const;
    ::XMLSchema::string<char>& href ();
    void href (::XMLSchema::string<char> const&);

    protected:
    using href_type = std::unique_ptr<::XMLSchema::string<char>>;
    href_type href_;

    // idref
    public:
    bool idref_p () const;
    ::XMLSchema::IDREF<char> const& idref () const;
    ::XMLSchema::IDREF<char>& idref ();
    void idref (::XMLSchema::IDREF<char> const&);

    protected:
    using idref_type = std::unique_ptr<::XMLSchema::IDREF<char>>;
    idref_type idref_;

    // version
    public:
    bool version_p () const;
    ::XMLSchema::string<char> const& version () const;
    ::XMLSchema::string<char>& version ();
    void version (::XMLSchema::string<char> const&);

    protected:
    using version_type = std::unique_ptr<::XMLSchema::string<char>>;
    version_type version_;

    // extender
    public:
    bool extender_p () const;
    ::XMLSchema::string<char> const& extender () const;
    ::XMLSchema::string<char>& extender ();
    void extender (::XMLSchema::string<char> const&);

    protected:
    using extender_type = std::unique_ptr<::XMLSchema::string<char>>;
    extender_type extender_;

    // extenderID
    public:
    bool extenderID_p () const;
    ::XMLSchema::string<char> const& extenderID () const;
    ::XMLSchema::string<char>& extenderID ();
    void extenderID (::XMLSchema::string<char> const&);

    protected:
    using extenderID_type = std::unique_ptr<::XMLSchema::string<char>>;
    extenderID_type extenderID_;

    public:
    Extension ();

    explicit Extension (::XSCRT::XML::Element<char> const&);
    Extension (Extension const& s);
    Extension& operator= (Extension const& s);

    Extension (Extension&&) = default;
    Extension& operator= (Extension&&) = default;
  };
}

namespace XMI
{
  namespace reader
  {
    XSC_XML_Handlers_Export
    ::XMI::Extension
    extension (xercesc::DOMDocument const*);
  }
}

#endif // XMI_HPP
