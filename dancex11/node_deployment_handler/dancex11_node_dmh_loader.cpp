/**
 * @file    dancex11_node_dmh_loader.cpp
 * @author  Martin Corino
 *
 * @brief   Service loader for Node DMH implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_node_dmh_loader.h"
#include "dancex11_node_deployment_handler.h"

#include "ace/Service_Config.h"

namespace DAnCEX11
{
  // Type to help declare the standardized factory name
  using DAnCEX11_DeploymentHandler_Impl = Node_DMHandler_Loader;

  // Define the static Service descriptor.
  // Note the standardized name for the factory method declaration.
  ACE_STATIC_SVC_DEFINE (Node_DMHandler_Loader,
                         ACE_TEXT ("DANCEX11_DEPLOYMENT_HANDLER"),
                         ACE_SVC_OBJ_T,
                         &ACE_SVC_NAME (DAnCEX11_DeploymentHandler_Impl),
                         ACE_Service_Type::DELETE_THIS
                          | ACE_Service_Type::DELETE_OBJ,
                         0)
  // Define implementation for Service factory method and cleanup method
  // (gobbler).
  // Instead of the real classname a type is used to force generation
  // of the standardized factory method name.
  ACE_FACTORY_DEFINE (DANCEX11_NODE_DH, DAnCEX11_DeploymentHandler_Impl)

  Node_DMHandler_Loader::~Node_DMHandler_Loader ()
  {
  }

  CORBA::object_reference<DeploymentManagerHandler>
  Node_DMHandler_Loader::create_handler ()
  {
    return CORBA::make_reference<NodeDeploymentHandler> ();
  }

  int
  Node_DMHandler_Loader::Initializer (const ACE_TCHAR* svcname)
  {
    if (svcname)
    {
      ace_svc_desc_Node_DMHandler_Loader.name_ = svcname;
    }
    return ACE_Service_Config::process_directive (ace_svc_desc_Node_DMHandler_Loader);
  }
}
