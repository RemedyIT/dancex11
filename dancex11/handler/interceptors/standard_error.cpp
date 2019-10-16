/**
 * @file    standard_error.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "standard_error.h"

#include "dancex11/deployment/deployment_starterrorC.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  Standard_Error::~Standard_Error ()
  {
  }

  void
  Standard_Error::configure (const ::Deployment::Properties & )
  {
  }

  void
  Standard_Error::post_install (const Deployment::DeploymentPlan &plan,
    uint32_t index,
    const CORBA::Any &,
    const CORBA::Any &exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Standard_Error::post_install - " <<
                            "Propagating exception from instance <" <<
                            plan.instance ()[index].name () << ">");
        DAnCEX11::Utility::throw_exception_from_any (exception);
      }
  }

  void
  Standard_Error::post_connect (const Deployment::DeploymentPlan &plan,
    uint32_t connection,
    const CORBA::Any &exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Standard_Error::post_connect - " <<
                            "Propagating exception from connection <" <<
                            plan.connection ()[connection].name () << ">");
        DAnCEX11::Utility::throw_exception_from_any (exception);
      }
  }

  void
  Standard_Error::post_configured (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any &exception )
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Standard_Error::post_configured - " <<
                            "Propagating exception from configuring instance <" <<
                            plan.instance ()[index].name () << ">");
        DAnCEX11::Utility::throw_exception_from_any (exception);
      }
  }

  void
  Standard_Error::post_activate (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any & exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Standard_Error::post_activate - " <<
                            "Propagating exception from activation of instance <" <<
                            plan.instance ()[index].name () << ">");
        DAnCEX11::Utility::throw_exception_from_any (exception);
      }
  }

  void
  Standard_Error::post_passivate (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any & exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Standard_Error::post_passivate - " <<
                            "Propagating exception from passivation of instance <" <<
                            plan.instance ()[index].name () << ">");
        DAnCEX11::Utility::throw_exception_from_any (exception);
      }
  }

  void
  Standard_Error::post_remove (const Deployment::DeploymentPlan & plan,
    uint32_t index,
    const CORBA::Any & exception)
  {
    if (exception.type()->kind () != CORBA::TCKind::tk_null)
      {
        DANCEX11_LOG_ERROR ("Standard_Error::post_remove - " <<
                            "Propagating exception from removal of instance <" <<
                            plan.instance ()[index].name () << ">");
        DAnCEX11::Utility::throw_exception_from_any (exception);
      }
  }
}

extern "C"
{
  void
  create_DAnCEX11_Standard_Error (
    IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type& plugin)
  {
    plugin = CORBA::make_reference<DAnCEX11::Standard_Error> ();
  }
}
