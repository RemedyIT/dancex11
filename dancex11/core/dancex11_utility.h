/**
 * @file    dancex11_utility.h
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_Utility_H
#define DAnCEX11_Utility_H

#include "dancex11_stub_export.h"

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_targetdataC.h"

#include <map>
#include <string>

namespace DAnCEX11
{
  namespace Utility
  {
    typedef std::map< std::string, CORBA::Any > PROPERTY_MAP;

    DANCEX11_STUB_Export void
    build_property_map (PROPERTY_MAP &map,
      const Deployment::Properties &prop,
      bool overwrite = true);

    DANCEX11_STUB_Export void
    build_property_sequence (Deployment::Properties &prop,
      const PROPERTY_MAP &map);

    template<class T>
    bool
    get_property_value (const std::string &name,
      const PROPERTY_MAP &properties,
      T &val);

    template<class T>
    bool
    get_property_value (const std::string &name,
      const Deployment::Properties &properties,
      T &val);

    template<class T>
    bool
    get_satisfierproperty_value (const std::string &name,
      const Deployment::SatisfierProperties &properties,
      T &val);

    DANCEX11_STUB_Export bool
    get_resource_value (const std::string &type,
      const Deployment::Resources &resources,
      Deployment::Resource  &val);

    template<class T>
    void
    update_property_value (const std::string &name,
      Deployment::Properties &properties,
      const T &val);

    template <typename EXCEPTION>
    void
    test_and_set_exception (bool &flag, EXCEPTION &exception,
      const std::string &name, const std::string &reason);

    template <typename EXCEPTION>
    void
    test_and_set_exception (bool &flag, EXCEPTION &exception,
      const std::string &name, const CORBA::Exception &reason);

    DANCEX11_STUB_Export std::string
    get_instance_type (const Deployment::Properties &prop);

    DANCEX11_STUB_Export void
    append_properties (Deployment::Properties &dest,
      const Deployment::Properties &src);

    /// Write a string (usually a stringified IOR) to a file
    /// designated by the @a pathname.  The file named will always get
    /// overwritten.
    DANCEX11_STUB_Export bool
    write_IOR (const std::string &pathname,
      const std::string &IOR);

    /// Attempt to extract the any into EXCEPTION type and
    /// throw.  Returns 'false' if extraction fails.
    template <typename EXCEPTION>
    bool
    extract_and_throw_exception (const CORBA::Any &excep);

    DANCEX11_STUB_Export bool
    throw_exception_from_any (const CORBA::Any &excep);

    template <typename EXCEPTION>
    CORBA::Any
    create_any_from_exception (const EXCEPTION &ex);

    DANCEX11_STUB_Export CORBA::Any
    create_any_from_user_exception (const CORBA::UserException &ex);

    template <typename EXCEPTION>
    bool
    stringify_exception (const CORBA::Any &excep,
      std::string &result);

    DANCEX11_STUB_Export std::string
    stringify_exception_from_any (const CORBA::Any &excep);

    DANCEX11_STUB_Export std::vector< std::string >
    tokenize (const char *str, char c=' ');
  }
}

#include "dancex11/core/dancex11_utility_t.cpp"

#endif
