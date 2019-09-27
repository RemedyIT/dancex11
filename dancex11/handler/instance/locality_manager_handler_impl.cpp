// -*- C++ -*-
/**
 * @file locality_manager_handler_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "locality_manager_handler_impl.h"
//#include "dancex11/scheduler/plugin_manager.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_deployment_state.h"

namespace DAnCEX11
{
  const std::string
  Locality_Handler_i::instance_type_ =
      DAnCEX11::DANCE_LOCALITYMANAGER;

  // Implementation skeleton constructor
  Locality_Handler_i::
  Locality_Handler_i ()
  {
  }

  // Implementation skeleton destructor
  Locality_Handler_i::~Locality_Handler_i ()
  {
  }

  void
  Locality_Handler_i::close ()
  {
  }

  CORBA::StringSeq
  Locality_Handler_i::dependencies ()
  {
    return CORBA::StringSeq ();
  }

  std::string Locality_Handler_i::instance_type ()
  {
    DANCEX11_LOG_TRACE ("Locality_Handler_i::instance_type");
    return Locality_Handler_i::instance_type_;
  }

  void
  Locality_Handler_i::install_instance (const ::Deployment::DeploymentPlan & plan,
                                        uint32_t instanceRef,
                                        CORBA::Any& instance_reference)
  {
    DANCEX11_LOG_TRACE ("Locality_Handler_i::install_instance - plan " << plan.UUID ());

#if !defined(NDEBUG)
    DANCEX11_LOG_TRACE ("Locality_Handler_i::install_instance - " <<
                        "Deploying instance " << instanceRef << " of plan : " <<
                        IDL::traits< ::Deployment::DeploymentPlan>::write (plan));
#endif

    if (plan.instance ().size () <= instanceRef)
      {
        DANCEX11_LOG_CRITICAL ("Locality_Handler_i::install_instance - " <<
                               "Invalid instance reference " <<
                               instanceRef << " provided " <<
                               "to install_instance");
        throw ::Deployment::PlanError (plan.UUID (),
                                       "Invalid instance reference");
      }

    const ::Deployment::InstanceDeploymentDescription &idd =
      plan.instance ()[instanceRef];

    if (plan.implementation ().size () <= idd.implementationRef ())
      {
        DANCEX11_LOG_CRITICAL ("Locality_Handler_i::install_instance - " <<
                               "Invalid implementation reference " <<
                               idd.implementationRef () << " provided " <<
                               "to install_instance");
        throw ::Deployment::PlanError (plan.UUID (),
                                       "Invalid Implementation reference");
      }

    const ::Deployment::MonolithicDeploymentDescription &mdd =
      plan.implementation ()[idd.implementationRef ()];

    DANCEX11_LOG_TRACE ("Locality_Handler_i::install_instance - "
                        "Starting installation of instance <" <<
                        idd.name () << ">");

    ::Deployment::Properties allprops;

    Utility::append_properties (allprops,
                                mdd.execParameter ());
    Utility::append_properties (allprops,
                                idd.configProperty ());

    DANCEX11_LOG_TRACE ("Locality_Handler_i::install_instance - " <<
                        "Passing " << allprops.size () << " properties to activator");

    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type lm_ref =
        this->activator_->create_locality_manager (plan,
                                                   instanceRef,
                                                   allprops);

    if (!lm_ref)
      {
        DANCEX11_LOG_ERROR ("Locality_Handler_i::install_instance - " \
                            "Received nil reference from LocalityActivator");
        throw ::Deployment::StartError (idd.name (),
                                        "Received nil object reference for "
                                        "LocalityManager from Activator");
      }

    instance_reference <<= lm_ref;
  }

  void
  Locality_Handler_i::remove_instance (const ::Deployment::DeploymentPlan &plan,
                                       uint32_t,
                                       const CORBA::Any &)
  {
    this->activator_->remove_locality_manager (plan);
  }

  void
  Locality_Handler_i::connect_instance (const ::Deployment::DeploymentPlan &,
                                        uint32_t,
                                        const CORBA::Any &)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Locality_Handler_i::disconnect_instance (const ::Deployment::DeploymentPlan &,
                                           uint32_t)
  {
    throw CORBA::NO_IMPLEMENT ();
  }

  void
  Locality_Handler_i::configure (const ::Deployment::Properties &prop )
  {
    DANCEX11_LOG_DEBUG ("Locality_Handler_i - " \
                        "Spawning Locality manager");

    this->activator_ =
        CORBA::make_reference<DAnCE_LocalityActivator_i> (
              prop,
              false,
              DAnCEX11::State::instance ()->orb (),
              DAnCEX11::State::instance ()->root_poa ());
    if (!this->activator_)
      throw CORBA::NO_MEMORY ();

    DAnCEX11::State::instance ()->root_poa ()->activate_object (this->activator_);
  }
}

extern "C"
{
  void create_DAnCEX11_Locality_Handler (
    IDL::traits< ::DAnCEX11::InstanceDeploymentHandler>::ref_type& handler)
  {
    handler = CORBA::make_reference< ::DAnCEX11::Locality_Handler_i> ();
  }
}
