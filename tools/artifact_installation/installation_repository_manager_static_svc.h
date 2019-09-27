/**
 * @file    installation_repository_manager_static_svc.h
 * @author  Martin Corino
 *
 * @brief   Static service loader for Installation Repository Manager service.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_INSTALLATION_REPOMAN_STATIC_SVC_H_
#define DANCEX11_INSTALLATION_REPOMAN_STATIC_SVC_H_

#pragma once

#include "installation_repository_manager_impl.h"

#if !defined (DANCEX11_LOCALITY_DMH_SVC_NAME)
# define DANCEX11_LOCALITY_DMH_SVC_NAME nullptr
#endif

namespace DAnCEX11
{
  static int DAnCE_Requires_InstallationRepositoryManager_Service_Initializer =
    InstallationRepositoryManagerSvc_Impl::Initializer ();
}

#endif /* DANCEX11_INSTALLATION_REPOMAN_STATIC_SVC_H_ */
