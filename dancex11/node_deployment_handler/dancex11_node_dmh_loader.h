/**
 * @file    dancex11_node_dmh_loader.h
 * @author  Martin Corino
 *
 * @brief   Service loader for Node DMH implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_NODE_DMH_LOADER_H_
#define DANCEX11_NODE_DMH_LOADER_H_

#pragma once

#include "dancex11/core/dancex11_dmh_loader.h"

#include "dancex11_node_deployment_handler_export.h"

namespace DAnCEX11
{
  class DANCEX11_NODE_DH_Export Node_DMHandler_Loader
  : public DMHandler_Loader
  {
  public:
    /// The destructor
    virtual ~Node_DMHandler_Loader ();

    /**
     * Create and activate an DMH instance.
     * This method cannot throw any exception, but it can return a nil
     * object to indicate an error condition.
     */
    virtual CORBA::object_reference<DeploymentManagerHandler>
    create_handler () override;

    static int Initializer (const ACE_TCHAR*);
  };

  // Declares the static Service descriptor
  ACE_STATIC_SVC_DECLARE (Node_DMHandler_Loader)
  // Declares the factory method for the Service.
  // For DMHandler_Loader services the name of factory method is expected to
  // be standardized to '_make_DAnCEX11_DeploymentHandler_Impl'.
  ACE_FACTORY_DECLARE (DANCEX11_NODE_DH, DAnCEX11_DeploymentHandler_Impl)
}

#endif /* DANCEX11_NODE_DMH_LOADER_H_ */
