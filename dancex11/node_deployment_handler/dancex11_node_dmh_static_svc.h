/**
 * @file    dancex11_node_dmh_static_svc.h
 * @author  Martin Corino
 *
 * @brief   Static service loader for Node deployment handler.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_NODE_DMH_STATIC_SVC_H_
#define DANCEX11_NODE_DMH_STATIC_SVC_H_

#pragma once

#include "dancex11_node_dmh_loader.h"

#if !defined (DANCEX11_NODE_DMH_SVC_NAME)
# define DANCEX11_NODE_DMH_SVC_NAME nullptr
#endif

namespace DAnCEX11
{
  static int DAnCEX11_Requires_Node_DMHandler_Loader_Svc_Initializer =
      Node_DMHandler_Loader::Initializer (DANCEX11_NODE_DMH_SVC_NAME);
}

#endif /* DANCEX11_NODE_DMH_STATIC_SVC_H_ */
