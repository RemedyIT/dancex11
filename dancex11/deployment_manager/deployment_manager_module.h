// -*- C++ -*-
/**
 * @file deployment_manager_module.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DEPLOYMENT_MANAGER_MODULE_H
#define DANCEX11_DEPLOYMENT_MANAGER_MODULE_H

#pragma once

#include "dancex11/core/dancex11_deploymentmanagerhandlerC.h"
#include "dancex11/core/dancex11_shutdown_controlC.h"

#include <string>
#include <map>

namespace DAnCEX11
{
  /**
   * @class DeploymentManager_Module
   *
   * @brief DeploymentManager main module.
   *
   * This class runs the node manager instance
   */
  class DeploymentManager_Module
    : public IDL::traits<ShutdownHandler>::base_type
  {
   public:
    /// Constructor.
    DeploymentManager_Module ();

    /// Destructor.
    ~DeploymentManager_Module ();

    /// Initialize a DeploymentManager_Module object.
    bool init (int argc, char *argv []);

    bool run ();

    /// ShutdownHandler callback
    void shutdown () override;

   private:
    const char * usage ();

    bool parse_args (int argc, char *argv []);

    struct SOptions
    {
      std::vector<std::string> other_opts_;
      std::string dm_handler_lib_;
      std::string dm_handler_arg_;
      std::string dm_name_;
      std::string dm_kind_;
      std::string deployment_nc_;
      bool dm_nc_registration_ {true};
      unsigned int threads_ {};
      std::string dm_config_;
      Deployment::Properties dm_props_ {};
      bool launcher_ {};
      std::string plan_file_;
      std::string plan_fmt_;
      uint16_t listen_port_ {};
      std::string listen_addr_;
      bool numeric_addresses_ {};
      bool teardown_ {};
      SOptions();

      static const char* orb_endpt_opt_;
      static const char* orb_listen_opt_;
      static const char* orb_dotted_dec_opt_;
      static const char* svcname_prefix_;
      static const char* svcobj_name_;
      static const char* svcobj_factory_;
    };

    bool
    init_teardown (const char*,
                   const std::string&,
                   const std::string&);
    bool
    init_startup (const char*,
                  const std::string&,
                  const std::string&);

    std::unique_ptr<const char*[]>
    create_orb_args (int &argc,
                     const char* argv0);

    void
    load_handler ();

    void
    remove_handler ();

    IDL::traits<DAnCEX11::DeploymentManagerHandler>::ref_type handler_;

    std::string orb_endpoint_;
    std::string svc_objectid_;
    std::string svc_objectkind_;
    // activated deployment manager reference
    IDL::traits<CORBA::Object>::ref_type dm_;

    SOptions options_;
  };

}

#endif  /* DANCEX11_DEPLOYMENT_MANAGER_MODULE_H */
