// -*- C++ -*-
/**
 * @file dancex11_deployment_plan_loader.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "tao/CDR.h"

#include "tao/x11/tao_corba.h"

#include "dancex11_deployment_plan_loader.h"
#include "dancex11_deployment_config_plan_loader.h"
#include "dancex11/logger/log.h"

#include <fstream>

namespace DAnCEX11
{
  bool Plan_Loader::instance_exists_ {false};


  Plan_Loader* Plan_Loader::instance ()
  {
    static Plan_Loader instance_;

    return &instance_;
  }

  Plan_Loader::Plan_Loader ()
  {
    // register CONFIG plan loader by default
    CFGPlan_Loader::instance ()->init (*this);
    instance_exists_ = true;
  }

  Plan_Loader::~Plan_Loader ()
  {
    // mark this instance as "gone"
    instance_exists_ = false;
  }

  std::string
  Plan_Loader::extension (const std::string& path)
  {
    std::string::size_type const n = path.rfind ('.');
    return (n == std::string::npos) ? "" : path.substr (n+1);
  }

  bool
  Plan_Loader::read_plan (
      const std::string& filename,
      Deployment::DeploymentPlan& plan,
      std::string format)
  {
    DANCEX11_LOG_TRACE ("Plan_Loader::read_plan - file: [" <<
                        filename << "], format: [" << format << "]");

    Plan_Loader_base* pl {};
    if (format.empty ())
    {
      format = this->extension (filename);
    }

    DANCEX11_LOG_DEBUG ("Plan_Loader::read_plan - determined format: [" <<
      format << "]");

    // lock map while looking up plan loader
    {
      std::lock_guard<std::mutex> __guard (this->lock_);

      LoaderMap::iterator it =
          this->loaders_.find (format);
      if (it != this->loaders_.end ())
      {
        pl = it->second;
      }
    }

    if (pl)
    {
      return pl->read_plan (filename, plan);
    }
    else
    {
      DANCEX11_LOG_ERROR ("Plan_Loader::read_plan - "
                          "No Plan Loader for format: " << format);
    }

    return false;
  }

  bool
  Plan_Loader::read_domain (
      const std::string& filename,
      Deployment::Domain& dom,
      std::string format)
  {
    DANCEX11_LOG_TRACE ("Plan_Loader::read_domain - file: [" <<
                        filename << "], format: [" << format << "]");

    Plan_Loader_base* pl {};
    if (format.empty ())
    {
      format = this->extension (filename);
    }

    // lock map while looking up plan loader
    {
      std::lock_guard<std::mutex> __guard (this->lock_);

      LoaderMap::iterator it =
          this->loaders_.find (format);
      if (it != this->loaders_.end ())
      {
        pl = it->second;
      }
    }

    if (pl)
    {
      return pl->read_domain (filename, dom);
    }
    else
    {
      DANCEX11_LOG_ERROR ("Plan_Loader::read_domain - "
                          "No Plan Loader for format: " << format);
    }

    return false;
  }

  void
  Plan_Loader::register_plan_loader_i (std::string fmt, Plan_Loader_base* pl)
  {
    DANCEX11_LOG_TRACE ("Plan_Loader::register_plan_loader_i@" << this << " - "
                        "format: " << fmt);

    std::lock_guard<std::mutex> __guard (this->lock_);

    LoaderMap::iterator it = this->loaders_.find (fmt);
    if (it == this->loaders_.end ())
    {
      this->loaders_[fmt] = pl;
    }
    else if (it->second != pl)
    {
      DANCEX11_LOG_ERROR ("Plan_Loader::register_plan_loader - "
                          "Plan Loader already registered for format: " << fmt);
    }
  }

  void Plan_Loader::deregister_plan_loader_i (std::string fmt)
  {
    DANCEX11_LOG_TRACE ("Plan_Loader::deregister_plan_loader_i - "
                        "format: " << fmt);

    std::lock_guard<std::mutex> __guard (this->lock_);

    LoaderMap::iterator it = this->loaders_.find (fmt);
    if (it != this->loaders_.end ())
    {
      this->loaders_.erase (it);
    }
  }
};
