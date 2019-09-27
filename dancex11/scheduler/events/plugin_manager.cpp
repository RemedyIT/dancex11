/**
 * @file    plugin_manager.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#include "plugin_manager.h"

#include "ace/Service_Config.h"

#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/core/dancex11_dynamic_loader.h"

#include "dancex11/logger/log.h"

#include <sstream>

namespace DAnCEX11
{
  namespace
  {
    template <typename PLUGIN>
    typename PLUGIN::_ref_type
    load_plugin (
        const std::string& artifact,
        const std::string& entrypoint,
        uint32_t open_mode)
    {
      if (artifact.empty () || entrypoint.empty ())
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::load_plugin - " <<
                            "Must provide non-empty artifact and entrypoint names");
        throw ::Deployment::PlanError ("", "Invalid parameters for plug-in installation");
      }

      DANCEX11_LOG_DEBUG ("Plugin_Manager::load_plugin - " <<
                          "Loading plugin from <" << artifact << ">:<" << entrypoint << ">");

      std::string error;
      DL_Module plugin_mod (artifact, open_mode);
      auto plugin_factory = plugin_mod.resolve<void, typename PLUGIN::_ref_type&> (error, entrypoint);

      if (!plugin_factory)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::load_plugin - " <<
                            "Error loading plugin from <" << artifact << ">:<" << entrypoint
                            << "> :" << error);
        throw ::Deployment::PlanError (artifact, error);
      }

      typename PLUGIN::_ref_type plugin;

      // Call the plugin factory
      plugin_factory (plugin);

      if (!plugin)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::load_plugin - " <<
                            "Unable to load plugin <" << artifact << ">:<" << entrypoint <<
                            ">, nil result from factory");
        throw ::Deployment::PlanError (artifact,
                                       "Nil result from factory");
      }

      DANCEX11_LOG_DEBUG ("Plugin_Manager::load_plugin - " <<
                          "Successfully created plugin from <" << artifact <<
                          "> with mode <" << open_mode << ">:<" << entrypoint << ">");

      return plugin;
    }
  }

  template<typename T>
  struct Closer
  {
    void operator() (T& item)
    {
      item.second->close ();
    }
  };

  template<>
  struct Closer<Plugin_Manager::INTERCEPTORS::value_type>
  {
    void operator() (Plugin_Manager::INTERCEPTORS::value_type& item)
    {
      item->close ();
    }
  };

  Plugin_Manager::~Plugin_Manager ()
  {
    try
    {
      std::for_each (this->handler_map_.begin (),
                     this->handler_map_.end (),
                     Closer<HANDLER_MAP::value_type> ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("Plugin_Manager::~Plugin_Manager - " <<
                          "Caught unknown C++ exception while closing Instance handlers");
    }
    try
    {
      std::for_each (this->interceptors_.begin (),
                     this->interceptors_.end (),
                     Closer<INTERCEPTORS::value_type> ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("Plugin_Manager::~Plugin_Manager - " <<
                          "Caught unknown C++ exception while closing Interceptors");
    }
    try
    {
      std::for_each (this->config_plugins_.begin (),
                     this->config_plugins_.end (),
                     Closer<CONFIG_MAP::value_type> ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("Plugin_Manager::~Plugin_Manager - " <<
                          "Caught unknown C++ exception while closing Configurators");
    }
    try
    {
      for (const std::string& svcobj : this->service_objects_)
      {
        std::string remove = "remove "+svcobj;
        if (ACE_Service_Config::process_directive (remove.c_str ()) != 0)
        {
          DANCEX11_LOG_ERROR ("Plugin_Manager::~Plugin_Manager - " <<
                              "error while processing Service Config"
                              "directive: " << remove);
        }
      }
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("Plugin_Manager::~Plugin_Manager - " <<
                          "Caught unknown C++ exception while closing plugin manager");
    }
  }

  void
  Plugin_Manager::set_configuration (const Deployment::Properties &config)
  {
    this->config_ = config;
  }

  void
  Plugin_Manager::add_configuration (const Deployment::Properties &config)
  {
    if (this->config_.empty ())
    {
      // just copy the provided props
      this->config_ = config;
    }
    else
    {
      // merge the existing props with the provided props (overwriting duplicates)
      Utility::PROPERTY_MAP cfg_map;
      Utility::build_property_map (cfg_map, this->config_);
      Utility::build_property_map (cfg_map, config, true);
      this->config_.clear ();
      // rebuild the property sequence
      Utility::build_property_sequence (this->config_, cfg_map);
    }
  }

  std::string
  Plugin_Manager::register_installation_handler (
      const std::string& artifact,
      const std::string& entrypoint,
      const Plugin_Manager::IH_DEPS &depends,
      uint32_t open_mode,
      Utility::PROPERTY_MAP& config)
  {
    IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type plugin =
      load_plugin<DAnCEX11::InstanceDeploymentHandler> (artifact,
                                                     entrypoint,
                                                     open_mode);

    if (!plugin)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_installation_handler - " <<
                            "Failed to create installation handler for artifact <" <<
                            artifact << ">");
      }

    std::string instance_type = plugin->instance_type ();

    DANCEX11_LOG_DEBUG ("Plugin_Manager::register_installation_handler - " <<
                        "Successfully created installation handler for instance type <" <<
                        instance_type << ">");

    try
      {
        // add manager properties if not yet defined
        Utility::build_property_map (config, this->config_, false);

        // configure plugin
        Deployment::Properties plugin_props;
        Utility::build_property_sequence (plugin_props, config);
        plugin->configure (plugin_props);

        this->ih_dep_.add_dependency (instance_type, depends);

        for (const std::string &dep : plugin->dependencies ())
          {
            this->ih_dep_.add_dependency (instance_type, dep);
          }

        this->handler_map_.insert (HANDLER_MAP_PAIR (instance_type, plugin));

        return instance_type;
      }
    catch (const CORBA::Exception &ex)
      {
        std::ostringstream err;
        err << ex;
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_installation_handler - " <<
                            "CORBA Exception caught while loading artifact <" << artifact <<
                            ">:<" << entrypoint << "> - " << err.str ());
        throw ::Deployment::PlanError (artifact,
                                       err.str ());
      }
    catch (const std::exception& ex)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_installation_handler - "
                            "STD C++ exception while configuring plugin from <" << artifact <<
                            ">:<" << entrypoint << "> -" << ex.what ());
        throw ::Deployment::PlanError (artifact,
                                       ex.what ());
      }
    catch (...)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_installation_handler - "
                            "Unknown C++ exception while configuring plugin from <" << artifact <<
                            ">:<" << entrypoint << ">");
        throw ::Deployment::PlanError (artifact,
                                       "Unknown C++ exception during handler configuration");
      }
  }

  void
  Plugin_Manager::get_installation_order (Plugin_Manager::INSTALL_ORDER &io)
  {
    this->ih_dep_.calculate_order (io);
  }

  void
  Plugin_Manager::register_interceptor (
      const std::string& artifact,
      const std::string& entrypoint,
      uint32_t open_mode,
      Utility::PROPERTY_MAP& config)
  {
    IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type plugin =
      load_plugin<DAnCEX11::DeploymentInterceptor > (artifact,
                                                  entrypoint,
                                                  open_mode);
    try
      {
        // add manager properties if not yet defined
        Utility::build_property_map (config, this->config_, false);

        // configure plugin
        Deployment::Properties plugin_props;
        Utility::build_property_sequence (plugin_props, config);
        plugin->configure (plugin_props);

        DANCEX11_LOG_DEBUG ("Plugin_Manager::register_interceptor - " <<
                            "Successfully created deployment interceptor");

        this->interceptors_.push_back (plugin);
      }
    catch (const CORBA::Exception &ex)
      {
        std::ostringstream err;
        err << ex;
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_interceptor - " <<
                            "CORBA Exception caught while loading artifact <" << artifact << ">:<" <<
                            entrypoint << "> - " << err.str ());
        throw ::Deployment::PlanError (artifact,
                                       err.str ());
      }
    catch (...)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_interceptor - " <<
                            "Unknown C++ exception while configuring plugin from <" << artifact << ">:<" <<
                            entrypoint << ">");
        throw ::Deployment::PlanError (artifact,
                                      "Unknown C++ exception during handler configuration\n");
      }
  }

  IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type
  Plugin_Manager::fetch_installation_handler (const std::string &instance_type)
  {
    HANDLER_MAP::iterator i = this->handler_map_.find (instance_type);

    if (i == this->handler_map_.end ())
      {
        DANCEX11_LOG_INFO ("Plugin_Manager::fetch_installation_handler - " <<
                           "No installation handler for type <" << instance_type << "> found");

        return nullptr;
      }

    return i->second;
  }

  const
  Plugin_Manager::INTERCEPTORS &
  Plugin_Manager::fetch_interceptors ()
  {
    return this->interceptors_;
  }

  void
  Plugin_Manager::register_configuration_plugin (
      const std::string& artifact,
      const std::string& entrypoint,
      uint32_t open_mode)
  {
    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type plugin =
      load_plugin<DAnCEX11::DeploymentConfiguration > (artifact,
                                                  entrypoint,
                                                  open_mode);

    try
      {
        this->config_plugins_.insert (CONFIG_MAP_PAIR (plugin->type (), plugin));
      }
    catch (const CORBA::Exception &ex)
      {
        std::ostringstream err;
        err << ex;
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_configuration_plugin - " <<
                            "CORBA Exception caught while loading artifact <" << artifact << ">:<" << entrypoint <<
                            "> - " << err.str ());
        throw ::Deployment::PlanError (artifact,
                                        err.str ());
      }
    catch (...)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_configuration_plugin - " <<
                            "Unknown C++ exception while configuring plugin from <" << artifact << ">:<" <<
                            entrypoint << ">");
        throw ::Deployment::PlanError (artifact,
                                      "Unknown C++ exception during plugin configuration\n");
      }
  }

  IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type
  Plugin_Manager::get_configuration_handler (const std::string& id)
  {
    CONFIG_MAP::iterator i = this->config_plugins_.find (id);

    if (i == this->config_plugins_.end ())
      {
        DANCEX11_LOG_DEBUG ("Plugin_Manager::get_configuration_handler - " <<
                            "No configuration plugin for type <" << id << "> found");

        return nullptr;
      }

    return i->second;
  }

  void
  Plugin_Manager::register_service (const std::string& artifact,
                                    const std::string& entrypoint,
                                    const std::string& arguments,
                                    bool ignore_load_error)
  {
    // create an ACE Service Config directive for the service
    std::ostringstream os;
    os << "dynamic " << artifact << "_" << entrypoint << " Service_Object * " <<
          artifact << ":" << entrypoint <<
          "() \"" << arguments << "\"";
    std::string svcfg_txt = os.str ();

    DANCEX11_LOG_DEBUG ("Service_Object_Handler_Impl::install_instance - "
                        "Processing Service Config directive :\n\t" <<
                        svcfg_txt);

    if (ACE_Service_Config::process_directive (svcfg_txt.c_str ()) != 0)
    {
      if (!ignore_load_error)
      {
        DANCEX11_LOG_ERROR ("Plugin_Manager::register_service - "
                            "Error(s) while processing "
                            "Service Config directive :\n\t" <<
                            svcfg_txt);
        throw ::Deployment::PlanError (artifact,
          "Error processing Service Config directive [" +
          svcfg_txt +
          "] for ServiceObject plugin\n");
      }
      else
      {
        DANCEX11_LOG_DEBUG ("Plugin_Manager::register_service - "
                            "Ignored error(s) while processing "
                            "Service Config directive :\n\t" <<
                            svcfg_txt);
      }
    }
    else
    {
      this->service_objects_.push_back (artifact + "_" + entrypoint);
    }
  }

}
