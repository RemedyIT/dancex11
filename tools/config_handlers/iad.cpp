/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#include "iad.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    // NamedImplementationArtifact

    NamedImplementationArtifact::NamedImplementationArtifact (::XMLSchema::string<char> const& name__,
                                                              ::DAnCE::Config_Handlers::ImplementationArtifactDescription const& referencedArtifact__)
    : ::XSCRT::Type ()
    , name_ (std::make_unique< ::XMLSchema::string<char>> (name__))
    , referencedArtifact_ (std::make_unique< ::DAnCE::Config_Handlers::ImplementationArtifactDescription> (referencedArtifact__))
    {
    }

    NamedImplementationArtifact::NamedImplementationArtifact (NamedImplementationArtifact const& s) :
    ::XSCRT::Type (s)
    , name_ (std::make_unique< ::XMLSchema::string<char>> (*s.name_))
    , referencedArtifact_ (std::make_unique< ::DAnCE::Config_Handlers::ImplementationArtifactDescription> (*s.referencedArtifact_))
    {
    }

    NamedImplementationArtifact&
    NamedImplementationArtifact::operator= (NamedImplementationArtifact const& s)
    {
      if (std::addressof(s) != this)
      {
        name (*s.name_);

        referencedArtifact (*s.referencedArtifact_);
      }

      return *this;
    }


    // NamedImplementationArtifact
    ::XMLSchema::string<char> const& NamedImplementationArtifact::
    name () const
    {
      return *name_;
    }

    void NamedImplementationArtifact::
    name (::XMLSchema::string<char> const& e)
    {
      *name_ = e;
    }

    // NamedImplementationArtifact
    ::DAnCE::Config_Handlers::ImplementationArtifactDescription const& NamedImplementationArtifact::
    referencedArtifact () const
    {
      return *referencedArtifact_;
    }

    void NamedImplementationArtifact::
    referencedArtifact (::DAnCE::Config_Handlers::ImplementationArtifactDescription const& e)
    {
      *referencedArtifact_ = e;
    }


    // ImplementationArtifactDescription

    ImplementationArtifactDescription::ImplementationArtifactDescription ()
    : ::XSCRT::Type ()
    {
    }

    ImplementationArtifactDescription::ImplementationArtifactDescription (ImplementationArtifactDescription const& s) :
    ::XSCRT::Type (s)
    , label_ (s.label_ ? std::make_unique< ::XMLSchema::string<char>> (*s.label_) : nullptr)
    , UUID_ (s.UUID_ ? std::make_unique< ::XMLSchema::string<char>> (*s.UUID_) : nullptr)
    , location_ (s.location_)
    , dependsOn_ (s.dependsOn_)
    , execParameter_ (s.execParameter_)
    , infoProperty_ (s.infoProperty_)
    , deployRequirement_ (s.deployRequirement_)
    , contentLocation_ (s.contentLocation_ ? std::make_unique< ::XMLSchema::string<char>> (*s.contentLocation_) : nullptr)
    , href_ (s.href_ ? std::make_unique< ::XMLSchema::string<char>> (*s.href_) : nullptr)
    {
    }

    ImplementationArtifactDescription&
    ImplementationArtifactDescription::operator= (ImplementationArtifactDescription const& s)
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

        location_ = s.location_;

        dependsOn_ = s.dependsOn_;

        execParameter_ = s.execParameter_;

        infoProperty_ = s.infoProperty_;

        deployRequirement_ = s.deployRequirement_;

        if (s.contentLocation_)
          contentLocation (*(s.contentLocation_));
        else
          contentLocation_.reset (nullptr);

        if (s.href_) href (*(s.href_));
        else href_.reset (nullptr);
      }

      return *this;
    }


    // ImplementationArtifactDescription
    bool ImplementationArtifactDescription::
    label_p () const
    {
      return !!label_;
    }

    ::XMLSchema::string<char> const& ImplementationArtifactDescription::
    label () const
    {
      return *label_;
    }

    void ImplementationArtifactDescription::
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

    // ImplementationArtifactDescription
    bool ImplementationArtifactDescription::
    UUID_p () const
    {
      return !!UUID_;
    }

    ::XMLSchema::string<char> const& ImplementationArtifactDescription::
    UUID () const
    {
      return *UUID_;
    }

    void ImplementationArtifactDescription::
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

    // ImplementationArtifactDescription
    ImplementationArtifactDescription::location_const_iterator ImplementationArtifactDescription::
    begin_location () const
    {
      return location_.cbegin ();
    }

    ImplementationArtifactDescription::location_const_iterator ImplementationArtifactDescription::
    end_location () const
    {
      return location_.cend ();
    }

    size_t ImplementationArtifactDescription::
    count_location(void) const
    {
      return location_.size ();
    }

    // ImplementationArtifactDescription
    ImplementationArtifactDescription::dependsOn_const_iterator ImplementationArtifactDescription::
    begin_dependsOn () const
    {
      return dependsOn_.cbegin ();
    }

    ImplementationArtifactDescription::dependsOn_const_iterator ImplementationArtifactDescription::
    end_dependsOn () const
    {
      return dependsOn_.cend ();
    }

    size_t ImplementationArtifactDescription::
    count_dependsOn(void) const
    {
      return dependsOn_.size ();
    }

    // ImplementationArtifactDescription
    ImplementationArtifactDescription::execParameter_const_iterator ImplementationArtifactDescription::
    begin_execParameter () const
    {
      return execParameter_.cbegin ();
    }

    ImplementationArtifactDescription::execParameter_const_iterator ImplementationArtifactDescription::
    end_execParameter () const
    {
      return execParameter_.cend ();
    }

    size_t ImplementationArtifactDescription::
    count_execParameter(void) const
    {
      return execParameter_.size ();
    }

    // ImplementationArtifactDescription
    ImplementationArtifactDescription::infoProperty_const_iterator ImplementationArtifactDescription::
    begin_infoProperty () const
    {
      return infoProperty_.cbegin ();
    }

    ImplementationArtifactDescription::infoProperty_const_iterator ImplementationArtifactDescription::
    end_infoProperty () const
    {
      return infoProperty_.cend ();
    }

    size_t ImplementationArtifactDescription::
    count_infoProperty(void) const
    {
      return infoProperty_.size ();
    }

    // ImplementationArtifactDescription
    ImplementationArtifactDescription::deployRequirement_const_iterator ImplementationArtifactDescription::
    begin_deployRequirement () const
    {
      return deployRequirement_.cbegin ();
    }

    ImplementationArtifactDescription::deployRequirement_const_iterator ImplementationArtifactDescription::
    end_deployRequirement () const
    {
      return deployRequirement_.cend ();
    }

    size_t ImplementationArtifactDescription::
    count_deployRequirement(void) const
    {
      return deployRequirement_.size ();
    }

    // ImplementationArtifactDescription
    bool ImplementationArtifactDescription::
    contentLocation_p () const
    {
      return !!contentLocation_;
    }

    ::XMLSchema::string<char> const& ImplementationArtifactDescription::
    contentLocation () const
    {
      return *contentLocation_;
    }

    void ImplementationArtifactDescription::
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

    // ImplementationArtifactDescription
    bool ImplementationArtifactDescription::
    href_p () const
    {
      return !!href_;
    }

    ::XMLSchema::string<char> const& ImplementationArtifactDescription::
    href () const
    {
      return *href_;
    }

    ::XMLSchema::string<char>& ImplementationArtifactDescription::
    href ()
    {
      return *href_;
    }

    void ImplementationArtifactDescription::
    href (::XMLSchema::string<char> const& e)
    {
      if (href_)
      {
        *href_ = e;
      }

      else
      {
        href_ = std::make_unique< ::XMLSchema::string<char>> (e);
      }
    }
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
    // NamedImplementationArtifact

    NamedImplementationArtifact::
    NamedImplementationArtifact (::XSCRT::XML::Element<char> const& e)
    :Base (e)
    {

      ::XSCRT::Parser<char> p (e);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "name")
        {
          name_ = std::make_unique< ::XMLSchema::string<char>> (e);
        }

        else if (n == "referencedArtifact")
        {
          referencedArtifact_ = std::make_unique< ::DAnCE::Config_Handlers::ImplementationArtifactDescription> (e);
        }

        else 
        {
        }
      }
    }

    // ImplementationArtifactDescription

    ImplementationArtifactDescription::
    ImplementationArtifactDescription (::XSCRT::XML::Element<char> const& e)
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

        else if (n == "location")
        {
          ::XMLSchema::string<char> t (e);
          location_.push_back (t);
        }

        else if (n == "dependsOn")
        {
          ::DAnCE::Config_Handlers::NamedImplementationArtifact t (e);
          dependsOn_.push_back (t);
        }

        else if (n == "execParameter")
        {
          ::DAnCE::Config_Handlers::Property t (e);
          execParameter_.push_back (t);
        }

        else if (n == "infoProperty")
        {
          ::DAnCE::Config_Handlers::Property t (e);
          infoProperty_.push_back (t);
        }

        else if (n == "deployRequirement")
        {
          ::DAnCE::Config_Handlers::Requirement t (e);
          deployRequirement_.push_back (t);
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

      while (p.more_attributes ())
      {
        ::XSCRT::XML::Attribute<char> a (p.next_attribute ());
        std::string n (::XSCRT::XML::uq_name (a.name ()));
        if (n ==  ("href"))
        {
          ::XMLSchema::string<char> t (a);
          href (t);
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

