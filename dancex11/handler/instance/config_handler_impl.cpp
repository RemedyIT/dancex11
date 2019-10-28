// -*- C++ -*-
/**
 * @file    config_handler_impl.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "config_handler_impl.h"

#include "dancex11/core/dancex11_propertiesC.h"
#include "ace/DLL.h"

namespace DAnCEX11
{
  Config_Handler_Impl::Config_Handler_Impl (
      Plugin_Manager& plugins)
    : plugins_ (plugins)
  {
  }

  Config_Handler_Impl::~Config_Handler_Impl ()
  {
  }

  CORBA::StringSeq
  Config_Handler_Impl::dependencies ()
  {
    CORBA::StringSeq retval;
    return retval;
  }

  void
  Config_Handler_Impl::close ()
  {
  }

  std::string
  Config_Handler_Impl::instance_type ()
  {
    DANCEX11_LOG_TRACE ("Config_Handler_Impl::instance_type");
    return "nl.remedy.it.DnCX11.InstanceHandler";
  }

  void
  Config_Handler_Impl::install_instance (const Deployment::DeploymentPlan &plan,
    uint32_t instanceRef,
    CORBA::Any &)
  {
    DANCEX11_LOG_TRACE ("Config_Handler_Impl::install_instance");
    if (plan.instance ().size () <= instanceRef)
    {
      DANCEX11_LOG_ERROR ("Config_Handler_Impl::install_instance - " <<
                          "Invalid instance reference " << instanceRef << " provided " <<
                          "to install_instance");
      throw Deployment::PlanError (plan.UUID (),
                                      "Invalid instance reference");
    }

    Deployment::InstanceDeploymentDescription const &idd =
      plan.instance ()[instanceRef];

    if (plan.implementation ().size () <= idd.implementationRef ())
    {
      DANCEX11_LOG_ERROR ("Config_Handler_Impl::install_instance - " <<
                          "Invalid implementation reference " << idd.implementationRef () << " provided " <<
                          "to install_instance");
      throw ::Deployment::PlanError (plan.UUID (),
                                      "Invalid Implementation reference");
    }

    Deployment::MonolithicDeploymentDescription const &mdd =
      plan.implementation ()[idd.implementationRef ()];

    DANCEX11_LOG_DEBUG ("Config_Handler_Impl::install_instance - " <<
                        "Starting installation of instance <" << idd.name () << ">");

    std::string entrypoint;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_FACTORY,
      mdd.execParameter (),
      entrypoint))
    {
      DANCEX11_LOG_ERROR ("Config_Handler_Impl::install_instance - " <<
                          "No entrypoint found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No entrypoint found for plug-in initialization\n");
    }

    std::string artifact;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_ARTIFACT,
      mdd.execParameter (),
      artifact))
    {
      DANCEX11_LOG_ERROR ("Config_Handler_Impl::install_instance - " <<
                          "No artifact found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No artifact found for plug-in initialization\n");
    }

    uint32_t open_mode = ACE_DEFAULT_SHLIB_MODE;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_OPENMODE,
      mdd.execParameter (),
      open_mode))
    {
      DANCEX11_LOG_INFO ("Config_Handler_Impl::install_instance - " <<
                         "No open mode found for plug-in initialization");
    }

    this->plugins_.register_configuration_plugin (
      ACE_TEXT_CHAR_TO_TCHAR (artifact.c_str ()),
      ACE_TEXT_CHAR_TO_TCHAR (entrypoint.c_str ()),
      open_mode);
  }

  void
  Config_Handler_Impl::remove_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
  }

  void
  Config_Handler_Impl::connect_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Config_Handler_Impl::disconnect_instance (const Deployment::DeploymentPlan &,
    uint32_t)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Config_Handler_Impl::configure (const ::Deployment::Properties &)
  {
  }
}
