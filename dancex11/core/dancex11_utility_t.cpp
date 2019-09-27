/**
 * @file    dancex11_utility_t.cpp
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_UTILITY_T_CPP
#define DANCEX11_UTILITY_T_CPP

#include "dancex11/core/dancex11_utility.h"
#include "dancex11/logger/log.h"

#include <sstream>

namespace DAnCEX11
{
  namespace Utility
  {
    template<class T>
    bool
    get_property_value (const std::string &name, const PROPERTY_MAP &properties, T &val)
    {
      DANCEX11_LOG_TRACE ("DAnCEX11::Utility::get_property_value<T>");

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_property_value<T> - " <<
                         "Finding property value for name '" << name << "'");

      PROPERTY_MAP::const_iterator i = properties.find (name);
      if (i != properties.end ())
        {
          if (i->second >>= val)
            {
              return true;
            }
          else
            {
              DANCEX11_LOG_ERROR ("DAnCEX11::Utility::get_property_value<T> - " <<
                                  "Failed to extract property value for " <<  name);
              return false;
            }
        }

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_property_value<T> - " <<
                         "Property value for name '" << name << "' has no value");

      return false;
    }

    template<class T>
    bool
    get_property_value (const std::string &name, const Deployment::Properties &properties, T &val)
    {
      DANCEX11_LOG_TRACE ("DAnCEX11::Utility::get_property_value<T>");

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_property_value<T> - " <<
                         "Finding property value for name '" << name << "'");

      for (Deployment::Property property : properties)
        {
          if (property.name () == name)
            {
              DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_property_value<T> - " <<
                                 "Found property '" << name << "'");
              if (property.value () >>= val)
                {
                  return true;
                }
              else
                {
                  DANCEX11_LOG_ERROR ("DAnCEX11::Utility::get_property_value<T> - " <<
                                      "Failed to extract property value for '" << name << "'");
                  return false;
                }
            }
        }

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_property_value<T> - " <<
                         "Property value for name '" << name << "' has no value");

      return false;
    }

    template<class T>
    bool
    get_satisfier_property_value (const std::string &name, const Deployment::SatisfierProperties &properties, T &val)
    {
      DANCEX11_LOG_TRACE ("DAnCEX11::Utility::get_satisfier_property_value<T>");

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_satisfier_property_value<T> - " <<
                         "Finding property value for name '" << name << "'");

      for (Deployment::SatisfierProperty property : properties)
        {
          if (property.name () == name)
            {
              DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_satisfier_property_value<T> - " <<
                                 "Found property '" << name << "'");
              if (property.value () >>= val)
                {
                  return true;
                }
              else
                {
                  DANCEX11_LOG_ERROR ("DAnCEX11::Utility::get_satisfier_property_value<T> - " <<
                                      "Failed to extract property value for '" << name << "'");
                  return false;
                }
            }
        }

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_satisfier_property_value<T> - " <<
                         "Property value for name '" << name << "' has no value");

      return false;
    }

    template<class T>
    void
    update_property_value (const std::string &name, Deployment::Properties &properties, const T &val)
    {
      DANCEX11_LOG_TRACE ("DAnCEX11::Utility::update_property_value<T>");

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::update_property_value<T> - " <<
                         "Finding property value for name '" << name << "'");

      for (Deployment::Property property : properties)
        {
          if (property.name () == name)
            {
              DANCEX11_LOG_INFO ("DAnCEX11::Utility::update_property_value<T> - " <<
                                 "Found property '" << name << "'");
              property.value () <<= val;
              return;
            }
        }

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::update_property_value<T> - " <<
                         "Property for name '" << name << "' not found; adding property");

      properties.push_back (Deployment::Property ());
      properties.back ().name (name);
      properties.back ().value () <<= val;
    }

    /// Tests flag, if false, sets it to true and replaces the name and
    /// reason flags of the exception.
    template <typename EXCEPTION>
    void
    test_and_set_exception (bool &flag, EXCEPTION &exception,
      const std::string &name, const std::string &reason)
    {
      if (!flag)
        {
          flag = true;
          exception.name (name);
          exception.reason (reason);
        }
    }

    /// Tests flag, if false, sets it to true and replaces the name and
    /// reason flags of the exception.
    template <typename EXCEPTION>
    void
    test_and_set_exception (bool &flag, EXCEPTION &exception,
      const std::string &name, const CORBA::Exception &reason)
    {
      if (!flag)
        {
          flag = true;
          exception.name (name);
          std::ostringstream os;
          IDL::traits<CORBA::Exception>::write_on (os, reason);
          exception.reason (os.str ());
        }
    }

    template <typename EXCEPTION>
    bool
    extract_and_throw_exception (const CORBA::Any &excep)
    {
      EXCEPTION ex_holder;

      if (excep >>= ex_holder)
        ex_holder._raise ();

      return false;
    }

    template <typename EXCEPTION>
    bool
    stringify_exception (const CORBA::Any &excep,
      std::string &result)
    {
      EXCEPTION ex_holder;

       if (excep >>= ex_holder)
         {
           std::ostringstream os;
           os << " Name: " << ex_holder.name () << " Reason: " << ex_holder.reason ();
           result += os.str ();
           return true;
         }

      return false;
    }

    template <typename EXCEPTION>
    CORBA::Any
    create_any_from_exception (const EXCEPTION &ex)
    {
      DANCEX11_LOG_ERROR ("create_any_from_exception - " <<
                          "Handling exception <" << ex << ">");
      CORBA::Any tmp;
      tmp <<= ex;
      return tmp;
    }
  }
}

#endif /* DANCEX11_UTILITY_T_CPP */
