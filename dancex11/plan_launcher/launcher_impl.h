/**
 * @file    launcher_impl.h
 * @author  Martin Corino
 *
 * @brief   Plan launcher implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DAnCEX11_LAUNCHER_IMPL_H_
#define DAnCEX11_LAUNCHER_IMPL_H_

#pragma once

#include "plan_launcher_module.h"

namespace DAnCEX11
{
  class Launcher_Base
  {
  public:
    Launcher_Base (
        const PlanLauncher_Module::SOptions &options,
        const std::string& svc_objectkind)
      : options_ (options),
        svc_objectkind_ (svc_objectkind)
    {}
    virtual ~Launcher_Base () = default;

    bool
    launch_plan (IDL::traits<Deployment::Deployment_Manager>::ref_type);

    bool
    teardown_plan (IDL::traits<Deployment::Deployment_Manager>::ref_type);

  protected:
    virtual IDL::traits<Deployment::ApplicationManager>::ref_type
    resolve_app_manager (
        IDL::traits<Deployment::Deployment_Manager>::ref_type);

    virtual IDL::traits<Deployment::Application>::ref_type
    resolve_application (
        IDL::traits<Deployment::ApplicationManager>::ref_type);

    const PlanLauncher_Module::SOptions &options_;
    const std::string& svc_objectkind_;
  };

  class EM_Launcher : public Launcher_Base
  {
  public:
    EM_Launcher (
        const PlanLauncher_Module::SOptions &options,
        const std::string& svc_objectkind)
      : Launcher_Base (options, svc_objectkind)
    {}

  protected:
    IDL::traits<Deployment::ApplicationManager>::ref_type
    resolve_app_manager (
        IDL::traits<Deployment::Deployment_Manager>::ref_type) override;

    IDL::traits<Deployment::Application>::ref_type
    resolve_application (
        IDL::traits<Deployment::ApplicationManager>::ref_type) override;
  };

  class NM_Launcher : public Launcher_Base
  {
  public:
    NM_Launcher (
        const PlanLauncher_Module::SOptions &options,
        const std::string& svc_objectkind)
      : Launcher_Base (options, svc_objectkind)
    {}
  };

  class LM_Launcher : public Launcher_Base
  {
  public:
    LM_Launcher (
        const PlanLauncher_Module::SOptions &options,
        const std::string& svc_objectkind)
      : Launcher_Base (options, svc_objectkind)
    {}
  };
}

#endif /* DAnCEX11_LAUNCHER_IMPL_H_ */
