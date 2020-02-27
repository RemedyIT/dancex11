/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#include "pcd.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    // ComponentPackageImport

    ComponentPackageImport::ComponentPackageImport ()
    : ::XSCRT::Type ()
    {
    }

    ComponentPackageImport::ComponentPackageImport (ComponentPackageImport const& s) :
    ::XSCRT::Type (s)
    , location_ (s.location_)
    {
    }

    ComponentPackageImport&
    ComponentPackageImport::operator= (ComponentPackageImport const& s)
    {
      if (std::addressof(s) != this)
      {
        location_ = s.location_;
      }

      return *this;
    }


    // ComponentPackageImport
    ComponentPackageImport::location_const_iterator ComponentPackageImport::
    begin_location () const
    {
      return location_.cbegin ();
    }

    ComponentPackageImport::location_const_iterator ComponentPackageImport::
    end_location () const
    {
      return location_.cend ();
    }

    size_t ComponentPackageImport::
    count_location(void) const
    {
      return location_.size ();
    }


    // PackageConfiguration

    PackageConfiguration::PackageConfiguration ()
    : ::XSCRT::Type ()
    {
    }

    PackageConfiguration::PackageConfiguration (PackageConfiguration const& s) :
    ::XSCRT::Type (s)
    , label_ (s.label_ ? std::make_unique< ::XMLSchema::string<char>> (*s.label_) : nullptr)
    , UUID_ (s.UUID_ ? std::make_unique< ::XMLSchema::string<char>> (*s.UUID_) : nullptr)
    , basePackage_ (s.basePackage_ ? std::make_unique< ::DAnCE::Config_Handlers::ComponentPackageDescription> (*s.basePackage_) : nullptr)
    , specializedConfig_ (s.specializedConfig_ ? std::make_unique< ::DAnCE::Config_Handlers::PackageConfiguration> (*s.specializedConfig_) : nullptr)
    , importedPackage_ (s.importedPackage_ ? std::make_unique< ::DAnCE::Config_Handlers::ComponentPackageImport> (*s.importedPackage_) : nullptr)
    , referencedPackage_ (s.referencedPackage_ ? std::make_unique< ::DAnCE::Config_Handlers::ComponentPackageReference> (*s.referencedPackage_) : nullptr)
    , selectRequirement_ (s.selectRequirement_)
    , configProperty_ (s.configProperty_)
    , contentLocation_ (s.contentLocation_ ? std::make_unique< ::XMLSchema::string<char>> (*s.contentLocation_) : nullptr)
    {
    }

    PackageConfiguration&
    PackageConfiguration::operator= (PackageConfiguration const& s)
    {
      if (std::addressof(s) != this)
      {
        if (s.label_)
          label (*(s.label_));
        else
          label_.reset (nullptr);

        if (s.UUID_)
          UUID (*(s.UUID_));
        else
          UUID_.reset (nullptr);

        if (s.basePackage_)
          basePackage (*(s.basePackage_));
        else
          basePackage_.reset (nullptr);

        if (s.specializedConfig_)
          specializedConfig (*(s.specializedConfig_));
        else
          specializedConfig_.reset (nullptr);

        if (s.importedPackage_)
          importedPackage (*(s.importedPackage_));
        else
          importedPackage_.reset (nullptr);

        if (s.referencedPackage_)
          referencedPackage (*(s.referencedPackage_));
        else
          referencedPackage_.reset (nullptr);

        selectRequirement_ = s.selectRequirement_;

        configProperty_ = s.configProperty_;

        if (s.contentLocation_)
          contentLocation (*(s.contentLocation_));
        else
          contentLocation_.reset (nullptr);
      }

      return *this;
    }


    // PackageConfiguration
    bool PackageConfiguration::
    label_p () const
    {
      return !!label_;
    }

    ::XMLSchema::string<char> const& PackageConfiguration::
    label () const
    {
      return *label_;
    }

    void PackageConfiguration::
    label (::XMLSchema::string<char> const& e)
    {
      if (label_)
      {
        *label_ = e;
      }

      else
      {
        label_ = std::make_unique< ::XMLSchema::string<char>> (e);
      }
    }

    // PackageConfiguration
    bool PackageConfiguration::
    UUID_p () const
    {
      return !!UUID_;
    }

    ::XMLSchema::string<char> const& PackageConfiguration::
    UUID () const
    {
      return *UUID_;
    }

    void PackageConfiguration::
    UUID (::XMLSchema::string<char> const& e)
    {
      if (UUID_)
      {
        *UUID_ = e;
      }

      else
      {
        UUID_ = std::make_unique< ::XMLSchema::string<char>> (e);
      }
    }

    // PackageConfiguration
    bool PackageConfiguration::
    basePackage_p () const
    {
      return !!basePackage_;
    }

    ::DAnCE::Config_Handlers::ComponentPackageDescription const& PackageConfiguration::
    basePackage () const
    {
      return *basePackage_;
    }

    void PackageConfiguration::
    basePackage (::DAnCE::Config_Handlers::ComponentPackageDescription const& e)
    {
      if (basePackage_)
      {
        *basePackage_ = e;
      }

      else
      {
        basePackage_ = std::make_unique< ::DAnCE::Config_Handlers::ComponentPackageDescription> (e);
      }
    }

    // PackageConfiguration
    bool PackageConfiguration::
    specializedConfig_p () const
    {
      return !!specializedConfig_;
    }

    ::DAnCE::Config_Handlers::PackageConfiguration const& PackageConfiguration::
    specializedConfig () const
    {
      return *specializedConfig_;
    }

    void PackageConfiguration::
    specializedConfig (::DAnCE::Config_Handlers::PackageConfiguration const& e)
    {
      if (specializedConfig_)
      {
        *specializedConfig_ = e;
      }

      else
      {
        specializedConfig_ = std::make_unique< ::DAnCE::Config_Handlers::PackageConfiguration> (e);
      }
    }

    // PackageConfiguration
    bool PackageConfiguration::
    importedPackage_p () const
    {
      return !!importedPackage_;
    }

    ::DAnCE::Config_Handlers::ComponentPackageImport const& PackageConfiguration::
    importedPackage () const
    {
      return *importedPackage_;
    }

    void PackageConfiguration::
    importedPackage (::DAnCE::Config_Handlers::ComponentPackageImport const& e)
    {
      if (importedPackage_)
      {
        *importedPackage_ = e;
      }

      else
      {
        importedPackage_ = std::make_unique< ::DAnCE::Config_Handlers::ComponentPackageImport> (e);
      }
    }

    // PackageConfiguration
    bool PackageConfiguration::
    referencedPackage_p () const
    {
      return !!referencedPackage_;
    }

    ::DAnCE::Config_Handlers::ComponentPackageReference const& PackageConfiguration::
    referencedPackage () const
    {
      return *referencedPackage_;
    }

    void PackageConfiguration::
    referencedPackage (::DAnCE::Config_Handlers::ComponentPackageReference const& e)
    {
      if (referencedPackage_)
      {
        *referencedPackage_ = e;
      }

      else
      {
        referencedPackage_ = std::make_unique< ::DAnCE::Config_Handlers::ComponentPackageReference> (e);
      }
    }

    // PackageConfiguration
    PackageConfiguration::selectRequirement_const_iterator PackageConfiguration::
    begin_selectRequirement () const
    {
      return selectRequirement_.cbegin ();
    }

    PackageConfiguration::selectRequirement_const_iterator PackageConfiguration::
    end_selectRequirement () const
    {
      return selectRequirement_.cend ();
    }

    size_t PackageConfiguration::
    count_selectRequirement(void) const
    {
      return selectRequirement_.size ();
    }

    // PackageConfiguration
    PackageConfiguration::configProperty_const_iterator PackageConfiguration::
    begin_configProperty () const
    {
      return configProperty_.cbegin ();
    }

    PackageConfiguration::configProperty_const_iterator PackageConfiguration::
    end_configProperty () const
    {
      return configProperty_.cend ();
    }

    size_t PackageConfiguration::
    count_configProperty(void) const
    {
      return configProperty_.size ();
    }

    // PackageConfiguration
    bool PackageConfiguration::
    contentLocation_p () const
    {
      return !!contentLocation_;
    }

    ::XMLSchema::string<char> const& PackageConfiguration::
    contentLocation () const
    {
      return *contentLocation_;
    }

    void PackageConfiguration::
    contentLocation (::XMLSchema::string<char> const& e)
    {
      if (contentLocation_)
      {
        *contentLocation_ = e;
      }

      else
      {
        contentLocation_ = std::make_unique< ::XMLSchema::string<char>> (e);
      }
    }
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
    // ComponentPackageImport

    ComponentPackageImport::
    ComponentPackageImport (::XSCRT::XML::Element<char> const& e)
    :Base (e)
    {

      ::XSCRT::Parser<char> p (e);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "location")
        {
          ::XMLSchema::string<char> t (e);
          location_.push_back (t);
        }

        else
        {
        }
      }
    }

    // PackageConfiguration

    PackageConfiguration::
    PackageConfiguration (::XSCRT::XML::Element<char> const& e)
    :Base (e)
    {

      ::XSCRT::Parser<char> p (e);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "label")
        {
          ::XMLSchema::string<char> t (e);
          label (t);
        }

        else if (n == "UUID")
        {
          ::XMLSchema::string<char> t (e);
          UUID (t);
        }

        else if (n == "basePackage")
        {
          ::DAnCE::Config_Handlers::ComponentPackageDescription t (e);
          basePackage (t);
        }

        else if (n == "specializedConfig")
        {
          ::DAnCE::Config_Handlers::PackageConfiguration t (e);
          specializedConfig (t);
        }

        else if (n == "importedPackage")
        {
          ::DAnCE::Config_Handlers::ComponentPackageImport t (e);
          importedPackage (t);
        }

        else if (n == "referencedPackage")
        {
          ::DAnCE::Config_Handlers::ComponentPackageReference t (e);
          referencedPackage (t);
        }

        else if (n == "selectRequirement")
        {
          ::DAnCE::Config_Handlers::Requirement t (e);
          selectRequirement_.push_back (t);
        }

        else if (n == "configProperty")
        {
          ::DAnCE::Config_Handlers::Property t (e);
          configProperty_.push_back (t);
        }

        else if (n == "contentLocation")
        {
          ::XMLSchema::string<char> t (e);
          contentLocation (t);
        }

        else
        {
        }
      }
    }
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
  }
}

