// -*- C++ -*-
/**
 * @file dancex11_deployment_plan_loader.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DEPLOYMENT_PLAN_LOADER_H
#define DANCEX11_DEPLOYMENT_PLAN_LOADER_H

#pragma once

#include <mutex>

#include "dancex11_plan_loader_stub_export.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_targetdataC.h"


#include <map>

namespace DAnCEX11
{
  class DANCEX11_PLAN_LOADER_STUB_Export Plan_Loader_base
  {
  public:
    virtual ~Plan_Loader_base () = default;

    virtual bool read_plan (const std::string& filename,
                            Deployment::DeploymentPlan& plan) = 0;

    virtual bool read_domain (const std::string& filename,
                              Deployment::Domain& dom) = 0;

  protected:
    Plan_Loader_base () = default;
  };

  class DANCEX11_PLAN_LOADER_STUB_Export Plan_Loader
  {
  public:
    bool read_plan (const std::string& filename,
                    Deployment::DeploymentPlan& plan,
                    std::string format = {});

    bool read_domain (const std::string& filename,
                      Deployment::Domain& dom,
                      std::string format = {});

    static bool instance_exists ()
    { return instance_exists_; }

    static Plan_Loader* instance ();

    static inline void register_plan_loader (std::string fmt, Plan_Loader_base* pl);
    static inline void deregister_plan_loader (std::string fmt);

    void register_plan_loader_i (std::string, Plan_Loader_base*);
    void deregister_plan_loader_i (std::string);

  private:
    Plan_Loader ();
    ~Plan_Loader ();

    friend class Plan_Loader_base;

    std::string extension (const std::string& path);

    typedef std::map<std::string, Plan_Loader_base*> LoaderMap;
    LoaderMap loaders_;
    std::mutex lock_;

    static bool instance_exists_;
  };

  inline void Plan_Loader::register_plan_loader (std::string fmt, Plan_Loader_base* pl)
  {
    instance ()->register_plan_loader_i (fmt, pl);
  }

  inline void Plan_Loader::deregister_plan_loader (std::string fmt)
  {
    if (instance_exists ()) instance ()->deregister_plan_loader_i (fmt);
  }
};

#endif /* DANCEX11_DEPLOYMENT_PLAN_LOADER_H */
