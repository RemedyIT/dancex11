// -*- C++ -*-
/**
 * @file    interceptor_handler_impl.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "interceptor_handler_impl.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "ace/DLL.h"

namespace DAnCEX11
{
  Interceptor_Handler_Impl::Interceptor_Handler_Impl (
      Plugin_Manager& plugins)
    : plugins_ (plugins)
  {
  }

  Interceptor_Handler_Impl::~Interceptor_Handler_Impl ()
  {
  }

  CORBA::StringSeq
  Interceptor_Handler_Impl::dependencies ()
  {
    CORBA::StringSeq retval;
    return retval;
  }

  void
  Interceptor_Handler_Impl::close ()
  {
  }

  std::string
  Interceptor_Handler_Impl::instance_type ()
  {
    DANCEX11_LOG_TRACE ("Interceptor_Handler_Impl::instance_type");
    return DAnCEX11::DANCE_DEPLOYMENTINTERCEPTOR;
  }

  void
  Interceptor_Handler_Impl::install_instance (const Deployment::DeploymentPlan & plan,
    uint32_t instanceRef,
    CORBA::Any&)
  {
    DANCEX11_LOG_TRACE ("Interceptor_Handler_Impl::install_instance - Called");
    if (plan.instance ().size () <= instanceRef)
    {
      DANCEX11_LOG_ERROR ("Interceptor_Handler_Impl::install_instance - " <<
                          "Invalid instance reference " << instanceRef << " provided " <<
                          "to install_instance");
      throw ::Deployment::PlanError (plan.UUID (),
        "Invalid instance reference");
    }

    Deployment::InstanceDeploymentDescription const &idd =
      plan.instance ()[instanceRef];

    if (plan.implementation ().size () <= idd.implementationRef ())
    {
      DANCEX11_LOG_ERROR ("Interceptor_Handler_Impl::install_instance - " <<
                          "Invalid implementation reference " << idd.implementationRef () << " provided " <<
                          "to install_instance");
      throw Deployment::PlanError (plan.UUID (),
        "Invalid Implementation reference");
    }

    Deployment::MonolithicDeploymentDescription const &mdd =
      plan.implementation ()[idd.implementationRef ()];

    DANCEX11_LOG_DEBUG ("Interceptor_Handler_Impl::install_instance - " <<
                        "Starting installation of instance <" << idd.name () << ">");

    std::string entrypt;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_FACTORY,
      mdd.execParameter (),
      entrypt))
    {
      DANCEX11_LOG_ERROR ("Interceptor_Handler_Impl::install_instance - " <<
                          "No entrypoint found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No entrypoint found for plug-in initialization\n");
    }

    std::string artifact;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_ARTIFACT,
      mdd.execParameter (),
      artifact))
    {
      DANCEX11_LOG_ERROR ("Interceptor_Handler_Impl::install_instance - " <<
                          "No artifact found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No artifact found for plug-in initialization\n");
    }

    uint32_t open_mode = ACE_DEFAULT_SHLIB_MODE;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_OPENMODE,
      mdd.execParameter (),
      open_mode))
    {
      DANCEX11_LOG_INFO ("Interceptor_Handler_Impl::install_instance - " <<
                          "No open mode found for plug-in initialization");
    }

    Utility::PROPERTY_MAP config;
    Utility::build_property_map (config, idd.configProperty ());
    Utility::build_property_map (config, mdd.execParameter ());

    this->plugins_.register_interceptor (
          ACE_TEXT_CHAR_TO_TCHAR (artifact.c_str ()),
          ACE_TEXT_CHAR_TO_TCHAR (entrypt.c_str ()),
          open_mode,
          config);
  }

  void
  Interceptor_Handler_Impl::remove_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
  }

  void
  Interceptor_Handler_Impl::connect_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Interceptor_Handler_Impl::disconnect_instance (const Deployment::DeploymentPlan &,
    uint32_t)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Interceptor_Handler_Impl::configure (const ::Deployment::Properties &)
  {
  }
}
