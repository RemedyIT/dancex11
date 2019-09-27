/**
 * @file    launcher_impl.cpp
 * @author  Martin Corino
 *
 * @brief   Plan launcher implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "launcher_impl.h"
#include "dancex11/logger/log.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deployment_executionmanagerC.h"
#include "dancex11/deployment/deployment_domainapplicationmanagerC.h"
#include "dancex11/deployment/deployment_nodeapplicationmanagerC.h"
#include "dancex11/core/dancex11_deployment_plan_loader.h"
#include "dancex11/core/dancex11_deployment_state.h"

#include <sstream>

namespace DAnCEX11
{
  bool Launcher_Base::teardown_plan (IDL::traits<Deployment::Deployment_Manager>::ref_type dm)
  {
    DANCEX11_LOG_TRACE ("Launcher_Base::teardown_plan");

    DANCEX11_LOG_DEBUG ("Launcher_Base::teardown_plan - " <<
                        "tearing down plan: <" <<
                        this->options_.plan_uuid_ << ">");

    // first resolve application manager
    IDL::traits<Deployment::ApplicationManager>::ref_type am =
        this->resolve_app_manager (dm);

    if (am)
    {
      // now resolve application to destroy
      IDL::traits<Deployment::Application>::ref_type app =
          this->resolve_application (am);

      if (app)
      {
        // now tear it all down

        DANCEX11_LOG_DEBUG ("Launcher_Base::teardown_plan - " <<
                            "stopping application");

        bool stopped {};
        try
        {
          app->stop ();
          stopped = true;
        }
        catch (const Deployment::StopError &ex)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                              "caught StopError stopping application: " <<
                              ex);
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                              "caught CORBA exception "
                              "stopping application: " <<
                              ex);
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                              "caught unexpected C++ exception "
                              "stopping application");
        }

        DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                            "destroying application");

        bool destroyed {};
        if (stopped || this->options_.force_)
        {
          try
          {
            am->destroyApplication (app);
            destroyed = true;
          }
          catch (const Deployment::StopError &ex)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                                "caught StopError destroying application: " <<
                                ex);
          }
          catch (const CORBA::Exception& ex)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                                "caught CORBA exception "
                                "destroying application: " <<
                                ex);
          }
          catch (...)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                                "caught unexpected C++ exception "
                                "destroying application");
          }
        }

        if (destroyed || this->options_.force_)
        {
          try
          {
            dm->destroyManager (am);

            return true;
          }
          catch (const Deployment::StopError &ex)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                                "caught StopError destroying "
                                "application manager: " <<
                                ex);
          }
          catch (const CORBA::Exception& ex)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                                "caught CORBA exception "
                                "destroying application manager: " <<
                                ex);
          }
          catch (...)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                                "caught unexpected C++ exception "
                                "destroying application manager");
          }
        }
      }
      else
      {
        DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                            "unable to resolve application");
      }
    }
    else
    {
      DANCEX11_LOG_ERROR ("Launcher_Base::teardown_plan - " <<
                          "unable to resolve application manager");
    }

    return false;
  }

  bool
  Launcher_Base::launch_plan (
      IDL::traits<Deployment::Deployment_Manager>::ref_type dm)
  {
    DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                        "launching plan...");

    if (this->options_.plan_file_.empty ())
    {
      DANCEX11_LOG_PANIC ("Launcher_Base::launch_plan - "
                          "missing PLAN file to load.");
      return false;
    }

    // load plan
    DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                        "reading plan from " <<
                        this->options_.plan_file_);

    // read plan from file
    Deployment::DeploymentPlan plan;
    if (!DAnCEX11::Plan_Loader::instance ()->read_plan (
        this->options_.plan_file_,
        plan,
        this->options_.plan_fmt_))
    {
      DANCEX11_LOG_PANIC ("Launcher_Base::launch_plan - " <<
                          "failed to read plan from " <<
                          this->options_.plan_file_);
      return false;
    }

    DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                        "manager preparing plan [" <<
                        plan.UUID () << "]");

    // have the manager prepare the plan
    IDL::traits<Deployment::ApplicationManager>::ref_type am =
        dm->preparePlan (plan, nullptr);

    DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                        "application manager starting plan [" <<
                        plan.UUID () << "] launch");

    IDL::traits<Deployment::Application>::ref_type app;
    try
    {
      // have the application manager start the plan launching
      ::Deployment::Properties cfgProps;
      ::Deployment::Connections providedRefs;
      app = am->startLaunch (cfgProps, providedRefs);

      DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                          "application finishing plan [" <<
                          plan.UUID () << "] launch");

      // have the node application finish the launch
      app->finishLaunch (providedRefs, false);

      DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                          "application starting plan [" <<
                          plan.UUID () << "]");

      // have the node application start the application
      app->start ();

      DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                          "application started plan [" <<
                          plan.UUID () << "]");

      // when requested or not launching on an EM
      // write the IOR of the referenced application (manager)
      // to file
      if (!this->options_.am_ior_.empty () ||
            this->svc_objectkind_ != "DomainManager")
      {
        std::string ior_file = this->options_.am_ior_;
        if (ior_file.empty ())
        {
          ior_file = "appmgr-" + plan.UUID () + ".ior";
        }

        std::string ior = DAnCEX11::State::instance ()->orb ()->object_to_string (am);

        DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                            "Writing application manager IOR " <<
                            ior << " to file " <<  ior_file);
        if (!DAnCEX11::PlanLauncher::write_IOR (ior_file, ior))
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::launch_plan - " <<
                              "Error: Unable to write IOR to file " <<
                              ior_file);
        }
      }
      if (!this->options_.app_ior_.empty () ||
            this->svc_objectkind_ != "DomainManager")
      {
        std::string ior_file = this->options_.app_ior_;
        if (ior_file.empty ())
        {
          ior_file = "app-" + plan.UUID () + ".ior";
        }

        std::string ior = DAnCEX11::State::instance ()->orb ()->object_to_string (app);

        DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                            "Writing application IOR " <<
                            ior << " to file " <<  ior_file);
        if (!DAnCEX11::PlanLauncher::write_IOR (ior_file, ior))
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::launch_plan - " <<
                              "Error: Unable to write IOR to file " <<
                              ior_file);
        }
      }
    }
    catch (const CORBA::UserException& ex)
    {
      // attempt controlled shutdown
      DANCEX11_LOG_PANIC ("Launcher_Base::launch_plan - " <<
                          "Caught deployment exception while starting plan [" <<
                          plan.UUID () << "]: " << ex);
      try
      {
        if (app)
        {
          DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                              "stopping application...");

          app->stop ();

          DANCEX11_LOG_DEBUG ("Node_Launcher::launch - "
                              "destroying application...");

          am->destroyApplication (app);
        }

        DANCEX11_LOG_DEBUG ("Launcher_Base::launch_plan - " <<
                            "destroying application manager...");

        dm->destroyManager (am);

        dm.reset ();
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("Node_Launcher::launch - "
                            "Caught exception while attempting "
                            "controlled shutdown of failed plan launch.");
      }

      return false;
    }

    return true;
  }

  IDL::traits<Deployment::ApplicationManager>::ref_type
  Launcher_Base::resolve_app_manager (
      IDL::traits<Deployment::Deployment_Manager>::ref_type)
  {
    DANCEX11_LOG_TRACE ("Launcher_Base::resolve_app_manager");
    // resolve am-ior if provided
    IDL::traits<Deployment::ApplicationManager>::ref_type am;
    if (!this->options_.am_ior_.empty ())
    {
      try
      {
        IDL::traits<CORBA::Object>::ref_type am_obj =
            DAnCEX11::State::instance ()->orb ()->string_to_object (
                this->options_.am_ior_);

        am = IDL::traits<Deployment::ApplicationManager>::narrow (am_obj);
        if (!am)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::resolve_app_manager - " <<
                              "Narrowing application manager returned nil.");
        }
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("Launcher_Base::resolve_app_manager - " <<
                            "Exception resolving application manager: " <<
                            ex);
      }
    }
    if (!am && this->svc_objectkind_ != "DomainManager")
    {
      // see if we can find an IOR file with default naming
      std::string const am_file = "appmgr-" + this->options_.plan_uuid_ + ".ior";

      DANCEX11_LOG_DEBUG ("Launcher_Base::resolve_app_manager - " <<
                          "looking for IOR file: <" << am_file << ">");

      std::string const ior = PlanLauncher::read_IOR (am_file);
      if (!ior.empty ())
      {
        DANCEX11_LOG_DEBUG ("Launcher_Base::resolve_app_manager - " <<
                            "resolving IOR: " << ior);
        try
        {
          IDL::traits<CORBA::Object>::ref_type am_obj =
              DAnCEX11::State::instance ()->orb ()->string_to_object (
                  ior);

          am = IDL::traits<Deployment::ApplicationManager>::narrow (am_obj);
          if (!am)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::resolve_app_manager - " <<
                                "Narrowing application manager returned nil.");
          }
        }
        catch (const CORBA::Exception &ex)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::resolve_app_manager - " <<
                              "Exception resolving application manager: " <<
                              ex);
        }
      }
    }
    return am;
  }

  IDL::traits<Deployment::Application>::ref_type
  Launcher_Base::resolve_application (
      IDL::traits<Deployment::ApplicationManager>::ref_type)
  {
    DANCEX11_LOG_TRACE ("Launcher_Base::resolve_application");
    // resolve app-ior if provided
    IDL::traits<Deployment::Application>::ref_type app;
    if (!this->options_.app_ior_.empty ())
    {
      try
      {
        IDL::traits<CORBA::Object>::ref_type app_obj =
            DAnCEX11::State::instance ()->orb ()->string_to_object (
                this->options_.app_ior_);

        app = IDL::traits<Deployment::Application>::narrow (app_obj);
        if (!app)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::resolve_application - " <<
                              "Narrowing application returned nil.");
        }
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("Launcher_Base::resolve_application - " <<
                            "Exception resolving application: " <<
                            ex);
      }
    }
    if (!app && this->svc_objectkind_ != "DomainManager")
    {
      // see if we can find an IOR file with default naming
      std::string const app_file = "app-" + this->options_.plan_uuid_ + ".ior";

      DANCEX11_LOG_DEBUG ("Launcher_Base::resolve_application - " <<
                          "looking for IOR file: <" << app_file << ">");

      std::string const ior = PlanLauncher::read_IOR (app_file);
      if (!ior.empty ())
      {
        DANCEX11_LOG_DEBUG ("Launcher_Base::resolve_application - " <<
                            "resolving IOR: " << ior);
        try
        {
          IDL::traits<CORBA::Object>::ref_type app_obj =
              DAnCEX11::State::instance ()->orb ()->string_to_object (
                  ior);

          app = IDL::traits<Deployment::Application>::narrow (app_obj);
          if (!app)
          {
            DANCEX11_LOG_ERROR ("Launcher_Base::resolve_application - " <<
                                "Narrowing application returned nil.");
          }
        }
        catch (const CORBA::Exception &ex)
        {
          DANCEX11_LOG_ERROR ("Launcher_Base::resolve_application - " <<
                              "Exception resolving application: " <<
                              ex);
        }
      }
    }
    return app;
  }

  IDL::traits<Deployment::ApplicationManager>::ref_type
  EM_Launcher::resolve_app_manager (
      IDL::traits<Deployment::Deployment_Manager>::ref_type dm)
  {
    DANCEX11_LOG_TRACE ("EM_Launcher::resolve_app_manager");

    // first check for explicitly specified reference
    IDL::traits<Deployment::ApplicationManager>::ref_type am =
        this->Launcher_Base::resolve_app_manager (dm);

    if (!am)
    {
      DANCEX11_LOG_DEBUG ("EM_Launcher::resolve_app_manager - " <<
                          "Looking up domain application manager by UUID.");

      // lookup AM using EM interface
      IDL::traits<Deployment::ExecutionManager>::ref_type em =
          IDL::traits<Deployment::ExecutionManager>::narrow (dm);

      if (em)
      {
        Deployment::DomainApplicationManagers const dams =
            em->getManagers ();
        for (IDL::traits<Deployment::DomainApplicationManager>::ref_type dam : dams)
        {
          if (this->options_.plan_uuid_ == dam->getPlan ().UUID ())
          {
            am = dam;
          }
        }

        if (!am)
        {
          DANCEX11_LOG_ERROR ("EM_Launcher::resolve_app_manager - " <<
                              "unable to find domain application "
                              "manager for plan:" <<
                              this->options_.plan_uuid_);
        }
      }
      else
      {
        DANCEX11_LOG_ERROR ("EM_Launcher::resolve_app_manager - " <<
                            "Narrowing execution manager returned nil.");
      }
    }

    return am;
  }

  IDL::traits<Deployment::Application>::ref_type
  EM_Launcher::resolve_application (
      IDL::traits<Deployment::ApplicationManager>::ref_type am)
  {
    DANCEX11_LOG_TRACE ("EM_Launcher::resolve_application");

    // first check for explicitly specified reference
    IDL::traits<Deployment::Application>::ref_type app =
        this->Launcher_Base::resolve_application (am);

    if (!app)
    {
      DANCEX11_LOG_DEBUG ("EM_Launcher::resolve_application - " <<
                          "Looking up domain application from DAM.");

      // lookup APP using DAM interface
      IDL::traits<Deployment::DomainApplicationManager>::ref_type dam =
          IDL::traits<Deployment::DomainApplicationManager>::narrow (am);

      if (dam)
      {
        Deployment::Applications apps =
            dam->getApplications ();
        app = apps.front ();

        if (!app)
        {
          DANCEX11_LOG_ERROR ("EM_Launcher::resolve_application - " <<
                              "unable to find domain application "
                              "for plan:" <<
                              this->options_.plan_uuid_);
        }
      }
      else
      {
        DANCEX11_LOG_ERROR ("EM_Launcher::resolve_application - " <<
                            "Narrowing domain application manager "
                            "returned nil.");
      }
    }

    return app;
  }
}
