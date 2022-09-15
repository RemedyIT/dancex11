// -*- C++ -*-
/**
 * @file   plan_launcher_module.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_PLAN_LAUNCHER_MODULE_H
#define DANCEX11_PLAN_LAUNCHER_MODULE_H

#pragma once

#include "dancex11/deployment/deployment_managerC.h"
#include "dancex11/scheduler/events/plugin_manager.h"

#include <string>
#include <map>

namespace DAnCEX11
{
  /**
   * @class PlanLauncher_Module
   *
   * @brief PlanLauncher main module.
   *
   * This class runs the node manager instance
   */
  class PlanLauncher_Module
  {
   public:
    /// Constructor.
    PlanLauncher_Module ();

    /// Destructor.
    ~PlanLauncher_Module ();

    /// Initialize a PlanLauncher_Module object.
    bool init (int argc, char *argv []);

    bool run ();

    struct SOptions
    {
      std::vector<std::string> other_opts_;
      std::string dm_ior_;
      std::string dm_name_;
      uint16_t    dm_port_ {};
      std::string dm_addr_;
      uint32_t    dm_timeout_ {};
      std::string deployment_nc_;
      std::string pl_config_;
      std::string plan_file_;
      std::string plan_fmt_;
      std::string plan_uuid_;
      std::string am_ior_;
      std::string app_ior_;
      bool teardown_ {};
      bool force_ {};
      SOptions();

      static const char* orb_listen_opt_;
      static uint16_t orb_listen_port_;
      static const char* svcname_prefix_;
      static const std::vector<std::string> svc_kinds;
    };

   private:
    const char * usage ();

    bool parse_args (int argc, char *argv []);

    bool resolve_manager (IDL::traits<CORBA::ORB>::ref_type orb);

    bool configure ();

    void
    create_orb_args (std::vector<std::string> &orb_args,
                     const char* argv0);

    std::string orb_endpoint_;
    std::string svc_objectid_;
    std::string svc_objectkind_;
    // activated deployment manager reference
    IDL::traits<Deployment::Deployment_Manager>::ref_type dm_;

    SOptions options_;

    Plugin_Manager plugins_;
  };

  namespace PlanLauncher
  {
    bool
    write_IOR (const std::string& ior_file_name, const std::string& ior);
    std::string
    read_IOR (const std::string& ior_file_name);
  };
}

#endif  /* DANCEX11_PLAN_LAUNCHER_MODULE_H */
