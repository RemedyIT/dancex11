/**
 * @file   dancex11_ddh_application_impl.cpp
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  Domain application
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11_ddh_application_impl.h"
#include "dancex11/deployment/deployment_nodeapplicationmanagerAmiC.h"
#include "dancex11/deployment/deployment_nodeapplicationAmiC.h"
#include "dancex11/logger/log.h"
#include "dancex11/scheduler/completion_base.h"
#include "dancex11/scheduler/deployment_orb_wait_strategy.h"

#include "ace/Future.h"

#include <sstream>

namespace DAnCEX11
{
  struct NAM_Result
  {
    NAM_Result () = default;
    NAM_Result (IDL::traits<Deployment::NodeApplicationManager>::ref_type nam,
                std::string node)
      : nam_ (nam), node_ (std::move (node)) {}
    NAM_Result (IDL::traits<Deployment::NodeApplicationManager>::ref_type nam,
                std::string node,
                IDL::traits< ::Deployment::Application>::ref_type app,
                const Deployment::Connections &providedReference)
      : nam_ (nam), node_ (std::move (node)), app_ (app), providedReference_ (providedReference) {}
    NAM_Result (std::string err)
      : error_ (std::move (err)) {}

    static NAM_Result error (std::string err)
    { return NAM_Result (err); }

    IDL::traits<Deployment::NodeApplicationManager>::ref_type nam_;
    std::string node_;
    IDL::traits< ::Deployment::Application>::ref_type app_;
    ::Deployment::Connections providedReference_;
    std::string error_;
  };

  class NAM_ReplyHandler
    : public CORBA::amic_traits<Deployment::NodeApplicationManager>::replyhandler_base_type
  {
  public:
    typedef ACE_Future <NAM_Result> future_type;

    NAM_ReplyHandler (IDL::traits<PortableServer::POA>::ref_type poa,
                      IDL::traits<Deployment::NodeApplicationManager>::ref_type nam,
                      std::string node)
     : poa_ (std::move(poa)), nam_ (std::move(nam)), node_ (std::move(node)) {}
    virtual ~NAM_ReplyHandler () = default;

    future_type& result () { return this->result_; }

    virtual
    void
    startLaunch (
        IDL::traits< ::Deployment::Application>::ref_type ami_return_val,
        const ::Deployment::Connections& providedReference) override
    {
      this->result_.set ({this->nam_, this->node_, ami_return_val, providedReference});

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    startLaunch_excep (
        IDL::traits< Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      try
      {
        excep_holder->raise_exception ();
      }
      catch (const Deployment::PlanError &ex)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::startLaunch_excep - " <<
                            "Caught a PlanError Exception handling node " << this->node_
                            << " : " <<  ex);
        std::ostringstream err;
        err << "PlanError exception executing startLaunch on node "
            << this->node_ << " : " << ex;
        this->result_.set (NAM_Result::error (err.str ()));
      }
      catch (const Deployment::StartError &ex)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::startLaunch_excep - " <<
                            "Caught a StartError Exception handling node " << this->node_
                            << " : " <<  ex);
        std::ostringstream err;
        err << "StartError exception executing startLaunch on node "
            << this->node_ << " : " << ex;
        this->result_.set (NAM_Result::error (err.str ()));
      }
      catch (const CORBA::Exception &ex)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::startLaunch_excep - " <<
                           "Caught a CORBA Exception executing startLaunch on node " <<
                           this->node_ << " : " <<  ex );
        std::ostringstream err;
        err << "CORBA exception executing startLaunch on node "
            << this->node_ << " : " << ex;
        this->result_.set (NAM_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::startLaunch_excep - " <<
                            "caught a unknown C++ exception handling node "
                            << this->node_);
        std::ostringstream err;
        err << "caught a unknown C++ exception executing startLaunch on node "
            << this->node_;
        this->result_.set (NAM_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    // generated from c++11/templates/cli/hdr/ami/operation.erb
    /// @copydoc Deployment::ApplicationManager::destroyApplication
    virtual
    void
    destroyApplication () override
    {
      this->result_.set ({this->nam_, this->node_});

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    destroyApplication_excep (
        IDL::traits< Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      try
      {
        excep_holder->raise_exception ();
      }
      catch (const Deployment::StopError &ex)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::destroyApplication_excep - " <<
                            "Caught a StopError Exception handling node " << this->node_
                            << " : " <<  ex);
        std::ostringstream err;
        err << "StopError exception executing destroyApplication on node "
            << this->node_ << " : " << ex;
        this->result_.set (NAM_Result::error (err.str ()));
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::destroyApplication_excep - " <<
                      "caught a CORBA exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "CORBA exception executing destroyApplication on node "
            << this->node_ << " : " << ex;
        this->result_.set (NAM_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NAM_ReplyHandler::destroyApplication_excep - " <<
                            "caught a unknown exception handling node " << this->node_);
        std::ostringstream err;
        err << "unknown exception executing destroyApplication on node "
            << this->node_;
        this->result_.set (NAM_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

  private:
    IDL::traits<PortableServer::POA>::ref_type poa_;
    IDL::traits<Deployment::NodeApplicationManager>::ref_type nam_;
    std::string node_;
    future_type result_;
  };

  struct NA_Result
  {
    NA_Result () = default;
    NA_Result (std::string node)
      : node_ (std::move (node)) {}

    static NA_Result error (std::string err)
    {
      NA_Result r; r.error_ = std::move (err); return r;
    }

    std::string node_;
    std::string error_;
  };

  class NA_ReplyHandler
    : public CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_base_type
  {
  public:
    typedef ACE_Future <NA_Result> future_type;

    NA_ReplyHandler (IDL::traits<PortableServer::POA>::ref_type poa,
                     std::string node)
     : poa_ (std::move(poa)), node_ (std::move(node)) {}
    virtual ~NA_ReplyHandler () = default;

    future_type& result () { return this->result_; }

    virtual
    void
    finishLaunch () override
    {
      this->result_.set ({this->node_});

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    finishLaunch_excep (
        IDL::traits< Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      try
      {
        excep_holder->raise_exception ();
      }
      catch (const Deployment::StartError &ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::finishLaunch_excep - " <<
                      "caught a StartError exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "StartError exception executing finishLaunch on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (const Deployment::InvalidConnection &ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::finishLaunch_excep - " <<
                      "caught an InvalidConnection exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "InvalidConnection exception executing finishLaunch on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::finishLaunch_excep - " <<
                      "caught a CORBA exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "CORBA exception executing finishLaunch on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::finishLaunch_excep - " <<
                            "caught a unknown exception handling node " << this->node_);
        std::ostringstream err;
        err << "unknown exception executing finishLaunch on node "
            << this->node_;
        this->result_.set (NA_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    start () override
    {
      this->result_.set ({this->node_});

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    start_excep (
        IDL::traits< Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      try
      {
        excep_holder->raise_exception ();
      }
      catch (const Deployment::StartError &ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::start_excep - " <<
                      "caught a StartError exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "StartError exception executing start on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::start_excep - " <<
                      "caught a CORBA exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "CORBA exception executing start on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::start_excep - " <<
                            "caught a unknown exception handling node " << this->node_);
        std::ostringstream err;
        err << "unknown exception executing start on node "
            << this->node_;
        this->result_.set (NA_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    stop () override
    {
      this->result_.set ({this->node_});

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

    virtual
    void
    stop_excep (
        IDL::traits< Messaging::ExceptionHolder>::ref_type excep_holder) override
    {
      try
      {
        excep_holder->raise_exception ();
      }
      catch (const Deployment::StopError &ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::stop_excep - " <<
                      "caught a StopError exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "StopError exception executing stop on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::stop_excep - " <<
                      "caught a CORBA exception handling node " << this->node_
                      << " : " << ex);
        std::ostringstream err;
        err << "CORBA exception executing stop on node "
            << this->node_ << " : " << ex;
        this->result_.set (NA_Result::error (err.str ()));
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("NA_ReplyHandler::stop_excep - " <<
                            "caught a unknown exception handling node " << this->node_);
        std::ostringstream err;
        err << "unknown exception executing stop on node "
            << this->node_;
        this->result_.set (NA_Result::error (err.str ()));
      }

      // deactivate
      PortableServer::ObjectId oid = this->poa_->servant_to_id (this->_lock ());
      this->poa_->deactivate_object (oid);
    }

  private:
    IDL::traits<PortableServer::POA>::ref_type poa_;
    std::string node_;
    future_type result_;
  };

  DomainApplication_Impl::DomainApplication_Impl (
    const std::string& plan_uuid,
    IDL::traits<PortableServer::POA>::ref_type poa,
    TNam2Nm & nams,
    TNm2Id& node_ids)
      : poa_ (std::move(poa)),
        planUUID_ (plan_uuid),
        nams_ (nams),
        node_ids_ (node_ids)
  {
    DANCEX11_LOG_TRACE("DomainApplication_Impl::DomainApplication_Impl");
  }

  DomainApplication_Impl::~DomainApplication_Impl()
  {
    DANCEX11_LOG_TRACE ("DomainApplication_Impl::~DomainApplication_Impl()");

    this->node_applications_.clear();
  }

  void
  DomainApplication_Impl::finishLaunch (const ::Deployment::Connections & connections,
                                        bool start)
  {
    DANCEX11_LOG_TRACE ("DomainApplication_Impl::finishLaunch");

    typedef Completion_T<ORB_Completion_Wait_Strategy<NA_Result>,
                         NA_Result> completion_type;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;

    try
    {
    DANCEX11_LOG_DEBUG ("DomainApplication_impl::finishLaunch - "
                        "Plan " << this->planUUID_ << " DomainApplication[" << this<<"] : " <<
                        this->node_applications_.size () << " applications to finish launching");
      for (TApp2Mgr::iterator iter = this->node_applications_.begin();
          iter != this->node_applications_.end();
          ++iter)
      {
        const std::string node_id = findNodeTApp2Id(iter->first);

        // narrow to async interface
        CORBA::amic_traits<Deployment::NodeApplication>::ref_type app_async =
            CORBA::amic_traits<Deployment::NodeApplication>::narrow (iter->first);

        // create reply handler
        CORBA::servant_traits<NA_ReplyHandler>::ref_type reply =
            CORBA::make_reference<NA_ReplyHandler> (
                this->poa_, node_id);

        // activate object
        PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

        // get Reply Handler reference
        IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
        CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_ref_type reply_ref =
            CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_traits::narrow (obj);

        try
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::finishLaunch - "
                        "Plan " << this->planUUID_ << " DomainApplication[" <<
                        this << "] node " << node_id << " : "
                        "calling finishLaunch on a node application");

          // schedule async result
          completion.accept (reply->result ());

          app_async->sendc_finishLaunch (reply_ref,
                                         connections,
                                         start);

          reply.reset (); // clear reply
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::finishLaunch - " <<
                              "DomainApplication[" << this  <<
                              "] caught a CORBA exception handling node " << node_id
                              << " : " << ex);
          std::ostringstream err;
          err << node_id
              << " - CORBA exception starting : "
              << ex;
          reply->result ().set (NA_Result::error (err.str ()));
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::finishLaunch - " <<
                              "DomainApplication[" << this
                              << "] caught a unknown exception handling node ");
          std::ostringstream err;
          err << node_id
              << " - unknown exception starting finishLaunch";
          reply->result ().set (NA_Result::error (err.str ()));
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
        NA_Result result;
        request.get (result);

        if (result.error_.empty ())
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::finishLaunch - " <<
                              "successfully finished launching node: "
                               <<  result.node_);
        }
        else
        {
          errors.push_back (result.error_);
          failed = true;
        }
      }

      if (failed)
      {
        DANCEX11_LOG_ERROR ("DomainApplication_Impl::finishLaunch - " <<
                            errors.size () << " errors captured from async "
                            "Node finishLaunch requests");
        std::ostringstream os;
        os << "Got the following finishLaunch errors: ";
        for (std::string &err : errors)
          os << err << "\n";
        throw Deployment::StartError (this->planUUID_, os.str ());
      }
    }
    catch (const CORBA::SystemException &)
    {
      throw;
    }
    catch (const Deployment::StartError &)
    {
      throw;
    }
    catch (const CORBA::UserException &e)
    {
      DANCEX11_LOG_ERROR ("DomainApplication_Impl::finishLaunch failed " <<
                          "with UserException " << e);
      std::ostringstream os;
      os << e;
      throw Deployment::StartError("DomainApplication_Impl", os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplication_Impl::finishLaunch " <<
                          "with unknown exception.");
      throw Deployment::StartError("DomainApplication_Impl",
                                   "Unknown C++ exception in finishLaunch");
    }

  }

  void
  DomainApplication_Impl::start ( )
  {
    DANCEX11_LOG_TRACE ("DomainApplication_Impl::start");

    typedef Completion_T<ORB_Completion_Wait_Strategy<NA_Result>,
                         NA_Result> completion_type;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;

    try
    {
      DANCEX11_LOG_DEBUG ("DomainApplication_impl::start - " <<
                    "Plan " <<  this->planUUID_<< " DomainApplication[" << this
                    << "] : " << this->node_applications_.size () << " applications to start");

      for (TApp2Mgr::iterator iter = this->node_applications_.begin();
          iter != this->node_applications_.end();
          ++iter)
      {
        const std::string node_id = findNodeTApp2Id(iter->first);

        // narrow to async interface
        CORBA::amic_traits<Deployment::NodeApplication>::ref_type app_async =
            CORBA::amic_traits<Deployment::NodeApplication>::narrow (iter->first);

        // create reply handler
        CORBA::servant_traits<NA_ReplyHandler>::ref_type reply =
            CORBA::make_reference<NA_ReplyHandler> (
                this->poa_, node_id);

        // activate object
        PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

        // get Reply Handler reference
        IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
        CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_ref_type reply_ref =
            CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_traits::narrow (obj);

        try
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::start - " <<
                              "calling start on " << node_id <<
                              " application");

          // schedule async result
          completion.accept (reply->result ());

          app_async->sendc_start (reply_ref);

          reply.reset (); // clear reply
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::start - " <<
                              "DomainApplication[" << this  <<
                              "] caught a CORBA exception handling node "
                              << node_id << " : " << ex);
          std::ostringstream err;
          err << node_id
              << " - CORBA exception starting : "
              << ex;
          reply->result ().set (NA_Result::error (err.str ()));
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::start - " <<
                              "DomainApplication[" << this <<
                              "] caught a unknown exception handling node "
                              << node_id << "");
          std::ostringstream err;
          err << node_id
              << " - unknown exception starting";
          reply->result ().set (NA_Result::error (err.str ()));
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
        NA_Result result;
        request.get (result);

        if (result.error_.empty ())
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::start - " <<
                              "successfully started node: "
                               <<  result.node_);
        }
        else
        {
          errors.push_back (result.error_);
          failed = true;
        }
      }

      if (failed)
      {
        DANCEX11_LOG_ERROR ("DomainApplication_Impl::start - " <<
                            errors.size () << " errors captured from async "
                            "Node start requests");
        std::ostringstream os;
        os << "captured start errors:\n";
        for (std::string &err : errors)
          os << err << "\n";
        throw Deployment::StartError (this->planUUID_, os.str ());
      }
    }
    catch (const CORBA::Exception& ex)
    {
      std::ostringstream os;
      os << ex;
      throw Deployment::StartError("DomainApplication_Impl", os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplication_Impl::start - " <<
                          "Propagating StartError for unknown exception caught here");
     throw Deployment::StartError(this->planUUID_,
                                  "unknown exception in start");
    }
  }

  void
  DomainApplication_Impl::startLaunch (
    const Deployment::Properties & configProperty,
    ::Deployment::Connections& providedReference)
  {
    DANCEX11_LOG_TRACE ("DomainApplication_Impl::startLaunch");

    DANCEX11_LOG_DEBUG ("DomainApplication_Impl::startLaunch - " <<
                  "Plan " << this->planUUID_ << " DomainApplication[" << this << "] : " <<  nams_.size ()
                  << " nodes to launch");

    typedef Completion_T<ORB_Completion_Wait_Strategy<NAM_Result>,
                         NAM_Result> completion_type;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;

    try
    {
      for (TNam2Nm::iterator iter = this->nams_.begin();
           iter != this->nams_.end();
           ++iter)
      {
        std::string node_id;
        for (TNm2Id::iterator node_iter = this->node_ids_.begin();
          node_iter != this->node_ids_.end();
          ++node_iter)
        {
          if (node_iter->first->_is_equivalent(iter->second))
          {
            node_id = node_iter->second;
          }
        }

        DANCEX11_LOG_DEBUG ("DomainApplication_Impl::startLaunch - " <<
                        "Plan " <<  this->planUUID_<< " DomainApplication[" << this << "] : "
                        "calling startLaunch on node application manager for node [" << node_id << "]");

        // narrow to async interface
        CORBA::amic_traits<Deployment::ApplicationManager>::ref_type am_async =
            CORBA::amic_traits<Deployment::ApplicationManager>::narrow (iter->first);

        // create reply handler
        CORBA::servant_traits<NAM_ReplyHandler>::ref_type reply =
            CORBA::make_reference<NAM_ReplyHandler> (
                this->poa_, iter->first, node_id);

        // activate object
        PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

        // get Reply Handler reference
        IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
        CORBA::amic_traits<Deployment::ApplicationManager>::replyhandler_ref_type reply_ref =
            CORBA::amic_traits<Deployment::ApplicationManager>::replyhandler_traits::narrow (obj);

        try
        {
          // schedule async result
          completion.accept (reply->result ());

          ::Deployment::Connections instance_references;
          am_async->sendc_startLaunch (reply_ref, configProperty);

          reply.reset (); // clear reply

        }
        catch (const CORBA::Exception &ex)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::startLaunch - " <<
                             "Caught a CORBA Exception handling node " << node_id
                             << " : " <<  ex );
          std::ostringstream err;
          err << node_id
              << " - CORBA exception starting startLaunch : "
              << ex;

          // report error result for scheduled reply
          reply->result ().set (NAM_Result::error (err.str ()));
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::startLaunch - " <<
                              "DomainApplication caught an unknown exception handling node "
                              << node_id );
          std::ostringstream err;
          err << node_id
              << " - unknown C++ exception starting startLaunch";

          // report error result for scheduled reply
          reply->result ().set (NAM_Result::error (err.str ()));
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
        NAM_Result result;
        request.get (result);

        if (result.app_)
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::startLaunch - " <<
                              "instance_references : "
                               <<  result.providedReference_.size ());

          this->node_applications_.push_back ( TApp2Mgr_PAIR(result.app_, result.nam_));
          this->app_node_ids_.push_back (TApp2Id_PAIR(result.app_, result.node_));

          for (::Deployment::Connection conn : result.providedReference_ )
          {
            providedReference.push_back(conn);
          }
        }
        else
        {
          errors.push_back (result.error_);
          failed = true;
        }
      }

      if (failed)
      {
        DANCEX11_LOG_ERROR ("DomainApplication_Impl::startLaunch - " <<
                            errors.size () << " errors captured from async "
                            "Node startLaunch requests");
        std::ostringstream os;
        os << "captured startLaunch errors:\n";
        for (std::string &err : errors)
          os << err << "\n";
        throw Deployment::StopError (this->planUUID_, os.str ());
      }

      DANCEX11_LOG_DEBUG ("DomainApplication_Impl::startLaunch - " <<
                      "startLaunch has been called on an application");
    }
    catch (const CORBA::Exception& ex)
    {
      DANCEX11_LOG_ERROR ("DomainApplication_Impl::startLaunch - " <<
                    "DomainApplication[" << this << "] caught a CORBA exception"
                    << " : " << ex << "");
      std::ostringstream err;
      err << this->planUUID_
          << " - CORBA exception handling startLaunch"
          << " : " << ex;
      throw Deployment::StartError (this->planUUID_, err.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplication_Impl::startLaunch - " <<
                          "DomainApplication[" << this << "] caught an unknown exception");
      std::ostringstream err;
      err << this->planUUID_
          << " - unknown exception handling startLaunch";
      throw Deployment::StartError (this->planUUID_, err.str ());
    }
  }

  void
  DomainApplication_Impl::stop ( )
  {
    DANCEX11_LOG_TRACE ("DomainApplication_Impl::stop");

    typedef Completion_T<ORB_Completion_Wait_Strategy<NA_Result>,
                         NA_Result> completion_type;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;

    try
    {
      DANCEX11_LOG_DEBUG ("DomainApplication_impl::stop - " <<
                    "Plan <" <<  this->planUUID_<< "> DomainApplication[" << this
                    << "] : " << this->node_applications_.size () << " applications to stop");

      for (TApp2Mgr::iterator iter = this->node_applications_.begin();
          iter != this->node_applications_.end();
          ++iter)
      {
        std::string const node_id = findNodeTApp2Id(iter->first);

        // narrow to async interface
        CORBA::amic_traits<Deployment::NodeApplication>::ref_type app_async =
            CORBA::amic_traits<Deployment::NodeApplication>::narrow (iter->first);

        // create reply handler
        CORBA::servant_traits<NA_ReplyHandler>::ref_type reply =
            CORBA::make_reference<NA_ReplyHandler> (
                this->poa_, node_id);

        // activate object
        PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

        // get Reply Handler reference
        IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
        CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_ref_type reply_ref =
            CORBA::amic_traits<Deployment::NodeApplication>::replyhandler_traits::narrow (obj);

        try
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::stop - " <<
                              "calling stop on <" << node_id <<
                              "> application");

          // schedule async result
          completion.accept (reply->result ());

          app_async->sendc_stop (reply_ref);

          reply.reset (); // clear reply
        }
        catch (const CORBA::Exception& ex)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::stop - " <<
                              "DomainApplication[" << this  <<
                              "] caught a CORBA exception handling node "
                              << node_id << " : " << ex);
          std::ostringstream err;
          err << node_id
              << " - CORBA exception stopping : "
              << ex;
          reply->result ().set (NA_Result::error (err.str ()));
        }
        catch (...)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::stop - " <<
                              "DomainApplication[" << this <<
                              "] caught a unknown exception handling node "
                              << node_id << "");
          std::ostringstream err;
          err << node_id
              << " - unknown exception stopping";
          reply->result ().set (NA_Result::error (err.str ()));
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
        NA_Result result;
        request.get (result);

        if (result.error_.empty ())
        {
          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::stop - " <<
                              "successfully stopped node: "
                               <<  result.node_);
        }
        else
        {
          errors.push_back (result.error_);
          failed = true;
        }
      }

      if (failed)
      {
        DANCEX11_LOG_ERROR ("DomainApplication_Impl::stop - " <<
                            errors.size () << " errors captured from async "
                            "Node stop requests");
        std::ostringstream os;
        os << "captured stop errors:\n";
        for (std::string &err : errors)
          os << err << "\n";
        throw Deployment::StopError (this->planUUID_, os.str ());
      }
    }
    catch (const CORBA::Exception& ex)
    {
      std::ostringstream os;
      os << ex;
      throw Deployment::StopError("DomainApplication_Impl", os.str ());
    }
    catch (...)
    {
      DANCEX11_LOG_ERROR ("DomainApplication_Impl::stop - " <<
                          "Propagating StopError for unknown exception caught here");
     throw Deployment::StopError(this->planUUID_,
                                  "unknown exception in stop");
    }
  }

  void
  DomainApplication_Impl::destroyApplication ()
  {
    DANCEX11_LOG_TRACE ("DomainApplication_Impl::destroyApplication");

    DANCEX11_LOG_DEBUG ("DomainApplication_Impl::destroyApplication - "
                  "Plan " << this->planUUID_ << " DomainApplication[" << this
                  << "] : " <<  this->node_applications_.size ()<< " node applications to destroy");

    typedef Completion_T<ORB_Completion_Wait_Strategy<NAM_Result>,
                         NAM_Result> completion_type;
    completion_type completion;
    bool failed {};
    std::vector<std::string> errors;

    if (this->node_applications_.size () > 0)
    {
      try
      {
        for (TApp2Mgr::iterator iter = this->node_applications_.begin();
            iter != this->node_applications_.end();
            ++iter)
        {
          const std::string node_id = findNodeTApp2Id(iter->first);

          DANCEX11_LOG_DEBUG ("DomainApplication_Impl::destroyApplication - "
                              "Plan " << this->planUUID_ << " DomainApplication[" << this << "] : "
                              "calling destroyApplication on node application manager for node [" << node_id << "]");
          // narrow to async interface
          CORBA::amic_traits<Deployment::ApplicationManager>::ref_type am_async =
              CORBA::amic_traits<Deployment::ApplicationManager>::narrow (iter->second);

          // create reply handler
          CORBA::servant_traits<NAM_ReplyHandler>::ref_type reply =
              CORBA::make_reference<NAM_ReplyHandler> (
                  this->poa_, iter->second, node_id);

          // activate object
          PortableServer::ObjectId const oid = this->poa_->activate_object (reply);

          // get Reply Handler reference
          IDL::traits<CORBA::Object>::ref_type obj = this->poa_->id_to_reference (oid);
          CORBA::amic_traits<Deployment::ApplicationManager>::replyhandler_ref_type reply_ref =
              CORBA::amic_traits<Deployment::ApplicationManager>::replyhandler_traits::narrow (obj);

          try
          {
            // schedule async result
            completion.accept (reply->result ());

            am_async->sendc_destroyApplication (reply_ref, iter->first);

            reply.reset (); // clear reply
          }
          catch (const CORBA::Exception &ex)
          {
            DANCEX11_LOG_ERROR ("DomainApplication_Impl::destroyApplication - " <<
                               "Caught a CORBA exception handling node "
                               << node_id  << " : " <<  ex);
            std::ostringstream err;
            err << node_id
                << " - CORBA exception starting preparePlan : "
                << ex;

            // report error result for scheduled reply
            reply->result ().set (NAM_Result::error (err.str ()));
          }
          catch (...)
          {
            DANCEX11_LOG_ERROR ("DomainApplication_Impl::destroyApplication - " <<
                                "Caught unknown exception handling node "
                                << node_id);
            std::ostringstream err;
            err << node_id
                << " - unknown exception starting preparePlan";

            // report error result for scheduled reply
            reply->result ().set (NAM_Result::error (err.str ()));
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
          NAM_Result result;
          request.get (result);

          if (result.error_.empty ())
          {
            DANCEX11_LOG_DEBUG ("DomainApplication_Impl::destroyApplication - " <<
                                "destroyApplication successfully completed on node "
                                << result.node_);
          }
          else
          {
            errors.push_back (result.error_);
            failed = true;
          }
        }

        if (failed)
        {
          DANCEX11_LOG_ERROR ("DomainApplication_Impl::destroyApplication - " <<
                              errors.size () << " errors captured from async "
                              "Node destroyApplication requests");
          std::ostringstream os;
          os << "captured destroyApplication errors:\n";
          for (std::string &err : errors)
            os << err << "\n";
          throw Deployment::StopError (this->planUUID_, os.str ());
        }

        DANCEX11_LOG_DEBUG ("DomainApplication_Impl::destroyApplication - " <<
                      "destroyApplication has been called on node application manager");
      }
      catch (const CORBA::Exception& ex)
      {
        DANCEX11_LOG_ERROR ("DomainApplication_Impl::destroyApplication - " <<
                      "DomainApplication[" << this << "] caught a CORBA exception"
                      << " : " << ex << "");
        std::ostringstream err;
        err << this->planUUID_
            << " - CORBA exception handling destroyApplication"
            << " : " << ex;
        throw Deployment::StopError (this->planUUID_, err.str ());
      }
      catch (...)
      {
        DANCEX11_LOG_ERROR ("DomainApplication_Impl::destroyApplication - " <<
                            "DomainApplication[" << this << "] caught an unknown exception");
        std::ostringstream err;
        err << this->planUUID_
            << " - unknown exception handling destroyApplication";
        throw Deployment::StopError (this->planUUID_, err.str ());
      }
    }
    //else  no node applications to clean up so finish immediately
    DANCEX11_LOG_DEBUG ("DomainApplication_Impl::destroyApplication - " <<
                        "DomainApplication_Impl::destroyApplication - finished");
  }

  std::string
  DomainApplication_Impl::getPlanUUID ()
    {
      return this->planUUID_;
    }

  std::string
  DomainApplication_Impl::findNodeTApp2Id (
      IDL::traits< ::Deployment::Application >::ref_type app)
  {
    for (TApp2Id::iterator node_iter = this->app_node_ids_.begin();
        node_iter != this->app_node_ids_.end();
        ++node_iter)
    {
      if (node_iter->first->_is_equivalent(app))
      {
        return node_iter->second;
      }
    }
    return {};
  }
} // DAnCE

