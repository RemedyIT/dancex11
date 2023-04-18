/**
 * @file    dancex11_ddh_application_manager_impl.cpp
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  Domain application manager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_ddh_application_manager_impl.h"
#include "dancex11/deployment/deployment_connectionC.h"
#include "dancex11/deployment/deployment_nodemanagerAmiC.h"
#include "dancex11/logger/log.h"
#include "dancex11/scheduler/completion_base.h"
#include "dancex11/scheduler/deployment_orb_wait_strategy.h"
#include "tools/split_plan/split_plan.h"
#include "tools/split_plan/node_splitter.h"

#include "ace/Future.h"

#include <sstream>

namespace DAnCEX11
{
  struct NM_Result
  {
    NM_Result () = default;

    NM_Result (IDL::traits<Deployment::NodeManager>::ref_type nm,
               std::string node,
               IDL::traits<::Deployment::ApplicationManager>::ref_type am)
     : nm_ (std::move(nm)), node_ (std::move(node)), am_ (std::move(am)) {}

    NM_Result (std::string error)
     : error_ (std::move (error)) {}

    static NM_Result error (std::string err)
    { return NM_Result (err); }

    IDL::traits<Deployment::NodeManager>::ref_type nm_;
    std::string node_;
    IDL::traits<::Deployment::ApplicationManager>::ref_type am_;
    std::string error_;
  };

  class NodeManager_ReplyHandler
    : public CORBA::amic_traits<Deployment::NodeManager>::replyhandler_base_type
  {
  public:
    using future_type = ACE_Future <NM_Result>;

    NodeManager_ReplyHandler (IDL::traits<PortableServer::POA>::ref_type poa,
                              IDL::traits<Deployment::NodeManager>::ref_type nm,
                              std::string node)
     : poa_ (std::move(poa)), nm_ (std::move(nm)), node_ (std::move(node)) {}

    ~NodeManager_ReplyHandler () override = default;

    future_type& result () { return this->result_; }

    // NodeManager methods
    void
    joinDomain () override {}
    void
    joinDomain_excep (
        IDL::traits<Messaging::ExceptionHolder>::ref_type) override {}
    void
    leaveDomain () override {}
    void
    leaveDomain_excep (
        IDL::traits<Messaging::ExceptionHolder>::ref_type) override {}
    void
    getDynamicResources (
        const ::Deployment::Resources&) override {}
    void
    getDynamicResources_excep (
        IDL::traits<Messaging::ExceptionHolder>::ref_type) override {}

    // Deployment_Manager methods
    void
    preparePlan (
        IDL::traits<::Deployment::ApplicationManager>::ref_type ami_return_val) override
    {
      DANCEX11_LOG_TRACE( "NodeManager_ReplyHandler::preparePlan");

      this->result_.set ({this->nm_, this->node_, ami_return_val});

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    void
    preparePlan_excep (
        IDL::traits<Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      DANCEX11_LOG_TRACE( "NodeManager_ReplyHandler::preparePlan_excep");

      try
      {
        excep_holder->raise_exception ();
      }
      catch ( const Deployment::PlanError &ex)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::preparePlan_excep - " <<
                           "Caught a PlanError exception handling node "
                           << this->node_  << " : " <<  ex);
        std::ostringstream err;
        err << this->node_
            << " - PlanError exception starting preparePlan : "
            << ex.name() << ", " << ex.reason();
        this->result_.set (NM_Result::error (err.str ()));
      }
      catch (const Deployment::StartError &ex)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::preparePlan_excep - " <<
                            "Caught a StartError exception handling node "
                             << this->node_  << " : " <<  ex);
        std::ostringstream err;
        err << this->node_
            << " - StartError exception starting preparePlan : "
            << ex;
        this->result_.set (NM_Result::error (err.str ()));
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::preparePlan_excep - " <<
                           "Caught a CORBA exception handling node "
                           << this->node_  << " : " <<  ex);
        std::ostringstream err;
        err << this->node_
            << " - CORBA exception starting preparePlan : "
            << ex;
        this->result_.set (NM_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::preparePlan_excep - " <<
                            "Caught unknown exception handling node "
                            << this->node_ );
        std::ostringstream err;
        err << this->node_
            << " - unknown exception starting preparePlan";
        this->result_.set (NM_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    void
    destroyManager () override
    {
      DANCEX11_LOG_TRACE( "NodeManager_ReplyHandler::destroyManager");

      this->result_.set ({nullptr, this->node_, nullptr});

      // deactivate
      PortableServer::ObjectId const oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }
    void
    destroyManager_excep (
        IDL::traits<Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      DANCEX11_LOG_TRACE( "NodeManager_ReplyHandler::destroyManager_excep");

      try
      {
        excep_holder->raise_exception ();
      }
      catch (const Deployment::StopError &ex)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::destroyManager_excep - " <<
                            "Caught a StopError exception handling node "
                             << this->node_  << " : " <<  ex);
        std::ostringstream err;
        err << this->node_
            << " - StopError exception executing destroyManager : "
            << ex;
        this->result_.set (NM_Result::error (err.str ()));
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::destroyManager_excep - " <<
                           "Caught a CORBA exception handling node "
                           << this->node_  << " : " <<  ex);
        std::ostringstream err;
        err << this->node_
            << " - CORBA exception executing destroyManager : "
            << ex;
        this->result_.set (NM_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NodeManager_ReplyHandler::destroyManager_excep - " <<
                            "Caught unknown exception handling node "
                            << this->node_ );
        std::ostringstream err;
        err << this->node_
            << " - unknown exception executing destroyManager";
        this->result_.set (NM_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

  private:
    IDL::traits<PortableServer::POA>::ref_type poa_;
    IDL::traits<Deployment::NodeManager>::ref_type nm_;
    std::string node_;
    future_type result_;
  };

  DomainApplicationManager_Impl::DomainApplicationManager_Impl (
    IDL::traits<PortableServer::POA>::ref_type poa,
    Deployment::DeploymentPlan plan,
    Node_Locator & nodes)
    : poa_ (std::move(poa))
    , plan_ (std::move (plan))
    , nodes_ (nodes)
  {
    DANCEX11_LOG_TRACE( "DomainApplicationManager_Impl::DomainApplicationManager_Impl");
  }

  DomainApplicationManager_Impl::~DomainApplicationManager_Impl()
  {
    DANCEX11_LOG_TRACE( "DomainApplicationManager_Impl::~DomainApplicationManager_Impl()");

    //Application's could already be deleted at his point by previous call to
    //destroyApplication();
    for (CORBA::servant_reference<DomainApplication_Impl> da : this->running_app_)
    {
      DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::~DomainApplicationManager_Impl - "
                          "Deleting application for plan " <<
                          this->getPlanUUID ());
      IDL::traits<CORBA::Object>::ref_type app =
        this->poa_->servant_to_reference (da);
      PortableServer::ObjectId const id = this->poa_->reference_to_id (app);
      this->poa_->deactivate_object (id);
      da.reset ();
    }

    this->running_app_.clear();
    this->sub_app_mgr_.clear();
    this->node_ids_.clear ();
  }

  IDL::traits<::Deployment::Application>::ref_type
  DomainApplicationManager_Impl::startLaunch (const ::Deployment::Properties& configProperty,
                                              ::Deployment::Connections& providedReference)
  {
    DANCEX11_LOG_TRACE("DomainApplicationManager_Impl::startLaunch");

    try
    {
      DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::startLaunch - "
                          "Creating DomainApplication for plan " <<
                         this->getPlanUUID ());


      // Creating Domainpplication object
      DANCEX11_LOG_TRACE ("DomainApplicationManager_impl::startLaunch - " <<
                          "Initializing DomainApplication");

      //app
      CORBA::servant_reference<DAnCEX11::DomainApplication_Impl> app_impl_ =
          CORBA::make_reference<DomainApplication_Impl> (
                this->getPlanUUID (),
                this->poa_,
                this->sub_app_mgr_,
                this->node_ids_);

      if (!app_impl_)
        throw CORBA::NO_MEMORY ();

      DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::startLaunch - "
                           "Successfully created DomainApplication for plan "
                           << this->getPlanUUID ());

      this->running_app_.push_back(app_impl_);

      app_impl_->startLaunch (configProperty, providedReference);

      DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::startLaunch - "
                          "DomainApplication startLaunch has been called");

      PortableServer::ObjectId const app_impl_id =
          this->poa_->activate_object (app_impl_);

      IDL::traits<CORBA::Object>::ref_type app_impl_obj = this->poa_->id_to_reference (app_impl_id);
      IDL::traits<Deployment::Application>::ref_type app = IDL::traits<Deployment::Application>::narrow (app_impl_obj);

      if (!app)
      {
        DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::startLaunch - " <<
                                "DomainApplication servant failed to activate");
        throw ::Deployment::StartError ("DomainApplication",
                                        "Activation failure");
      }
      return app;
    }
    catch (const CORBA::Exception &ex)
    {
      std::ostringstream os;
      os << ex;
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::startLaunch - "
                         "Caught a CORBA exception, propagating StartError: "
                         << os.str ());

      throw Deployment::StartError (this->getPlanUUID (),
                                    os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::startLaunch - "
                         "Caught unknown exception.  Propagating StartError");

      throw Deployment::StartError (this->getPlanUUID (),
                                    "Unknown C++ exception");
    }
    return {};
  }

  void
  DomainApplicationManager_Impl::destroyApplication (IDL::traits<::Deployment::Application>::ref_type app)
  {
    DANCEX11_LOG_TRACE ( "DomainApplicationManager_Impl::destroyApplication");

    try
    {
      bool found = false;
      TApplications::iterator iter;
      for (iter = this->running_app_.begin();
           iter != this->running_app_.end();
           ++iter)
      {
        CORBA::servant_reference<DomainApplication_Impl> da = (*iter);
        IDL::traits<CORBA::Object>::ref_type da_obj =
           this->poa_->servant_to_reference (da);

        IDL::traits<::Deployment::Application>::ref_type da_app_ =
                   IDL::traits<::Deployment::Application>::narrow (da_obj);
        if (da_app_->_is_equivalent(app))
        {
          found = true;

          PortableServer::ObjectId id = this->poa_->reference_to_id (app);
          this->poa_->deactivate_object (id);


          da->destroyApplication();

          break;
        }
      }
      if (found)
      {
        app.reset ();
        this->running_app_.erase(iter);
        DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::destroyApplication - "
                            "Delete application for plan " <<
                             this->getPlanUUID ());
      }
      else
      {
        DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::destroyApplication - "
                            "Didn't find application for plan " <<
                             this->getPlanUUID ());
      }
      return;
    }
    catch (const CORBA::SystemException &)
    {
      throw;
    }
    catch (const Deployment::StopError &)
    {
      throw;
    }
    catch (const CORBA::UserException &e)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyApplication failed " <<
                           "with UserException " << e);
      std::ostringstream os;
      os << e;
      throw Deployment::StopError("DomainApplicatoinManager", os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyApplication failed " <<
                          "with unknown exception.");
      throw Deployment::StopError("DomainApplicatoinManager", "Unknown C++ exception in destroyApplication");
     }
  }

  Deployment::Applications
  DomainApplicationManager_Impl::getApplications ()
  {
    DANCEX11_LOG_TRACE ("DomainApplicationManager_Impl::getApplications ()");

    ::Deployment::Applications running_app;
    for (CORBA::servant_reference<DomainApplication_Impl> iter : this->running_app_)
    {
      IDL::traits<CORBA::Object>::ref_type app = this->poa_->servant_to_reference(iter);
      running_app.push_back(IDL::traits<::Deployment::Application>::narrow (app));
    }
    return running_app;
  }

  ::Deployment::DeploymentPlan
  DomainApplicationManager_Impl::getPlan ()
  {
    DANCEX11_LOG_TRACE ("DomainApplicationManager_Impl::getPlan ()");
    return this->plan_;
  }

  std::string
  DomainApplicationManager_Impl::getPlanUUID ()
  {
    return this->plan_.UUID();
  }
  //TO DO plan not nesseccary
  void
  DomainApplicationManager_Impl::preparePlan ()
  {
    DANCEX11_LOG_TRACE ("DomainApplicationManager_Impl::preparePlan()");

    using completion_type = Completion_T<ORB_Completion_Wait_Strategy<NM_Result>,
                         NM_Result>;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;
    try
    {
     //  this->split_plan_.split_plan (plan);
      this->split_plan_.split_plan (this->plan_);
      for (NodeSplitter::TSubPlanIterator itplan = this->split_plan_.plans ().begin ();
           itplan != this->split_plan_.plans ().end ();
           ++itplan)
      {
        const NodeSplitter::TSubPlanKey &sub_plan_key = itplan->first;

        // Finding correspondent NodeManager
        IDL::traits<Deployment::NodeManager>::ref_type nm = this->nodes_.locate_node (sub_plan_key);
        // If NodeManager not found throw StartError exception
        if (!nm)
        {
          DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                               "Deployment::StartError exception. NodeManager "
                               << sub_plan_key << " cannot be found");
          throw Deployment::StartError (sub_plan_key,
                                        "NodeManager not found");
        }
        // narrow to async interface
        CORBA::amic_traits<Deployment::NodeManager>::ref_type nm_async =
            CORBA::amic_traits<Deployment::NodeManager>::narrow (nm);

        // create reply handler
        CORBA::servant_traits<NodeManager_ReplyHandler>::ref_type reply =
            CORBA::make_reference<NodeManager_ReplyHandler> (
                this->poa_, nm, sub_plan_key);

        // activate object
        PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

        // get Reply Handler reference
        IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
        CORBA::amic_traits<Deployment::NodeManager>::replyhandler_ref_type reply_ref =
            CORBA::amic_traits<Deployment::NodeManager>::replyhandler_traits::narrow (obj);

        try
        {
          // schedule async result
          completion.accept (reply->result ());

          //ResourceCommitmentManage is for future use
          IDL::traits<::Deployment::ResourceCommitmentManager>::ref_type rm;
          nm_async->sendc_preparePlan (reply_ref,
                                       itplan->second,
                                       rm);

          reply.reset (); // clear reply
        }
        catch (const CORBA::Exception &ex)
        {
          DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                             "Caught a CORBA exception handling node "
                             << itplan->first  << " : " <<  ex);
          std::ostringstream err;
          err << itplan->first
              << " - CORBA exception starting preparePlan : "
              << ex;

          // report error result for scheduled reply
          reply->result ().set (NM_Result::error (err.str ()));
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                              "Caught unknown exception handling node "
                              << itplan->first );
          std::ostringstream err;
          err << itplan->first
              << " - unknown exception starting preparePlan";

          // report error result for scheduled reply
          reply->result ().set (NM_Result::error (err.str ()));
        }

        // if reply handler servant reference not cleared deactivate the servant
        if (reply)
        {
          this->poa_->deactivate_object (oid);
        }
      }

      // wait for all async request to be handled
      completion.wait_on_completion (nullptr);

      // get completed request results
      completion_type::future_list requests;
      completion.completed_events (requests);

      for (completion_type::future_type &request : requests)
      {
        NM_Result result;
        request.get (result);

        if (result.am_)
        {
          IDL::traits<::Deployment::NodeApplicationManager>::ref_type nam =
            IDL::traits<::Deployment::NodeApplicationManager>::narrow (result.am_);

          DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::preparePlan - " <<
                               "preparePlan on node [" << result.node_ <<
                               "] has been called");

          // We save combination NAM and NM in TNam2Nm vector
          this->sub_app_mgr_.push_back(
                  std::pair< IDL::traits<::Deployment::NodeApplicationManager>::ref_type,
                             IDL::traits<::Deployment::NodeManager>::ref_type>(nam, result.nm_));
          // Save combination NM and node_id in TNm2Id vector
          this->node_ids_.push_back( DomainApplication_Impl::TNm2Id_PAIR (result.nm_, result.node_));
        }
        else
        {
          errors.push_back (result.error_);
          failed = true;
        }
      }

      if (failed)
      {
        DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                            errors.size () << " errors captured from async "
                            "Node preparePlan requests");
        std::ostringstream os;
        os << "captured preparePlan errors:\n";
        for (std::string &err : errors)
          os << err << "\n";
        throw Deployment::StartError (this->plan_.UUID(), os.str ());
      }
    }
    catch (const Deployment::StartError &e)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                         "Propagating StartError exception caught here");
      throw e;
    }
    catch (const CORBA::Exception &ex)
    {
      std::ostringstream os;
      os << ex;
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                         "Caught a CORBA exception, propagating StartError: "
                         << os.str ());

      throw Deployment::StartError (this->plan_.UUID(),
                                    os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::preparePlan - " <<
                         "Caught unknown exception.  Propagating StartError");

      throw Deployment::StartError (this->plan_.UUID(),
                                    "Unknown C++ exception");
    }
    DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::preparePlan - finished");
  }

  //Never used, why implement?
  void
  DomainApplicationManager_Impl::destroyManager ()
  {
    DANCEX11_LOG_TRACE ("DomainApplicationManager_Impl::destroyManager");

    if (this->running_app_.size () > 0)
    {
      DANCEX11_LOG_WARNING ("DomainApplicationManager_Impl::destroyManager - " <<
                            "Destroying " << this->running_app_.size ()
                            << " applications for plan " << this->getPlanUUID ());

      try
      {
        // destroy any still running applications
        // (correct shutdown should already have finished these).
        while (this->running_app_.size () > 0)
        {
          IDL::traits<CORBA::Object>::ref_type app =
            this->poa_->servant_to_reference (this->running_app_.front ());
          this->destroyApplication (IDL::traits<Deployment::Application>::narrow (app));
        }
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                            "caught CORBA exception destroying domain applications : " <<
                            ex);
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                            "caught unknown exception destroying domain applications.");
      }
    }

    // now call destroyManager() for all node-app managers
    using completion_type = Completion_T<ORB_Completion_Wait_Strategy<NM_Result>,
                         NM_Result>;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;
    try
    {
      for (DomainApplication_Impl::TNam2Nm_PAIR& nampair : this->sub_app_mgr_)
      {
        // Get NodeManager
        IDL::traits<Deployment::NodeManager>::ref_type nm
            = nampair.second;

        // Find corresponding node name
        std::string const node = this->findNode4NM (nm);

        // narrow to async interface
        CORBA::amic_traits<Deployment::NodeManager>::ref_type nm_async =
            CORBA::amic_traits<Deployment::NodeManager>::narrow (nm);

        // create reply handler
        CORBA::servant_traits<NodeManager_ReplyHandler>::ref_type reply =
            CORBA::make_reference<NodeManager_ReplyHandler> (
                this->poa_, nm, node);

        // activate object
        PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

        // get Reply Handler reference
        IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
        CORBA::amic_traits<Deployment::NodeManager>::replyhandler_ref_type reply_ref =
            CORBA::amic_traits<Deployment::NodeManager>::replyhandler_traits::narrow (obj);

        try
        {
          // schedule async result
          completion.accept (reply->result ());

          nm_async->sendc_destroyManager (reply_ref,
                                          nampair.first);

          reply.reset (); // clear reply
        }
        catch (const CORBA::Exception &ex)
        {
          DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                             "Caught a CORBA exception handling node "
                             << node  << " : " <<  ex);
          std::ostringstream err;
          err << node
              << " - CORBA exception starting destroyManager : "
              << ex;

          // report error result for scheduled reply
          reply->result ().set (NM_Result::error (err.str ()));
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                              "Caught unknown exception handling node "
                              << node );
          std::ostringstream err;
          err << node
              << " - unknown exception starting destroyManager";

          // report error result for scheduled reply
          reply->result ().set (NM_Result::error (err.str ()));
        }

        // if reply handler servant reference not cleared deactivate the servant
        if (reply)
        {
          this->poa_->deactivate_object (oid);
        }
      }

      // wait for all async request to be handled
      completion.wait_on_completion (nullptr);

      // get completed request results
      completion_type::future_list requests;
      completion.completed_events (requests);

      for (completion_type::future_type &request : requests)
      {
        NM_Result result;
        request.get (result);

        if (result.error_.empty ())
        {
          DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::destroyManager - " <<
                               "destroyManager on node " << result.node_ <<
                               " has been called.");
        }
        else
        {
          errors.push_back (result.error_);
          failed = true;
        }
      }

      if (failed)
      {
        DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                            errors.size () << " errors captured from async "
                            "Node destroyManager requests");
        std::ostringstream os;
        os << "captured destroyManager errors:\n";
        for (std::string &err : errors)
          os << err << "\n";
        throw Deployment::StopError (this->plan_.UUID(), os.str ());
      }
    }
    catch (const Deployment::StopError &e)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                          "Propagating StopError exception caught here");
      throw e;
    }
    catch (const CORBA::Exception &ex)
    {
      std::ostringstream os;
      os << ex;
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                          "Caught a CORBA exception, propagating StopError: "
                         << os.str ());

      throw Deployment::StopError (this->plan_.UUID(),
                                   os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplicationManager_Impl::destroyManager - " <<
                          "Caught unknown exception.  Propagating StopError");

      throw Deployment::StopError (this->plan_.UUID(),
                                   "Unknown C++ exception");
    }
    DANCEX11_LOG_DEBUG ("DomainApplicationManager_Impl::destroyManager - finished");
  }


  const std::string
  DomainApplicationManager_Impl::findNode4NM(
      IDL::traits<::Deployment::NodeManager>::ref_type nm)
  {
    for (const DomainApplication_Impl::TNm2Id_PAIR& nmpair : this->node_ids_)
    {
      if (nm->_is_equivalent(nmpair.first))
      {
        return nmpair.second;
      }
    }
    return {};
  }

} // DAnCEX11
