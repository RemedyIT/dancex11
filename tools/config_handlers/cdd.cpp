/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 */
#include "cdd.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    // Domain

    Domain::Domain (node_container_type const& node__)
    : ::XSCRT::Type ()
    , node_ (node__)
    {
    }

    Domain::Domain (Domain const& s) :
    ::XSCRT::Type (s)
    , UUID_ (s.UUID_ ? std::make_unique<::XMLSchema::string<char>> (*s.UUID_) : nullptr)
    , label_ (s.label_ ? std::make_unique<::XMLSchema::string<char>> (*s.label_) : nullptr)
    , node_ (s.node_)
    , interconnect_ (s.interconnect_)
    , bridge_ (s.bridge_)
    , sharedResource_ (s.sharedResource_)
    , infoProperty_ (s.infoProperty_)
    {
    }

    Domain&
    Domain::operator= (Domain const& s)
    {
      if (std::addressof(s) != this)
      {
        if (s.UUID_)
          UUID (*(s.UUID_));
        else
          UUID_.release ();

        if (s.label_)
          label (*(s.label_));
        else
          label_.release ();

        node_ = s.node_;

        interconnect_ = s.interconnect_;

        bridge_ = s.bridge_;

        sharedResource_ = s.sharedResource_;

        infoProperty_ = s.infoProperty_;
      }

      return *this;
    }


    // Domain
    bool Domain::
    UUID_p () const
    {
      return !!UUID_;
    }

    ::XMLSchema::string<char> const& Domain::
    UUID () const
    {
      return *UUID_;
    }

    void Domain::
    UUID (::XMLSchema::string<char> const& e)
    {
      if (UUID_)
      {
        *UUID_ = e;
      }

      else
      {
        UUID_ = std::make_unique<::XMLSchema::string<char>> (e);
      }
    }

    // Domain
    bool Domain::
    label_p () const
    {
      return !!label_;
    }

    ::XMLSchema::string<char> const& Domain::
    label () const
    {
      return *label_;
    }

    void Domain::
    label (::XMLSchema::string<char> const& e)
    {
      if (label_)
      {
        *label_ = e;
      }

      else
      {
        label_ = std::make_unique<::XMLSchema::string<char>> (e);
      }
    }

    // Domain
    Domain::node_const_iterator Domain::
    begin_node () const
    {
      return node_.cbegin ();
    }

    Domain::node_const_iterator Domain::
    end_node () const
    {
      return node_.cend ();
    }

    size_t Domain::
    count_node() const
    {
      return node_.size ();
    }

    // Domain
    Domain::interconnect_const_iterator Domain::
    begin_interconnect () const
    {
      return interconnect_.cbegin ();
    }

    Domain::interconnect_const_iterator Domain::
    end_interconnect () const
    {
      return interconnect_.cend ();
    }

    size_t Domain::
    count_interconnect() const
    {
      return interconnect_.size ();
    }

    // Domain
    Domain::bridge_const_iterator Domain::
    begin_bridge () const
    {
      return bridge_.cbegin ();
    }

    Domain::bridge_const_iterator Domain::
    end_bridge () const
    {
      return bridge_.cend ();
    }

    size_t Domain::
    count_bridge() const
    {
      return bridge_.size ();
    }

    // Domain
    Domain::sharedResource_const_iterator Domain::
    begin_sharedResource () const
    {
      return sharedResource_.cbegin ();
    }

    Domain::sharedResource_const_iterator Domain::
    end_sharedResource () const
    {
      return sharedResource_.cend ();
    }

    size_t Domain::
    count_sharedResource() const
    {
      return sharedResource_.size ();
    }

    // Domain
    Domain::infoProperty_const_iterator Domain::
    begin_infoProperty () const
    {
      return infoProperty_.cbegin ();
    }

    Domain::infoProperty_const_iterator Domain::
    end_infoProperty () const
    {
      return infoProperty_.cend ();
    }

    size_t Domain::
    count_infoProperty() const
    {
      return infoProperty_.size ();
    }


    // Bridge

    Bridge::Bridge (::XMLSchema::string<char> const& name__,
                    connect_container_type const& connect__)
    : ::XSCRT::Type ()
    , name_ (std::make_unique<::XMLSchema::string<char>> (name__))
    , connect_ (connect__)
    {
    }

    Bridge::Bridge (Bridge const& s) :
    ::XSCRT::Type (s)
    , name_ (std::make_unique<::XMLSchema::string<char>> (*s.name_))
    , label_ (s.label_ ? std::make_unique<::XMLSchema::string<char>> (*s.label_) : nullptr)
    , connect_ (s.connect_)
    , resource_ (s.resource_)
    {
    }

    Bridge&
    Bridge::operator= (Bridge const& s)
    {
      if (std::addressof(s) != this)
      {
        name (*s.name_);

        if (s.label_)
          label (*(s.label_));
        else
          label_.release ();

        connect_ = s.connect_;

        resource_ = s.resource_;
      }

      return *this;
    }


    // Bridge
    ::XMLSchema::string<char> const& Bridge::
    name () const
    {
      return *name_;
    }

    void Bridge::
    name (::XMLSchema::string<char> const& e)
    {
      *name_ = e;
    }

    // Bridge
    bool Bridge::
    label_p () const
    {
      return !!label_;
    }

    ::XMLSchema::string<char> const& Bridge::
    label () const
    {
      return *label_;
    }

    void Bridge::
    label (::XMLSchema::string<char> const& e)
    {
      if (label_)
      {
        *label_ = e;
      }

      else
      {
        label_ = std::make_unique<::XMLSchema::string<char>> (e);
      }
    }

    // Bridge
    Bridge::connect_const_iterator Bridge::
    begin_connect () const
    {
      return connect_.cbegin ();
    }

    Bridge::connect_const_iterator Bridge::
    end_connect () const
    {
      return connect_.cend ();
    }

    size_t Bridge::
    count_connect() const
    {
      return connect_.size ();
    }

    // Bridge
    Bridge::resource_const_iterator Bridge::
    begin_resource () const
    {
      return resource_.cbegin ();
    }

    Bridge::resource_const_iterator Bridge::
    end_resource () const
    {
      return resource_.cend ();
    }

    size_t Bridge::
    count_resource() const
    {
      return resource_.size ();
    }


    // Interconnect

    Interconnect::Interconnect (::XMLSchema::string<char> const& name__,
                                connect_container_type const& connect__)
    : ::XSCRT::Type ()
    , name_ (std::make_unique<::XMLSchema::string<char>> (name__))
    , connect_ (connect__)
    {
    }

    Interconnect::Interconnect (Interconnect const& s) :
    ::XSCRT::Type (s)
    , name_ (std::make_unique<::XMLSchema::string<char>> (*s.name_))
    , label_ (s.label_ ? std::make_unique<::XMLSchema::string<char>> (*s.label_) : nullptr)
    , connection_ (s.connection_)
    , connect_ (s.connect_)
    , resource_ (s.resource_)
    {
    }

    Interconnect&
    Interconnect::operator= (Interconnect const& s)
    {
      if (std::addressof(s) != this)
      {
        name (*s.name_);

        if (s.label_)
          label (*(s.label_));
        else
          label_.release ();

        connection_ = s.connection_;

        connect_ = s.connect_;

        resource_ = s.resource_;
      }

      return *this;
    }


    // Interconnect
    ::XMLSchema::string<char> const& Interconnect::
    name () const
    {
      return *name_;
    }

    void Interconnect::
    name (::XMLSchema::string<char> const& e)
    {
      *name_ = e;
    }

    // Interconnect
    bool Interconnect::
    label_p () const
    {
      return !!label_;
    }

    ::XMLSchema::string<char> const& Interconnect::
    label () const
    {
      return *label_;
    }

    void Interconnect::
    label (::XMLSchema::string<char> const& e)
    {
      if (label_)
      {
        *label_ = e;
      }

      else
      {
        label_ = std::make_unique<::XMLSchema::string<char>> (e);
      }
    }

    // Interconnect
    Interconnect::connection_const_iterator Interconnect::
    begin_connection () const
    {
      return connection_.cbegin ();
    }

    Interconnect::connection_const_iterator Interconnect::
    end_connection () const
    {
      return connection_.cend ();
    }

    size_t Interconnect::
    count_connection() const
    {
      return connection_.size ();
    }

    // Interconnect
    Interconnect::connect_const_iterator Interconnect::
    begin_connect () const
    {
      return connect_.cbegin ();
    }

    Interconnect::connect_const_iterator Interconnect::
    end_connect () const
    {
      return connect_.cend ();
    }

    size_t Interconnect::
    count_connect() const
    {
      return connect_.size ();
    }

    // Interconnect
    Interconnect::resource_const_iterator Interconnect::
    begin_resource () const
    {
      return resource_.cbegin ();
    }

    Interconnect::resource_const_iterator Interconnect::
    end_resource () const
    {
      return resource_.cend ();
    }

    size_t Interconnect::
    count_resource() const
    {
      return resource_.size ();
    }


    // Node

    Node::Node (::XMLSchema::string<char> const& name__)
    : ::XSCRT::Type ()
    , name_ (std::make_unique<::XMLSchema::string<char>> (name__))
    {
    }

    Node::Node (Node const& s) :
    ::XSCRT::Type (s)
    , name_ (std::make_unique<::XMLSchema::string<char>> (*s.name_))
    , label_ (s.label_ ? std::make_unique<::XMLSchema::string<char>> (*s.label_) : nullptr)
    , connection_ (s.connection_)
    , sharedResource_ (s.sharedResource_)
    , resource_ (s.resource_)
    {
    }

    Node&
    Node::operator= (Node const& s)
    {
      if (std::addressof(s) != this)
      {
        name (*s.name_);

        if (s.label_)
          label (*(s.label_));
        else
          label_.release ();

        connection_ = s.connection_;

        sharedResource_ = s.sharedResource_;

        resource_ = s.resource_;
      }

      return *this;
    }


    // Node
    ::XMLSchema::string<char> const& Node::
    name () const
    {
      return *name_;
    }

    void Node::
    name (::XMLSchema::string<char> const& e)
    {
      *name_ = e;
    }

    // Node
    bool Node::
    label_p () const
    {
      return !!label_;
    }

    ::XMLSchema::string<char> const& Node::
    label () const
    {
      return *label_;
    }

    void Node::
    label (::XMLSchema::string<char> const& e)
    {
      if (label_)
      {
        *label_ = e;
      }

      else
      {
        label_ = std::make_unique<::XMLSchema::string<char>> (e);
      }
    }

    // Node
    Node::connection_const_iterator Node::
    begin_connection () const
    {
      return connection_.cbegin ();
    }

    Node::connection_const_iterator Node::
    end_connection () const
    {
      return connection_.cend ();
    }

    size_t Node::
    count_connection() const
    {
      return connection_.size ();
    }

    // Node
    Node::sharedResource_const_iterator Node::
    begin_sharedResource () const
    {
      return sharedResource_.cbegin ();
    }

    Node::sharedResource_const_iterator Node::
    end_sharedResource () const
    {
      return sharedResource_.cend ();
    }

    size_t Node::
    count_sharedResource() const
    {
      return sharedResource_.size ();
    }

    // Node
    Node::resource_const_iterator Node::
    begin_resource () const
    {
      return resource_.cbegin ();
    }

    Node::resource_const_iterator Node::
    end_resource () const
    {
      return resource_.cend ();
    }

    size_t Node::
    count_resource() const
    {
      return resource_.size ();
    }


    // SharedResource

    SharedResource::SharedResource (::XMLSchema::string<char> const& name__,
                                    ::XMLSchema::string<char> const& resourceType__,
                                    ::DAnCE::Config_Handlers::Node const& node__,
                                    ::DAnCE::Config_Handlers::SatisfierProperty const& property__)
    : ::XSCRT::Type ()
    , name_ (std::make_unique<::XMLSchema::string<char>> (name__))
    , resourceType_ (std::make_unique<::XMLSchema::string<char>> (resourceType__))
    , node_ (std::make_unique<::DAnCE::Config_Handlers::Node> (node__))
    , property_ (std::make_unique<::DAnCE::Config_Handlers::SatisfierProperty> (property__))
    {
    }

    SharedResource::SharedResource (SharedResource const& s) :
    ::XSCRT::Type (s)
    , name_ (std::make_unique<::XMLSchema::string<char>> (*s.name_))
    , resourceType_ (std::make_unique<::XMLSchema::string<char>> (*s.resourceType_))
    , node_ (std::make_unique<::DAnCE::Config_Handlers::Node> (*s.node_))
    , property_ (std::make_unique<::DAnCE::Config_Handlers::SatisfierProperty> (*s.property_))
    {
    }

    SharedResource&
    SharedResource::operator= (SharedResource const& s)
    {
      if (std::addressof(s) != this)
      {
        name (*s.name_);

        resourceType (*s.resourceType_);

        node (*s.node_);

        property (*s.property_);
      }

      return *this;
    }


    // SharedResource
    ::XMLSchema::string<char> const& SharedResource::
    name () const
    {
      return *name_;
    }

    void SharedResource::
    name (::XMLSchema::string<char> const& e)
    {
      *name_ = e;
    }

    // SharedResource
    ::XMLSchema::string<char> const& SharedResource::
    resourceType () const
    {
      return *resourceType_;
    }

    void SharedResource::
    resourceType (::XMLSchema::string<char> const& e)
    {
      *resourceType_ = e;
    }

    // SharedResource
    ::DAnCE::Config_Handlers::Node const& SharedResource::
    node () const
    {
      return *node_;
    }

    void SharedResource::
    node (::DAnCE::Config_Handlers::Node const& e)
    {
      *node_ = e;
    }

    // SharedResource
    ::DAnCE::Config_Handlers::SatisfierProperty const& SharedResource::
    property () const
    {
      return *property_;
    }

    void SharedResource::
    property (::DAnCE::Config_Handlers::SatisfierProperty const& e)
    {
      *property_ = e;
    }
  }
}

namespace DAnCE
{
  namespace Config_Handlers
  {
    // Domain

    Domain::
    Domain (::XSCRT::XML::Element<char> const& element)
    :Base (element)
    {

      ::XSCRT::Parser<char> p (element);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "UUID")
        {
          ::XMLSchema::string<char> t (e);
          UUID (t);
        }

        else if (n == "label")
        {
          ::XMLSchema::string<char> t (e);
          label (t);
        }

        else if (n == "node")
        {
          ::DAnCE::Config_Handlers::Node t (e);
          node_.push_back (t);
        }

        else if (n == "interconnect")
        {
          ::DAnCE::Config_Handlers::Interconnect t (e);
          interconnect_.push_back (t);
        }

        else if (n == "bridge")
        {
          ::DAnCE::Config_Handlers::Bridge t (e);
          bridge_.push_back (t);
        }

        else if (n == "sharedResource")
        {
          ::DAnCE::Config_Handlers::SharedResource t (e);
          sharedResource_.push_back (t);
        }

        else if (n == "infoProperty")
        {
          ::DAnCE::Config_Handlers::Property t (e);
          infoProperty_.push_back (t);
        }

        else
        {
        }
      }
    }

    // Bridge

    Bridge::
    Bridge (::XSCRT::XML::Element<char> const& element)
    :Base (element)
    {

      ::XSCRT::Parser<char> p (element);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "name")
        {
          name_ = std::make_unique<::XMLSchema::string<char>> (e);
        }

        else if (n == "label")
        {
          ::XMLSchema::string<char> t (e);
          label (t);
        }

        else if (n == "connect")
        {
          ::DAnCE::Config_Handlers::Interconnect t (e);
          connect_.push_back (t);
        }

        else if (n == "resource")
        {
          ::DAnCE::Config_Handlers::Resource t (e);
          resource_.push_back (t);
        }

        else
        {
        }
      }
    }

    // Interconnect

    Interconnect::
    Interconnect (::XSCRT::XML::Element<char> const& element)
    :Base (element)
    {

      ::XSCRT::Parser<char> p (element);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "name")
        {
          name_ = std::make_unique<::XMLSchema::string<char>> (e);
        }

        else if (n == "label")
        {
          ::XMLSchema::string<char> t (e);
          label (t);
        }

        else if (n == "connection")
        {
          ::DAnCE::Config_Handlers::Bridge t (e);
          connection_.push_back (t);
        }

        else if (n == "connect")
        {
          ::DAnCE::Config_Handlers::Node t (e);
          connect_.push_back (t);
        }

        else if (n == "resource")
        {
          ::DAnCE::Config_Handlers::Resource t (e);
          resource_.push_back (t);
        }

        else
        {
        }
      }
    }

    // Node

    Node::
    Node (::XSCRT::XML::Element<char> const& element)
    :Base (element)
    {

      ::XSCRT::Parser<char> p (element);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "name")
        {
          name_ = std::make_unique<::XMLSchema::string<char>> (e);
        }

        else if (n == "label")
        {
          ::XMLSchema::string<char> t (e);
          label (t);
        }

        else if (n == "connection")
        {
          ::DAnCE::Config_Handlers::Interconnect t (e);
          connection_.push_back (t);
        }

        else if (n == "sharedResource")
        {
          ::DAnCE::Config_Handlers::SharedResource t (e);
          sharedResource_.push_back (t);
        }

        else if (n == "resource")
        {
          ::DAnCE::Config_Handlers::Resource t (e);
          resource_.push_back (t);
        }

        else
        {
        }
      }
    }

    // SharedResource

    SharedResource::
    SharedResource (::XSCRT::XML::Element<char> const& element)
    :Base (element)
    {

      ::XSCRT::Parser<char> p (element);

      while (p.more_elements ())
      {
        ::XSCRT::XML::Element<char> e (p.next_element ());
        std::string n (::XSCRT::XML::uq_name (e.name ()));

        if (n == "name")
        {
          name_ = std::make_unique<::XMLSchema::string<char>> (e);
        }

        else if (n == "resourceType")
        {
          resourceType_ = std::make_unique<::XMLSchema::string<char>> (e);
        }

        else if (n == "node")
        {
          node_ = std::make_unique<::DAnCE::Config_Handlers::Node> (e);
        }

        else if (n == "property")
        {
          property_ = std::make_unique<::DAnCE::Config_Handlers::SatisfierProperty> (e);
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

