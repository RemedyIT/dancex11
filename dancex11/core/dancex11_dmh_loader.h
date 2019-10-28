/**
 * @file    dancex11_dmh_loader.h
 * @author  Martin Corino
 *
 * @brief   DAnCEX11 DeploymentManagerHandler service object loader.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_DMH_LOADER_H
#define DAnCEX11_DMH_LOADER_H

#pragma once

#include "ace/Service_Object.h"

#include "dancex11_deploymentmanagerhandler_skel_export.h"
#include "dancex11_deploymentmanagerhandlerC.h"

namespace DAnCEX11
{
  /**
   * @class DAnCEX11_DMHandler_Loader
   *
   * @brief A class to dynamically load DMH implementations.
   */
  class DANCEX11_DMH_STUB_Export DMHandler_Loader
    : public ACE_Service_Object
  {
  public:
    /// The destructor
    virtual ~DMHandler_Loader ();

    /**
     * Create and activate an DMH instance.
     * This method cannot throw any exception, but it can return a nil
     * object to indicate an error condition.
     */
    virtual CORBA::object_reference<DAnCEX11::DeploymentManagerHandler>
    create_handler () = 0;
  };
}

#endif /* DAnCEX11_DMH_LOADER_H */
