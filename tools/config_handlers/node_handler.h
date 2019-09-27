/**
 * @file    node_handler.h
 * @author  Johnny Willemsen
 *
  * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIG_HANDLERS_NODE_HANDLER_H
#define DANCEX11_CONFIG_HANDLERS_NODE_HANDLER_H

#pragma once

#include "cdd.hpp"
#include "dancex11/deployment/deployment_targetdataC.h"

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    Deployment::Node
    convert_node (const DAnCE::Config_Handlers::Node& src);
  }
}

#endif /* DANCEX11_CONFIG_HANDLERS_NODE_HANDLER_H */
