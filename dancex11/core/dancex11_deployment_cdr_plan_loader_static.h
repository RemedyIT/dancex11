// -*- C++ -*-
/**
 * @file dancex11_deployment_cdr_plan_loader_static.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DEPLOYMENT_CDR_PLAN_LOADER_STATIC_H
#define DANCEX11_DEPLOYMENT_CDR_PLAN_LOADER_STATIC_H

#pragma once

#include "dancex11_deployment_cdr_plan_loader.h"

namespace DAnCEX11
{
  static int DAnCEX11_Requires_CDRPlan_Loader_Svc_Initializer =
      CDRPlan_Loader_Svc::Initializer ();
};

#endif /* DANCEX11_DEPLOYMENT_CDR_PLAN_LOADER_STATIC_H */
