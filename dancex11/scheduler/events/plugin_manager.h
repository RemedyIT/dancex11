/**
 * @file    plugin_manager.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_PLUGIN_MANAGER_H
#define DAnCEX11_PLUGIN_MANAGER_H

#include "dancex11/scheduler/deployment_scheduler_export.h"
#include "dancex11/scheduler/dependency_sorter.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/core/dancex11_deploymentconfiguratorsC.h"
#include "dancex11/core/dancex11_deploymentinterceptorsC.h"
#include "dancex11/core/dancex11_deploymenthandlersC.h"
#include "dancex11/core/dancex11_utility.h"

#include <map>
#include <set>
#include <list>

namespace DAnCEX11
{
  /**
   * @class Plugin_Manager
   * @brief Manager of plugins
   */
  class Deployment_Scheduler_Export Plugin_Manager final
  {
  public:
    Plugin_Manager () = default;

    ~Plugin_Manager ();

    /// Set the configuration of the plugin_manager
    void
    set_configuration (const Deployment::Properties &config);

    /// Add to the configuration of the plugin_manager
    void
    add_configuration (const Deployment::Properties &config);

    using IH_DEPS = Dependency_Sorter::IH_DEPS;

    /// Registers another installation handler.
    std::string
    register_installation_handler (
        const std::string& artifact,
        const std::string& entrypoint,
        const IH_DEPS &dependencies,
        uint32_t open_mode,
        Utility::PROPERTY_MAP& config);

    /// Registers another deployment interceptor
    void
    register_interceptor (
        const std::string& artifact,
        const std::string& entrypoint,
        uint32_t open_mode,
        Utility::PROPERTY_MAP& config);

    using INSTALL_ORDER = Dependency_Sorter::INSTALL_ORDER;

    void
    get_installation_order (INSTALL_ORDER &);

    IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type
    fetch_installation_handler (const std::string &instance_type);

    using INTERCEPTORS = std::list <IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type>;

    const INTERCEPTORS &
    fetch_interceptors ();

    void
    register_configuration_plugin (
        const std::string& artifact,
        const std::string& entrypoint,
        uint32_t open_mode);

    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type
    get_configuration_handler (const std::string& id);

    void
    register_service (const std::string& artifact,
                      const std::string& entrypoint,
                      const std::string& arguments,
                      bool ignore_load_error);

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Plugin_Manager (const Plugin_Manager&) = delete;
    Plugin_Manager (Plugin_Manager&&) = delete;
    Plugin_Manager& operator= (const Plugin_Manager& x) = delete;
    Plugin_Manager& operator= (Plugin_Manager&& x) = delete;
    //@}
  private:
    Deployment::Properties config_;

    using HANDLER_MAP_PAIR = std::pair <std::string,
      IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type>;
    using HANDLER_MAP = std::map <std::string,
      IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type>;

    using CONFIG_MAP_PAIR = std::pair <std::string,
      IDL::traits<DeploymentConfiguration>::ref_type>;
    using CONFIG_MAP = std::map <std::string,
      IDL::traits<DeploymentConfiguration>::ref_type>;

    using SVCOBJ_LIST = std::vector <std::string>;

    HANDLER_MAP handler_map_;

    INTERCEPTORS interceptors_;

    CONFIG_MAP config_plugins_;

    SVCOBJ_LIST service_objects_;

    Dependency_Sorter ih_dep_;
  };
}

#endif /* DAnCEX11_PLUGIN_MANAGER_H */

