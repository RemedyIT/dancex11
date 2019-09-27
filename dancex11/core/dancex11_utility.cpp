/**
 * @file    dancex11_utility.cpp
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11/core/dancex11_utility.h"

#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/deployment/deployment_starterrorC.h"
#include "dancex11/deployment/deployment_stoperrorC.h"
#include "dancex11/deployment/deployment_invalidconnectionC.h"
#include "dancex11/deployment/deployment_invalidnodeexecparameterC.h"
#include "dancex11/deployment/deployment_invalidpropertyC.h"

#include <fstream>
#include <sstream>

namespace DAnCEX11
{
  namespace Utility
  {
    void
    build_property_map (PROPERTY_MAP &map,
      const Deployment::Properties &prop,
      bool overwrite)
    {
      DANCEX11_LOG_TRACE("DAnCEX11::Utility::build_property_map");

      for (const Deployment::Property &property : prop)
      {
        if (overwrite || map.find (property.name ()) == map.end ())
        {
          map[property.name ()] = property.value ();

          DANCEX11_LOG_INFO ("build_property_map: " <<
                             "Bound value for property <" << property.name () << ">");
        }
      }
    }

    void
    build_property_sequence (::Deployment::Properties &prop,
        const PROPERTY_MAP &pmap)
    {
      for (PROPERTY_MAP::const_iterator i = pmap.begin ();
           i != pmap.end ();
           ++i)
        {
          prop.push_back (::Deployment::Property (i->first, i->second));

          DANCEX11_LOG_INFO ("build_property_sequence: " <<
                             "Added value for property <" << i->first << ">");
        }
    }

    bool
    write_IOR (const std::string &pathname, const std::string& ior)
    {
      // Output the IOR to the <ior_output_file>
      std::ofstream fos (pathname);
      if (!fos)
        {
          DANCEX11_LOG_ERROR ("DAnCEX11::Utility::write_IOR - " <<
                              "Failed create <" << pathname << ">");
          return false;
        }
      fos << ior;
      fos.close ();
      return true;
    }

    void
    append_properties (Deployment::Properties &dest,
      const Deployment::Properties &src)
    {
      for (const Deployment::Property &prop : src)
        {
          dest.push_back (prop);
        }
    }

    bool
    get_resource_value (const std::string &type,
      const Deployment::Resources &resources,
      Deployment::Resource &val)
    {
      DANCEX11_LOG_TRACE ("DAnCEX11::Utility::get_resource_value<const char *>");

      DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_resource_value - " <<
                         "Finding resource for type '" << type << "'");

      for (Deployment::Resource resource : resources)
        {
          /// search for the resource with resourceType
          for (const std::string &resource_type : resource.resourceType ())
            {
              if (type == resource_type)
                {
                  DANCEX11_LOG_DEBUG ("DAnCEX11::Utility::get_resource_value - " <<
                                      "Found resource for type '" << type << "'");
                  val = resource;
                  return true;
                }
            }
        }

      DANCEX11_LOG_ERROR ("DAnCEX11::Utility::get_resource_value - " <<
                          "Failed to extract resource for type '" << type << "'");
      return false;
    }

    std::string
    get_instance_type (const Deployment::Properties &prop)
    {
      DANCEX11_LOG_TRACE ("DAnCEX11::Utility::get_instance_type");

      for (const Deployment::Property &property : prop)
        {
          DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_instance_type - " <<
                             "Checking property <" << property.name () << ">");

          if (property.name () == DAnCEX11::HOME_FACTORY)
            {
              DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_instance_type - " <<
                                 "Found Home type");
              return DAnCEX11::CCM_HOME;
            }
          if (property.name () == DAnCEX11::COMPONENT_FACTORY)
            {
              DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_instance_type - " <<
                                 "Found unmanaged component type");
              return DAnCEX11::CCM_COMPONENT;
            }
          if (property.name () == DAnCEX11::EXPLICIT_HOME)
            {
              DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_instance_type - " <<
                                 "Found explicit home component type");
              return DAnCEX11::CCM_HOMEDCOMPONENT;
            }
          if (property.name () == DAnCEX11::IMPL_TYPE)
            {
              std::string val;
              if (get_property_value (DAnCEX11::IMPL_TYPE, prop, val))
                {
                  DANCEX11_LOG_INFO ("DAnCEX11::Utility::get_instance_type - " <<
                                     "Found instance type <" << val << ">");
                  return val;
                }
            }
        }

      DANCEX11_LOG_ERROR ("DAnCEX11::Utility::get_instance_type - " <<
                          "Unable to determine instance type, instance will be ignored");
      return "";
    }

    bool
    throw_exception_from_any (const CORBA::Any &excep)
    {
      IDL::traits<CORBA::TypeCode>::ref_type tc = excep.type ();
      const std::string &ex_id = tc->id ();
      DANCEX11_LOG_DEBUG ("DAnCEX11::Utility::throw_exception_from_any - " <<
                          "Found typecode " << ex_id);;
      if (ex_id == Deployment::_tc_PlanError->id ())
        {
          extract_and_throw_exception<Deployment::PlanError> (excep);
        }
      else if (ex_id == Deployment::_tc_StartError->id ())
        {
          extract_and_throw_exception<Deployment::StartError> (excep);
        }
      else if (ex_id == Deployment::_tc_StopError->id ())
        {
          extract_and_throw_exception<Deployment::StopError> (excep);
        }
      else if (ex_id == Deployment::_tc_InvalidConnection->id ())
        {
          extract_and_throw_exception<Deployment::InvalidConnection> (excep);
        }
      else if (ex_id == Deployment::_tc_InvalidNodeExecParameter->id ())
        {
          extract_and_throw_exception<Deployment::InvalidNodeExecParameter> (excep);
        }
      else if (ex_id == Deployment::_tc_InvalidProperty->id ())
        {
          extract_and_throw_exception<Deployment::InvalidProperty> (excep);
        }
      else if (ex_id.find ("IDL:omg.org/CORBA/") == 0)
        {
           std::unique_ptr<CORBA::SystemException> sysex =
               extract_system_exception_from_any (excep);
           if (sysex)
           {
             sysex->_raise ();
           }
        }
      DANCEX11_LOG_ERROR ("DAnCEX11::Utility::throw_exception_from_any - " <<
                          "Exception with typecode " << ex_id << " unknown");
      throw CORBA::UNKNOWN ();
    }

    CORBA::Any
    create_any_from_user_exception (const CORBA::UserException& ex)
    {
      CORBA::Any ex_any;
      ex_any <<= ex;
      return ex_any;
    }

    std::string
    stringify_exception_from_any (const CORBA::Any &excep)
    {
      std::string const ex_id = excep.type ()->id ();

      DANCEX11_LOG_DEBUG ("DAnCEX11::Utility::throw_exception_from_any - " <<
                          "Found typecode " << ex_id);
      bool flag (false);
      std::string result;

      if (ex_id == Deployment::_tc_PlanError->id ())
        {
          result += "PlanError exception -";
          flag = stringify_exception<Deployment::PlanError> (excep,
                                                             result);
        }
      else if (ex_id == Deployment::_tc_StartError->id ())
        {
          result += "StartError exception -";
          flag = stringify_exception<Deployment::StartError> (excep,
                                                              result);
        }
      else if (ex_id == Deployment::_tc_StopError->id ())
        {
          result += "StopError exception -";
          flag = stringify_exception<Deployment::StopError> (excep,
                                                             result);
        }
      else if (ex_id == Deployment::_tc_InvalidConnection->id ())
        {
          result += "InvalidConnection exception -";
          flag = stringify_exception<Deployment::InvalidConnection> (excep,
                                                                     result);
        }
      else if (ex_id == Deployment::_tc_InvalidNodeExecParameter->id ())
        {
          result += "InvalidNodeExecParameter exception -";
          flag = stringify_exception<Deployment::InvalidNodeExecParameter> (excep,
                                                                            result);
        }
      else if (ex_id == Deployment::_tc_InvalidProperty->id ())
        {
          result += "InvalidProperty exception -";
          flag = stringify_exception<Deployment::InvalidProperty> (excep,
                                                                   result);
        }
      else if (ex_id.find ("IDL:omg.org/CORBA/") == 0)
        {
           std::unique_ptr<CORBA::SystemException> sysex =
               extract_system_exception_from_any (excep);
           if (sysex)
             {
               std::ostringstream os;
               os << "CORBA System Exception: " << *sysex;
               result = os.str ();
               flag = true;
             }
        }

      if (!flag)
        {
          result += "Unable to decode exception meta-data for ID ";
          result += ex_id;
        }

      return result;
    }

    std::vector< std::string >
    tokenize (const char *str, char c)
    {
      std::vector< std::string > result;

      do
        {
          const char *begin = str;
          while(*str != c && *str)
            {
              str++;
            }
          result.push_back (std::string (begin, str));
        }
      while (0 != *str++);

      return result;
    }
  } /* namespace Utility */
} /* namespace DAnCEX11 */
