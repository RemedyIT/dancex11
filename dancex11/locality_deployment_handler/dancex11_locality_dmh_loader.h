/**
 * @file    dancex11_locality_dmh_loader.h
 * @author  Martin Corino
 *
 * @brief   Service loader for Locality DMH implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_LOCALITY_DMH_LOADER_H_
#define DANCEX11_LOCALITY_DMH_LOADER_H_

#pragma once

#include "dancex11/core/dancex11_dmh_loader.h"

#include "dancex11_locality_deployment_handler_export.h"

namespace DAnCEX11
{
  class DANCEX11_LOCALITY_DH_Export Locality_DMHandler_Loader
  : public DMHandler_Loader
  {
  public:
    /// The destructor
    ~Locality_DMHandler_Loader () override = default;

    /// Initializes handler on dynamic loading.
    int init (int argc, ACE_TCHAR *argv[]) override;

    /**
     * Create and activate an DMH instance.
     * This method cannot throw any exception, but it can return a nil
     * object to indicate an error condition.
     */
    CORBA::object_reference<DeploymentManagerHandler>
    create_handler () override;

    static int Initializer (const ACE_TCHAR*);

  private:
    std::string uuid_;
    std::string activator_ior_;
  };

  // Declares the static Service descriptor
  ACE_STATIC_SVC_DECLARE (Locality_DMHandler_Loader)
  // Declares the factory method for the Service.
  // For DMHandler_Loader services the name of factory method is expected to
  // be standardized to '_make_DAnCEX11_DeploymentHandler_Impl'.
  ACE_FACTORY_DECLARE (DANCEX11_LOCALITY_DH, DAnCEX11_DeploymentHandler_Impl)
}

#endif /* DANCEX11_LOCALITY_DMH_LOADER_H_ */
