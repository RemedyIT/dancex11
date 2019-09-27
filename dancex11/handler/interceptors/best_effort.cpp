/**
 * @file    best_effort.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#include "best_effort.h"

#include "dancex11/deployment/deployment_starterrorC.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  Best_Effort::~Best_Effort ()
  {
  }

  void
  Best_Effort::configure (const ::Deployment::Properties & )
  {
  }

  void
  Best_Effort::post_install (const Deployment::DeploymentPlan &plan,
    uint32_t index,
    const CORBA::Any &,
    const CORBA::Any &exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Best_Effort::post_install - " <<
                            "Received exception while installing instance " <<
                            "<" << plan.instance ()[index].name () << ">:<" <<
                            DAnCEX11::Utility::stringify_exception_from_any (exception) << ">");
      }
  }

  void
  Best_Effort::post_connect (const Deployment::DeploymentPlan &plan,
    uint32_t connection,
    const CORBA::Any &exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Best_Effort::post_connect - " <<
                            "Received exception while establishing connection " <<
                            "<" << plan.connection ()[connection].name () << ">:<" <<
                            DAnCEX11::Utility::stringify_exception_from_any (exception) << ">");
      }
  }

  void
  Best_Effort::post_configured (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any &exception )
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Best_Effort::post_configured - " <<
                            "Received exception while completing configuration of " <<
                            "instance <" << plan.instance ()[index].name () <<
                            ">:<" << DAnCEX11::Utility::stringify_exception_from_any (exception) << ">");
      }
  }

  void
  Best_Effort::post_activate (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any & exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Best_Effort::post_activate - " <<
                            "Received exception while activating instance " <<
                            "<" << plan.instance ()[index].name () << ">:<" <<
                            DAnCEX11::Utility::stringify_exception_from_any (exception) << ">");
      }
  }


  void
  Best_Effort::post_passivate (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any & exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Best_Effort::post_passivate - " <<
                            "Received exception while passivating instance " <<
                            "<" << plan.instance ()[index].name () << ">:<" <<
                            DAnCEX11::Utility::stringify_exception_from_any (exception) << ">");
      }
  }

  void
  Best_Effort::post_remove (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any & exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Best_Effort::post_remove - " <<
                            "Received exception while removing instance <" <<
                            plan.instance ()[index].name () << ">:<" <<
                            DAnCEX11::Utility::stringify_exception_from_any (exception) << ">");

      }
  }
}

extern "C"
{
  void
  create_DAnCEX11_Best_Effort (
    IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type& plugin)
  {
    plugin = CORBA::make_reference<DAnCEX11::Best_Effort> ();
  }
}
