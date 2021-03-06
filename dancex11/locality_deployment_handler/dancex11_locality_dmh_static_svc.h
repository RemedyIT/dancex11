/**
 * @file    dancex11_locality_dmh_static_svc.h
 * @author  Martin Corino
 *
 * @brief   Static service loader for Locality deployment handler.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_LOCALITY_DMH_STATIC_SVC_H_
#define DANCEX11_LOCALITY_DMH_STATIC_SVC_H_

#pragma once

#include "dancex11_locality_dmh_loader.h"

#if !defined (DANCEX11_LOCALITY_DMH_SVC_NAME)
# define DANCEX11_LOCALITY_DMH_SVC_NAME nullptr
#endif

namespace DAnCEX11
{
  static int DAnCEX11_Requires_Locality_DMHandler_Loader_Svc_Initializer =
      Locality_DMHandler_Loader::Initializer (DANCEX11_LOCALITY_DMH_SVC_NAME);
}

#endif /* DANCEX11_LOCALITY_DMH_STATIC_SVC_H_ */
