/**
 * @file    action_base.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "action_base.h"

#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
{
  Action_Base::Action_Base (
      Plugin_Manager& plugins,
      Event_Future holder,
      const std::string &name,
      const std::string &instance_type)
    : Deployment_Event (holder, name, instance_type),
      plugins_ (plugins)
  {
  }

  int
  Action_Base::call ()
  {
    DANCEX11_LOG_DEBUG ("Action_Base::call - Entering Action_Base");

    try
      {
        Plugin_Manager::INTERCEPTORS interceptors =
          this->plugins_.fetch_interceptors ();

        DANCEX11_LOG_DEBUG ("Action_Base::call - Invoking pre-install interceptors");
        for (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di : interceptors)
          {
            this->invoke_pre_interceptor (di);
          }

        IDL::traits<DAnCEX11::InstanceDeploymentHandler>::ref_type handler =
          this->plugins_.fetch_installation_handler (this->instance_type_);

        if (!handler)
          {
            this->create_unexpected_exception (this->name_,
                                               "Unable to load instance handler " + this->instance_type_);
          }
        else
          {
            try
              {
                this->invoke (handler);
              }
            catch (const Deployment::PlanError &ex)
              {
                DANCEX11_LOG_ERROR ("Action_Base::call - " <<
                                    "Caught Deployment::PlanError <" << ex <<
                                    "> while processing instance <" <<
                                    this->name_ << ">");
                this->instance_excep_ = DAnCEX11::Utility::create_any_from_user_exception (ex);
              }
            catch (const CORBA::UserException &ex)
              {
                DANCEX11_LOG_ERROR ("Action_Base::call - "
                                    "Caught CORBA UserException <" << ex <<
                                    "> while processing instance <" <<
                                    this->name_ << ">");
                this->instance_excep_ = DAnCEX11::Utility::create_any_from_user_exception (ex);
              }
            catch (const CORBA::SystemException &ex)
              {
                DANCEX11_LOG_ERROR ("Action_Base::call - "
                                    "Caught CORBA SystemException <" << ex <<
                                    "> while processing instance <" <<
                                    this->name_ << ">");
                this->instance_excep_ = DAnCEX11::Utility::create_any_from_exception (ex);
              }
            catch (...)
              {
                DANCEX11_LOG_ERROR ("Action_Base::call - " <<
                                    "Caught C++ exception while processing instance <" <<
                                    this->name_ << ">");

                this->create_unexpected_exception (this->name_,
                                                  "Caught unknown C++ exception from install");
              }
          }

        Event_Result result (this->name_, this->instance_excep_.type ()->kind () != CORBA::TCKind::tk_null);
        if (!interceptors.empty ())
          {
            DANCEX11_LOG_DEBUG ("Action_Base::call - Invoking post-action interceptors");
            for (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di : interceptors)
              {
                this->invoke_post_interceptor (di);
              }

            this->create_valid_result (result);
            result.exception_ = false;
          }
        else
          {
            DANCEX11_LOG_DEBUG ("Action_Base::call - No post-install interceptors; directly propagating result");
            if (result.exception_)
              {
                result.contents_ = this->instance_excep_;
              }
            else
              {
                this->create_valid_result (result);
              }
          }

        DANCEX11_LOG_DEBUG ("Action_Base::call - Signaling result for instance <" <<
                            this->name_ << ">");
        this->holder_.set (result);
      }
    catch (const CORBA::UserException &ex)
      {
        DANCEX11_LOG_ERROR ("Action_Base::call - " <<
                            "CORBA UserException <" << ex <<
                            "> propagated from interceptors for instance <" <<
                            this->name_ << ">");

        Event_Result result (this->name_, true);

        try
          {
            result.contents_ = DAnCEX11::Utility::create_any_from_user_exception (ex);
          }
        catch (...)
          {
            DANCEX11_LOG_ERROR ("Action_Base::call - " <<
                                "Unexpected exception whilst creating any from user exception for <" <<
                                this->name_ << ">");
          }

        this->holder_.set (result);
        return -1;
      }
    catch (const CORBA::SystemException &ex)
      {
        DANCEX11_LOG_ERROR ("Action_Base::call - " <<
                            "CORBA SystemException <" << ex << "> propagated from interceptors for instance <" <<
                            this->name_ << ">");

        Event_Result result (this->name_, true);

        try
          {
            result.contents_ = DAnCEX11::Utility::create_any_from_exception (ex);
          }
        catch (...)
          {
            DANCEX11_LOG_ERROR ("Action_Base::call - " <<
                                "Unexpected exception whilst creating any from a system exception for <" <<
                                this->name_ << ">");
          }

        this->holder_.set (result);
        return -1;
      }
    return 0;
  }
}
