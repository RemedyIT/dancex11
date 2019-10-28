/**
 * @file    plugin_conf.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @brief   Handles installation of plugins from a configuration file.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_PLUGIN_CONF_H
#define DAnCEX11_PLUGIN_CONF_H

#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/scheduler/events/plugin_manager.h"

#include "dancex11_deployment_handler_export.h"

namespace DAnCEX11
{
  class DAnCE_Deployment_Handler_Export Plugin_Configurator final
  {
  public:
    Plugin_Configurator () = default;
    ~Plugin_Configurator () = default;

    void
    load_from_plan (Plugin_Manager& plugins,
                    const Deployment::DeploymentPlan &plan);

  private:

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Plugin_Configurator& operator= (const Plugin_Configurator& x) = delete;
    Plugin_Configurator& operator= (Plugin_Configurator&& x) = delete;
    //@}
  };
}
#endif /* DAnCEX11_PLUGIN_CONF_H */
