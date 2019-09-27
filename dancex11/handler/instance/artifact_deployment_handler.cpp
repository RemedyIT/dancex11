// -*- C++ -*-
/**
 * @file   artifact_deployment_handler.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "artifact_deployment_handler.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"

#include <set>

namespace DAnCEX11
{
  const std::string
  Artifact_Deployment_Handler_i::instance_type_ =
    DAnCEX11::DANCE_ARTIFACT;

  // Implementation skeleton constructor
  Artifact_Deployment_Handler_i::
  Artifact_Deployment_Handler_i ()
  {
  }

  // Implementation skeleton destructor
  Artifact_Deployment_Handler_i::~Artifact_Deployment_Handler_i ()
  {
  }

  void
  Artifact_Deployment_Handler_i::close ()
  {
    this->installer_->clear ();
  }

  CORBA::StringSeq
  Artifact_Deployment_Handler_i::dependencies ()
  {
    return {};
  }

  std::string Artifact_Deployment_Handler_i::instance_type ()
  {
    DANCEX11_LOG_TRACE ("Artifact_Deployment_Handler_i::instance_type");
    return Artifact_Deployment_Handler_i::instance_type_;
  }

  void
  Artifact_Deployment_Handler_i::install_instance (const Deployment::DeploymentPlan & plan,
                                                   uint32_t instanceRef,
                                                   CORBA::Any& artifact_locations)
  {
    DANCEX11_LOG_TRACE ("Artifact_Deployment_Handler_i::install_instance - plan " <<
                        plan.UUID () << "; instance " << instanceRef);

#if !defined(NDEBUG)
    DANCEX11_LOG_TRACE ("Artifact_Deployment_Handler_i::install_instance - " <<
                        "Deploying artifact " << instanceRef << " of plan : " <<
                        IDL::traits< ::Deployment::DeploymentPlan>::write (plan));
#endif

    if (plan.instance ().size () <= instanceRef)
    {
      DANCEX11_LOG_CRITICAL ("Artifact_Deployment_Handler_i::install_instance - " <<
                             "Invalid instance reference " <<
                             instanceRef << " provided " <<
                             "to install_instance");
      throw ::Deployment::PlanError (plan.UUID (),
                                     "Invalid instance reference");
    }

    std::set<std::string> artifact_paths;

    if (DAnCEX11::ArtifactInstallation_Impl::handlers_available ())
    {
      // get instance deployment descriptors
      const ::Deployment::InstanceDeploymentDescription& idd =
          plan.instance ()[instanceRef];
      const ::Deployment::MonolithicDeploymentDescription& mdd =
          plan.implementation ()[idd.implementationRef ()];

      // install all artifacts referenced by this instance/implementation
      for (uint32_t artifactRef : mdd.artifactRef ())
      {
        // get artifact deployment descriptor
        Deployment::ArtifactDeploymentDescription &add =
          const_cast< Deployment::ArtifactDeploymentDescription &> (
              plan.artifact ()[artifactRef]);

        // install artifact
        // artifacts are reference counted so in case multiple instances use the same
        // artifact installation does not happen more than once just the refcount is
        // incremented
        this->installer_->install(plan.UUID (), add);

        // get the full path to the installed artifact
        std::string location =
          this->installer_->get_artifact_location (plan.UUID (), add.name ());

        // modify artifact descriptor location field to reflect the installed
        // artifact's location
        // store installed location as first in list shifting backwards all others
        add.location ().insert (
            add.location ().begin (),
            location);

        // record unique installation paths
        std::string::size_type pos = location.rfind (ACE_DIRECTORY_SEPARATOR_CHAR_A);
        if (pos != std::string::npos)
        {
          DANCEX11_LOG_INFO ("Artifact_Deployment_Handler_i::install_instance - " <<
                             "registering installation path: " <<
                             location.substr (0, pos>0 ? pos : 1));

          artifact_paths.insert (location.substr (0, pos>0 ? pos : 1));
        }
      }
    }
    else
    {
      DANCEX11_LOG_INFO ("Artifact_Deployment_Handler_i::install_instance - " <<
                         "no artifact installation handlers available");
    }
    // return any collected unique installation paths
    artifact_locations <<= std::vector<std::string> (
                              artifact_paths.begin (),
                              artifact_paths.end ());
  }

  void
  Artifact_Deployment_Handler_i::remove_instance (const ::Deployment::DeploymentPlan &plan,
                                                  uint32_t instanceRef,
                                                  const CORBA::Any &)
  {
    DANCEX11_LOG_TRACE ("Artifact_Deployment_Handler_i::remove_instance - plan " <<
                        plan.UUID () << "; instance " << instanceRef);


    if (plan.instance ().size () <= instanceRef)
    {
      DANCEX11_LOG_CRITICAL ("Artifact_Deployment_Handler_i::remove_instance - " <<
                             "Invalid instance reference " <<
                             instanceRef << " provided " <<
                             "to install_instance");
      throw ::Deployment::PlanError (plan.UUID (),
                                     "Invalid instance reference");
    }

    if (DAnCEX11::ArtifactInstallation_Impl::handlers_available ())
    {
      // get instance deployment descriptors
      const ::Deployment::InstanceDeploymentDescription& idd =
          plan.instance ()[instanceRef];
      const ::Deployment::MonolithicDeploymentDescription& mdd =
          plan.implementation ()[idd.implementationRef ()];

      // remove all artifacts referenced by this instance/implementation
      for (uint32_t artifactRef : mdd.artifactRef ())
      {
        // get artifact deployment descriptor
        Deployment::ArtifactDeploymentDescription &add =
          const_cast< Deployment::ArtifactDeploymentDescription &> (
              plan.artifact ()[artifactRef]);

        // remove artifact
        // artifacts are reference counted so in case multiple instances use the same
        // artifact removal does not happen more than once but only when the refcount
        // has been decremented to zero
        this->installer_->remove (plan.UUID (), add.name ());
      }
    }
  }

  void
  Artifact_Deployment_Handler_i::configure (const ::Deployment::Properties &/*prop*/ )
  {
    DANCEX11_LOG_DEBUG ("Artifact_Deployment_Handler_i::configure - "
                        "setting up artifact installer");

    // Retrieving in process artifact installation service
    this->installer_ =
        DAnCEX11::ArtifactInstallation_Impl::instance ();

    this->installer_->initialize ();
  }


  Artifact_Deployment_Initializer::~Artifact_Deployment_Initializer ()
  {
  }

  void
  Artifact_Deployment_Initializer::preprocess_plan (
      Deployment::DeploymentPlan & plan)
  {
    DANCEX11_LOG_TRACE ("Artifact_Deployment_Initializer::preprocess_plan - plan: " <<
                        plan.UUID ());

    // check if there is any installation handler
    if (DAnCEX11::ArtifactInstallation_Impl::handlers_available ())
    {
      // prepare the installation service for the plan
      DAnCEX11::ArtifactInstallation_Impl::instance ()->prepare (plan);
    }
  }
}

extern "C"
{
  void create_DAnCEX11_Artifact_Deployment_Handler (
    IDL::traits< ::DAnCEX11::InstanceDeploymentHandler>::ref_type& plugin)
  {
    plugin = CORBA::make_reference< ::DAnCEX11::Artifact_Deployment_Handler_i> ();
  }

  void create_DAnCEX11_Artifact_Deployment_Initializer (
    IDL::traits< ::DAnCEX11::DeploymentInterceptor>::ref_type& plugin)
  {
    plugin = CORBA::make_reference< ::DAnCEX11::Artifact_Deployment_Initializer> ();
  }
}
