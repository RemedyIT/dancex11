/**
 * @file    dancex11_domain_dmh_loader.h
 * @author  Martin Corino
 *
 * @brief   Service loader for Domain DMH implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_DOMAIN_DMH_LOADER_H_
#define DANCEX11_DOMAIN_DMH_LOADER_H_

#pragma once

#include "dancex11/core/dancex11_dmh_loader.h"

#include "dancex11_domain_deployment_handler_export.h"

namespace DAnCEX11
{
  class DANCEX11_DOMAIN_DH_Export Domain_DMHandler_Loader
  : public DMHandler_Loader
  {
  public:
    /// The destructor
    virtual ~Domain_DMHandler_Loader () = default;

    /// Initializes handler on dynamic loading.
    virtual int init (int argc, ACE_TCHAR *argv[]) override;

    /**
     * Create and activate an DMH instance.
     * This method cannot throw any exception, but it can return a nil
     * object to indicate an error condition.
     */
    virtual CORBA::object_reference<DeploymentManagerHandler>
    create_handler () override;

    static int Initializer (const ACE_TCHAR*);

  private:
    std::vector<std::string> nodes_;
    std::string cdd_;
    std::string cddfmt_;
  };

  // Declares the static Service descriptor
  ACE_STATIC_SVC_DECLARE (Domain_DMHandler_Loader)
  // Declares the factory method for the Service.
  // For DMHandler_Loader services the name of factory method is expected to
  // be standardized to '_make_DAnCEX11_DeploymentHandler_Impl'.
  ACE_FACTORY_DECLARE (DANCEX11_DOMAIN_DH, DAnCEX11_DeploymentHandler_Impl)
}

#endif /* DANCEX11_DOMAIN_DMH_LOADER_H_ */
