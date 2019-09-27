// -*- C++ -*-
/**
 * @file    inst_handler_impl.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "inst_handler_impl.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "ace/DLL.h"

namespace DAnCEX11
{
  Inst_Handler_Impl::Inst_Handler_Impl (
      Plugin_Manager& plugins)
    : plugins_ (plugins)
  {
  }

  CORBA::StringSeq
  Inst_Handler_Impl::dependencies ()
  {
    return {};
  }

  void
  Inst_Handler_Impl::close ()
  {
  }

  std::string
  Inst_Handler_Impl::instance_type ()
  {
    DANCEX11_LOG_TRACE ("Inst_Handler_Impl::instance_type");
    return "nl.remedy.it.DnCX11.InstanceHandler";
  }

  void
  Inst_Handler_Impl::install_instance (const Deployment::DeploymentPlan &plan,
    uint32_t instanceRef,
    CORBA::Any& instance_reference)
  {
    DANCEX11_LOG_TRACE ("Inst_Handler_Impl::install_instance - Called");

    if (plan.instance ().size () <= instanceRef)
    {
      DANCEX11_LOG_ERROR ("Inst_Handler_Impl::install_instance - " <<
                          "Invalid instance reference " << instanceRef << " provided " <<
                          "to install_instance");
      throw Deployment::PlanError (plan.UUID (),
        "Invalid instance reference");
    }

    Deployment::InstanceDeploymentDescription const &idd =
      plan.instance ()[instanceRef];

    if (plan.implementation ().size () <= idd.implementationRef ())
    {
      DANCEX11_LOG_ERROR ("Inst_Handler_Impl::install_instance - " <<
                          "Invalid implementation reference " << idd.implementationRef () << " provided " <<
                          "to install_instance");
      throw Deployment::PlanError (plan.UUID (),
        "Invalid Implementation reference");
    }

    Deployment::MonolithicDeploymentDescription const &mdd =
      plan.implementation ()[idd.implementationRef ()];

    DANCEX11_LOG_DEBUG ("Inst_Handler_Impl::install_instance - " <<
                        "Starting installation of instance <" << idd.name () << ">");

    std::string entrypoint;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_FACTORY,
      mdd.execParameter (),
      entrypoint))
    {
      DANCEX11_LOG_ERROR ("Inst_Handler_Impl::install_instance - " <<
                          "No entrypoint found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No entrypoint found for plug-in initialization\n");
    }

    std::string artifact;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_ARTIFACT,
      mdd.execParameter (),
      artifact))
    {
      DANCEX11_LOG_ERROR ("Inst_Handler_Impl::install_instance - " <<
                          "No artifact found for plug-in initialization");
      throw ::Deployment::StartError (idd.name (),
        "No artifact found for plug-in initialization\n");
    }

    uint32_t open_mode = ACE_DEFAULT_SHLIB_MODE;
    if (!DAnCEX11::Utility::get_property_value (DAnCEX11::DANCE_PLUGIN_OPENMODE,
      mdd.execParameter (),
      open_mode))
    {
      DANCEX11_LOG_INFO ("Inst_Handler_Impl::install_instance - " <<
                         "No open mode found for plug-in initialization");
    }

    Plugin_Manager::IH_DEPS deps;

    for (Deployment::Property property : idd.configProperty ())
    {
      if (property.name () == DAnCEX11::DANCE_IDH_DEPENDSON)
      {
        std::string id;

        if (property.value () >>= id)
        {
          deps.insert (id);
        }
        else
        {
          DANCEX11_LOG_ERROR ("Inst_Handler_Impl::install_instance - " <<
                              "Unable to extract dependency order from string");
          throw ::Deployment::StartError (idd.name (),
            "Unable to extract dependency order from string");
        }
      }
    }

    DANCEX11_LOG_DEBUG ("Inst_Handler_Impl::install_instance - " <<
                        "Registering installation handler for instance <" << idd.name () <<
                        "> artifact <" << artifact << "> entrypoint <" <<
                        entrypoint << "> openmode <" << open_mode << ">");

    Utility::PROPERTY_MAP config;
    Utility::build_property_map (config, idd.configProperty ());
    Utility::build_property_map (config, mdd.execParameter ());

    std::string plugin_id =
      this->plugins_.register_installation_handler (
          artifact,
          entrypoint,
          deps,
          open_mode,
          config);

    instance_reference <<= plugin_id;
  }

  void
  Inst_Handler_Impl::remove_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
  }

  void
  Inst_Handler_Impl::connect_instance (const Deployment::DeploymentPlan &,
    uint32_t,
    const CORBA::Any &)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Inst_Handler_Impl::disconnect_instance (const Deployment::DeploymentPlan &,
    uint32_t)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Inst_Handler_Impl::configure (const ::Deployment::Properties &)
  {
  }
}
