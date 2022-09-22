// -*- C++ -*-
/**
 * @file    service_object_handler_impl.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "service_object_handler_impl.h"

#include "dancex11/core/dancex11_propertiesC.h"

namespace DAnCEX11
{
  Service_Object_Handler_Impl::Service_Object_Handler_Impl (
      Plugin_Manager& plugins)
    : plugins_ (plugins)
  {
  }

  CORBA::StringSeq
  Service_Object_Handler_Impl::dependencies ()
  {
    return {};
  }

  void
  Service_Object_Handler_Impl::close ()
  {
  }

  std::string
  Service_Object_Handler_Impl::instance_type ()
  {
    DANCEX11_LOG_TRACE ("Service_Object_Handler_Impl::instance_type");
    return "nl.remedy.it.DnCX11.ServiceObject";
  }

  void
  Service_Object_Handler_Impl::install_instance (const Deployment::DeploymentPlan &plan,
    uint32_t instanceRef,
    CORBA::Any &)
  {
    DANCEX11_LOG_TRACE ("Service_Object_Handler_Impl::install_instance");
    if (plan.instance ().size () <= instanceRef)
    {
      DANCEX11_LOG_ERROR ("Service_Object_Handler_Impl::install_instance - " <<
                          "Invalid instance reference " << instanceRef << " provided " <<
                          "to install_instance");
      throw Deployment::PlanError (plan.UUID (),
                                      "Invalid instance reference");
    }

    Deployment::InstanceDeploymentDescription const &idd =
      plan.instance ()[instanceRef];

    if (plan.implementation ().size () <= idd.implementationRef ())
    {
      DANCEX11_LOG_ERROR ("Service_Object_Handler_Impl::install_instance - " <<
                          "Invalid implementation reference " << idd.implementationRef () << " provided " <<
                          "to install_instance");
      throw ::Deployment::PlanError (plan.UUID (),
                                      "Invalid Implementation reference");
    }

    Deployment::MonolithicDeploymentDescription const &mdd =
      plan.implementation ()[idd.implementationRef ()];

    DANCEX11_LOG_DEBUG ("Service_Object_Handler_Impl::install_instance - " <<
                        "Starting installation of instance <" << idd.name () << ">");

    std::string entrypoint;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_FACTORY,
      mdd.execParameter (),
      entrypoint))
    {
      DANCEX11_LOG_ERROR ("Service_Object_Handler_Impl::install_instance - " <<
                          "No entrypoint found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No entrypoint found for plug-in initialization\n");
    }

    std::string artifact;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_ARTIFACT,
      mdd.execParameter (),
      artifact))
    {
      DANCEX11_LOG_ERROR ("Service_Object_Handler_Impl::install_instance - " <<
                          "No artifact found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No artifact found for plug-in initialization\n");
    }

    std::string arguments;
    if (!DAnCEX11::Utility::get_property_value (this->instance_type () + ".Arguments",
      mdd.execParameter (),
      arguments))
    {
      if (!DAnCEX11::Utility::get_property_value (this->instance_type () + ".Arguments",
        idd.configProperty (),
        arguments))
      {
        DANCEX11_LOG_INFO ("Service_Object_Handler_Impl::install_instance - " <<
                           "No arguments found for plug-in initialization");
      }
    }

    bool ignore_load_error = false;
    if (!DAnCEX11::Utility::get_property_value (this->instance_type () + ".IgnoreLoadError",
      mdd.execParameter (),
      ignore_load_error))
    {
      if (!DAnCEX11::Utility::get_property_value (this->instance_type () + ".IgnoreLoadError",
        idd.configProperty (),
        ignore_load_error))
      {
        DANCEX11_LOG_INFO ("Service_Object_Handler_Impl::install_instance - " <<
                           "No ignore load error found for plug-in initialization");
      }
    }

    // register loaded service with plugin manager
    this->plugins_.register_service (mdd.name (), artifact, entrypoint, arguments, ignore_load_error);
  }

  void
  Service_Object_Handler_Impl::remove_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
  }

  void
  Service_Object_Handler_Impl::connect_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Service_Object_Handler_Impl::disconnect_instance (const Deployment::DeploymentPlan &,
    uint32_t)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Service_Object_Handler_Impl::configure (const ::Deployment::Properties &)
  {
  }
}
