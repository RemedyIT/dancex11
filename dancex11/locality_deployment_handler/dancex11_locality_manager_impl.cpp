// -*- C++ -*-
/**
 * @file    dancex11_locality_manager_impl.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "dancex11_locality_manager_impl.h"
#include "dancex11/logger/log.h"

#include "dancex11/core/dancex11_utility.h"
#include "dancex11/core/dancex11_propertiesC.h"
#include "dancex11/core/dancex11_deploymentconfiguratorsC.h"
#include "dancex11/core/dancex11_deployment_state.h"
#include "dancex11/scheduler/deployment_completion.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11/scheduler/events/install.h"
#include "dancex11/scheduler/events/connect.h"
#include "dancex11/scheduler/events/endpoint.h"
#include "dancex11/scheduler/events/start.h"
#include "dancex11/scheduler/events/configured.h"
#include "dancex11/scheduler/events/remove.h"
#include "dancex11/scheduler/events/passivate.h"
#include "dancex11/scheduler/events/disconnect.h"
#include "ace/CORBA_macros.h"

namespace DAnCEX11
{
  // Implementation skeleton constructor
  LocalityManager_i::LocalityManager_i (const std::string &uuid,
    std::shared_ptr<Plugin_Manager> plugins,
    IDL::traits<PortableServer::POA>::ref_type poa,
    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh)
    : uuid_ (uuid)
    , poa_ (std::move (poa))
    , sh_ (sh)
    , scheduler_ (plugins)
    , spawn_delay_ (30)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::LocalityManager_i");

    this->scheduler_.activate_scheduler (0);
  }

  LocalityManager_i::~LocalityManager_i ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::~LocalityManager_i");

    this->scheduler_.terminate_scheduler ();
  }

  void
  LocalityManager_i::configure (const Deployment::Properties &props)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::configure");

    DANCEX11_LOG_DEBUG ("LocalityManager_i::configure - " <<
                        "Received " << props.size () << " properties from init");

    if (props.size () != 0)
    {
      if (DAnCEX11::Utility::get_property_value (DAnCEX11::LOCALITY_TIMEOUT,
                                              props,
                                              this->spawn_delay_))
      {
        DANCEX11_LOG_DEBUG ("LocalityManager_i::configure - " <<
                            "Using provided spawn delay <" << this->spawn_delay_ << ">");
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::configure - " <<
                          "Number of LM configuration properties: " << props.size ());

      for (Deployment::Property const &property : props)
      {
        DANCEX11_LOG_DEBUG ("LocalityManager_i::configure - " <<
                            "Looking up configuration handler for <" <<
                            property.name () << ">");

        IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type config =
          this->scheduler_.plugins ().get_configuration_handler (property.name ());

        if (config)
        {
          DANCEX11_LOG_DEBUG ("LocalityManager_i::configure - " <<
                              "Invoking configuration handler for <" <<
                              property.name () << ">");
          config->configure (property);
        }
      }
    }
    else
    {
      DANCEX11_LOG_WARNING ("LocalityManager_i::configure - " <<
                            "Warning: No configuration properties");
    }
  }

  Deployment::Properties
  LocalityManager_i::configuration ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::configuration");
    throw CORBA::NO_IMPLEMENT ();
  }

  IDL::traits< Deployment::ApplicationManager>::ref_type
  LocalityManager_i::preparePlan (const Deployment::DeploymentPlan &plan,
    IDL::traits< Deployment::ResourceCommitmentManager>::ref_type)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::preparePlan");

    this->plan_ = plan;

    Plugin_Manager::INTERCEPTORS interceptors =
        this->scheduler_.plugins ().fetch_interceptors ();

    for (IDL::traits<DAnCEX11::DeploymentInterceptor>::ref_type di : interceptors)
    {
      if (di)
      {
        di->preprocess_plan (this->plan_);
      }
    }

    // populate the impl_type_table.
    for (Deployment::InstanceDeploymentDescriptions::size_type i = 0;
      i < this->plan_.instance ().size ();
      i++)
    {
      uint32_t const implRef = this->plan_.instance ()[i].implementationRef ();

      std::string const &inst_type =
        Utility::get_instance_type (
          this->plan_.implementation ()[implRef].execParameter ());

      DANCEX11_LOG_DEBUG ("LocalityManager_i::preparePlan - " <<
                          "Storing installation handler <" << inst_type << ">");

      if (this->instance_handlers_.find (inst_type) == this->instance_handlers_.end ())
      {
        INSTANCE_LIST list;
        this->instance_handlers_.insert (HANDLER_TABLE_PAIR (inst_type, list));
      }

      this->instance_handlers_[inst_type].push_back (
          ACE_Utils::truncate_cast<uint32_t> (i));
    }

    return this->_this ();
  }

  IDL::traits< Deployment::Application>::ref_type
  LocalityManager_i::startLaunch (const Deployment::Properties &,
    Deployment::Connections &providedReference)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::startLaunch");

    this->scheduler_.plugins ().get_installation_order (this->handler_order_);

    this->install_instances ();

    this->collect_references (providedReference);

    return this->_this ();
  }

  void
  LocalityManager_i::install_instances ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::install_instances");

    uint32_t dispatched {};

    Deployment_Completion completion (*this->scheduler_);

    for (std::string const &order : this->handler_order_)
    {
      DANCEX11_LOG_DEBUG ("LocalityManager_i::install_instances - " <<
                          "Starting installation of <" << order << "> type instances");

      INSTANCE_LIST inst_list = this->instance_handlers_[order];

      for (uint32_t idd : inst_list)
      {
        DANCEX11_LOG_DEBUG ("LocalityManager_i::install_instances - " <<
                            "Starting installation of instance <" <<
                            this->plan_.instance ()[idd].name () << ">");

        Event_Future result;
        completion.accept (result);

        this->scheduler_.schedule_event<Install_Instance> (
            this->plan_,
            idd,
            order,
            result);

        ++dispatched;
      }
    }

    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

    if (!completion.wait_on_completion (std::addressof(tv)))
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::install_instances - " <<
                          "Timed out while waiting on completion of scheduler");
    }

    tv = ACE_Time_Value::zero;

    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::install_instances - " <<
                          "Received only " << dispatched <<
                          " completed events, expected " << completed_events.size ());
    }

    for (DAnCEX11::Event_Future ev :  completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::install_instances - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StartError >
            (event.contents_) ||
            DAnCEX11::Utility::extract_and_throw_exception< Deployment::InvalidProperty >
            (event.contents_) ||
            DAnCEX11::Utility::extract_and_throw_exception< Deployment::InvalidNodeExecParameter >
            (event.contents_) ||
            DAnCEX11::Utility::extract_and_throw_exception< Deployment::InvalidComponentExecParameter >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::install_instances - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StartError (event.id_, "Unknown exception");
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::install_instances - " <<
                          "Instance <" << event.id_ << "> successfully deployed");
    }
  }

  void
  LocalityManager_i::collect_references (Deployment::Connections &providedReference)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::collect_references");

    uint32_t dispatched {};
    Deployment_Completion completion (*this->scheduler_);

    for (Deployment::PlanConnectionDescriptions::size_type i = 0;
      i < this->plan_.connection ().size ();
      ++i)
    {
      Deployment::PlanConnectionDescription const &conn =
        this->plan_.connection ()[i];

      // connections with external (reference) endpoints
      // we do not know how to resolve here; we just
      // collect them and allow connect handlers/interceptors
      // to handle them later; the peer endpoint should be an
      // internal endpoint for which we do not need to retrieve
      // a reference as this will become the connecting endpoint
      // which is accessed locally
      if (conn.externalReference ().size () > 0 ||
          conn.externalEndpoint ().size () > 0 )
      {
        Deployment::Connection con;
        con.name (conn.name ());
        con.endpoint ().push_back (nullptr);
        providedReference.push_back (con);
      }
      else
      {
        for (Deployment::PlanSubcomponentPortEndpoint const &ep :
          conn.internalEndpoint ())
        {
          // only collect references for provider (facet) endpoints as
          // using (receptacle) endpoints will always be accessed locally
          // in the locality where they are deployed
          if (ep.provider ())
          {
            uint32_t const implRef =
              this->plan_.instance ()[ep.instanceRef ()].implementationRef ();

            std::string const &inst_type =
              Utility::get_instance_type (
                this->plan_.implementation ()[implRef].execParameter ());

            Event_Future result;
            completion.accept (result);

            this->scheduler_.schedule_event<Endpoint_Reference> (
                this->plan_,
                ACE_Utils::truncate_cast<uint32_t> (i),
                inst_type,
                result);

            ++dispatched;
          }
        }
      }
    }

    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

    if (!completion.wait_on_completion (std::addressof(tv)))
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::collect_references - " <<
                          "Timed out while waiting on completion of scheduler");
    }

    tv = ACE_Time_Value::zero;

    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::collect_references - " <<
                          "Received only " << dispatched <<
                          " completed events, expected " << completed_events.size ());
    }

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::collect_references - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StartError >
            (event.contents_) ||
            DAnCEX11::Utility::extract_and_throw_exception< Deployment::InvalidProperty >
            (event.contents_)))
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::collect_references - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StartError (event.id_, "Unknown exception");
      }

      IDL::traits<CORBA::Object>::ref_type obj_ref;

      if (event.contents_.type()->kind () != CORBA::TCKind::tk_null)
      {
        event.contents_ >>= obj_ref;
      }
      else
      {
        DANCEX11_LOG_DEBUG ("LocalityManager_i::collect_references - " <<
                            "No reference returned for connection <" << event.id_ << ">");
      }

      Deployment::Connection con;
      con.name (event.id_);
      con.endpoint ().push_back (obj_ref);
      providedReference.push_back (con);
    }
  }

  void
  LocalityManager_i::finishLaunch (const Deployment::Connections &providedReferences,
    bool start)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::finishLaunch");

    typedef std::map< std::string, uint32_t > ConnMap;
    ConnMap conns;

    Deployment_Completion completion (*this->scheduler_);

    DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                        "Starting finishLaunch, received " <<
                        providedReferences.size () << " references, "
                        "have " << this->plan_.connection ().size () <<
                        " connections");

    for (Deployment::PlanConnectionDescriptions::size_type i = 0;
      i < this->plan_.connection ().size ();
      ++i)
    {
      conns[this->plan_.connection ()[i].name ()] =
          ACE_Utils::truncate_cast<uint32_t> (i);
    }

    uint32_t dispatched {};

    for (Deployment::Connection const &connection : providedReferences)
    {
      ConnMap::const_iterator conn_ref = conns.find (connection.name ());

      if (conn_ref == conns.end ())
        continue;

      uint32_t j (0);

      Deployment::PlanConnectionDescription const &conn =
        this->plan_.connection ()[conn_ref->second];

      DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                          "Connection <" << conn.name () << "> has " <<
                          (conn.internalEndpoint ().size () +
                              conn.externalReference ().size () +
                              conn.externalEndpoint ().size ()) << " endpoints");

      if (conn.internalEndpoint ().size () == 2)
      {
        // make sure to execute the connect on the receptacle endpoint
        if (!conn.internalEndpoint ()[1].provider ())
          j = 1;
      }
      else if (conn.internalEndpoint ().size () == 1 &&
                (conn.externalReference ().size () + conn.externalEndpoint ().size ()) == 0 &&
                conn.internalEndpoint ()[0].kind () == ::Deployment::CCMComponentPortKind::Facet)
      {
        DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                            "Skipping facet end of intra-plan connection <" << conn.name () << ">");
        continue;
      }
      else if (conn.internalEndpoint ().size () != 1 ||
                (conn.externalReference ().size () + conn.externalEndpoint ().size ()) > 1)
      {
        DANCEX11_LOG_INFO ("LocalityManager_i::finishLaunch - " <<
                           "Skipping unsupported connection <" << conn.name () << ">");
        continue;
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                          "Starting connection <" << conn.name () << ">");

      CORBA::Any reference;

      reference <<= connection.endpoint ()[0];

      uint32_t const instRef =
        conn.internalEndpoint ()[j].instanceRef ();
      uint32_t const implRef =
        this->plan_.instance ()[instRef].implementationRef ();
      std::string const &inst_type =
        Utility::get_instance_type (
          this->plan_.implementation ()[implRef].execParameter ());

      Event_Future result;
      completion.accept (result);

      this->scheduler_.schedule_event<Connect_Instance> (
          this->plan_,
          conn_ref->second,
          reference,
          inst_type,
          result);

      ++dispatched;
    }

    // @todo, risky in terms of time shift
    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

    if (!completion.wait_on_completion (std::addressof(tv)))
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                            "Timed out while waiting on completion of scheduler");
      }

    tv = ACE_Time_Value::zero;

    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                          "Received only " << dispatched << " completed events, expected " <<
                          completed_events.size ());
    }

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StartError >
            (event.contents_) ||
            DAnCEX11::Utility::extract_and_throw_exception< Deployment::InvalidConnection >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StartError (event.id_, "Unknown exception");
      }
    }

    dispatched = 0;

    for (std::string const &order : this->handler_order_)
    {
      INSTANCE_LIST const &inst_list = this->instance_handlers_[order];

      for (uint32_t inst : inst_list)
      {
        Event_Future result;
        completion.accept (result);

        this->scheduler_.schedule_event<Instance_Configured> (
            this->plan_,
            inst,
            order,
            result);

        ++dispatched;
      }
    }

    tv = ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_);

    if (!completion.wait_on_completion (std::addressof(tv)))
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                          "Timed out while waiting on completion of scheduler");
    }
    else
    {
      DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                          "events completed");
    }

    tv = ACE_Time_Value::zero;

    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                          "Received only " << dispatched << " completed events, expected " <<
                          completed_events.size ());
    }

    DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                        "checking " << completed_events.size () <<
                        " event results");

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StartError >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::finishLaunch - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StartError (event.id_,
          "Unknown exception from instance_configured");
      }
    }

    DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                        "finished checking " << completed_events.size () <<
                        " event results");

    if (start)
    {
      DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                          "calling start ()");

      this->start ();
    }

    DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                        "leaving");
  }

  void
  LocalityManager_i::disconnect_connections ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::disconnect_connections");

    Deployment_Completion completion (*this->scheduler_);

    DANCEX11_LOG_DEBUG ("LocalityManager_i::disconnect_connections - " <<
                        "Starting disconnect_connections, " <<
                        "have " << this->plan_.connection ().size () << " connections");

    uint32_t dispatched {};

    for (Deployment::PlanConnectionDescriptions::size_type i = 0;
        i < this->plan_.connection ().size ();
        ++i)
    {
      uint32_t j (0);

      Deployment::PlanConnectionDescription const &conn =
        this->plan_.connection ()[i];

      DANCEX11_LOG_DEBUG ("LocalityManager_i::disconnect_connections - " <<
                          "Connection <" << conn.name () << "> has " <<
                          conn.internalEndpoint ().size () << " endpoints");

      if (conn.internalEndpoint ().size () == 2)
      {
      // make sure to execute the connect on the receptacle endpoint
        if (!conn.internalEndpoint ()[1].provider ())
          j = 1;
      }
      else if (conn.internalEndpoint ().size () == 1 &&
                (conn.externalReference ().size () + conn.externalEndpoint ().size ()) == 0 &&
                conn.internalEndpoint ()[0].kind () == ::Deployment::CCMComponentPortKind::Facet)
      {
        DANCEX11_LOG_DEBUG ("LocalityManager_i::finishLaunch - " <<
                            "Skipping facet end of intra-plan connection <" << conn.name () << ">");
        continue;
      }
      else if (conn.internalEndpoint ().size () != 1 ||
          (conn.externalReference ().size () + conn.externalEndpoint ().size ()) > 1)
      {
        DANCEX11_LOG_INFO ("LocalityManager_i::disconnect_connections - " <<
                           "Skipping unsupported connection <" << conn.name () << ">");
        continue;
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::disconnect_connections - "
                          "Starting disconnect connection <" << conn.name () << ">");

      uint32_t const instRef =
        conn.internalEndpoint ()[j].instanceRef ();
      uint32_t const implRef =
        this->plan_.instance ()[instRef].implementationRef ();
      std::string const &inst_type =
        Utility::get_instance_type (
          this->plan_.implementation ()[implRef].execParameter ());

      Event_Future result;
      completion.accept (result);

      this->scheduler_.schedule_event<Disconnect_Instance> (
          this->plan_,
          ACE_Utils::truncate_cast<uint32_t> (i),
          inst_type,
          result);

      ++dispatched;
    }

    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

    if (!completion.wait_on_completion (std::addressof(tv)))
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::disconnect_connections - " <<
                            "Timed out while waiting on completion of scheduler");
      }

    tv = ACE_Time_Value::zero;

    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::disconnect_connections - " <<
                          "Received only " << dispatched << " completed events, expected " <<
                          completed_events.size ());
    }

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::disconnect_connections - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StartError >
            (event.contents_) ||
            DAnCEX11::Utility::extract_and_throw_exception< Deployment::InvalidConnection >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::disconnect_connections - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StartError (event.id_, "Unknown exception");
      }
    }
  }

  void
  LocalityManager_i::start ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::start");

    Deployment_Completion completion (*this->scheduler_);
    uint32_t dispatched {};

    for (std::string const &order : this->handler_order_)
    {
      INSTANCE_LIST const &inst_list = this->instance_handlers_[order];

      for (uint32_t inst : inst_list)
      {
        std::string const &name = this->plan_.instance ()[inst].name ();

        DANCEX11_LOG_DEBUG ("LocalityManager_i::start - " <<
                            "Scheduling start for instance <" << name << ">");

        Event_Future result;
        completion.accept (result);

        this->scheduler_.schedule_event<Start_Instance> (
            this->plan_,
            inst,
            order,
            result);

        ++dispatched;
      }
    }

    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

    if (!completion.wait_on_completion (std::addressof(tv)))
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::start - " <<
                          "Timed out while waiting on completion of scheduler\n");
    }

    tv = ACE_Time_Value::zero;

    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::start - " <<
                          "Received only " << dispatched << " completed events, expected " << completed_events.size ());
    }

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::start - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StartError >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::start - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StartError (event.id_, "Unknown exception");
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::start - " <<
                          "Instance <" << event.id_ << "> successfully activated");
    }
  }

  void
  LocalityManager_i::stop ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::stop");

    Deployment_Completion completion (*this->scheduler_);
    uint32_t dispatched {};

    for (Plugin_Manager::INSTALL_ORDER::reverse_iterator i = this->handler_order_.rbegin();
        i != this->handler_order_.rend ();
        ++i)
    {
      INSTANCE_LIST const &inst_list = this->instance_handlers_[*i];

      for (uint32_t inst : inst_list)
      {
        std::string const &name = this->plan_.instance ()[inst].name ();

        DANCEX11_LOG_DEBUG ("LocalityManager_i::stop - " <<
                            "Scheduling passivation for instance <" << name << ">");

        Event_Future result;
        completion.accept (result);

        this->scheduler_.schedule_event<Passivate_Instance> (
            this->plan_,
            inst,
            *i,
            result);

        ++dispatched;
      }
    }

    ACE_Time_Value tv (ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_));

    if (!completion.wait_on_completion (std::addressof(tv)))
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::stop - " <<
                          "Timed out while waiting on completion of scheduler");
    }

    tv = ACE_Time_Value::zero;

    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::stop - " <<
                          "Received only " << dispatched << " completed events, expected " <<
                          completed_events.size ());
    }

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::stop - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StopError >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::stop - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StopError (event.id_,
                                       "Unknown exception");
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::stop - " <<
                          "Instance <" << event.id_ << "> successfully passivated");
    }
  }

  void
  LocalityManager_i::destroyApplication (IDL::traits<Deployment::Application>::ref_type)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::destroyApplication");

    Deployment_Completion completion (*this->scheduler_);
    uint32_t dispatched {};

    // First disconnect all connections in the plan
    this->disconnect_connections ();

    for (Plugin_Manager::INSTALL_ORDER::reverse_iterator i = this->handler_order_.rbegin();
        i != this->handler_order_.rend ();
        ++i)
    {
      INSTANCE_LIST const &inst_list = this->instance_handlers_[*i];

      for (uint32_t inst : inst_list)
      {
        Event_Future result;
        completion.accept (result);

        this->scheduler_.schedule_event<Remove_Instance> (
            this->plan_,
            inst,
            *i,
            result);

        ++dispatched;
      }
    }

    ACE_Time_Value tv = ACE_OS::gettimeofday () + ACE_Time_Value (this->spawn_delay_);

    if (!completion.wait_on_completion (std::addressof(tv)))
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::destroyApplication - " <<
                          "Timed out while waiting on completion of scheduler");
    }

    tv = ACE_Time_Value::zero;


    Event_List completed_events;
    completion.completed_events (completed_events);

    if (completed_events.size () != dispatched)
    {
      DANCEX11_LOG_ERROR ("LocalityManager_i::destroyApplication - "
                          "Received only " << dispatched << " completed events, expected " << completed_events.size ());
    }

    dispatched = 0;

    for (DAnCEX11::Event_Future const &ev : completed_events)
    {
      Event_Result event;
      if (ev.get (event, &tv) != 0)
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::destroyApplication - " <<
                            "Failed to get future value for current instance");
        continue;
      }

      if (event.exception_ &&
          !(DAnCEX11::Utility::extract_and_throw_exception< Deployment::StopError >
            (event.contents_))
          )
      {
        DANCEX11_LOG_ERROR ("LocalityManager_i::destroyApplication - " <<
                            "Error: Unknown exception propagated");
        throw Deployment::StopError (event.id_,
                                       "Unknown exception");
      }

      DANCEX11_LOG_DEBUG ("LocalityManager_i::destroyApplication - " <<
                          "Instance <" << event.id_ << "> successfully removed");
    }
  }

  void
  LocalityManager_i::destroyManager (
    IDL::traits<Deployment::ApplicationManager>::ref_type)
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::destroyManager");
    // Add your implementation here
  }

  void
  LocalityManager_i::shutdown ()
  {
    DANCEX11_LOG_TRACE ("LocalityManager_i::shutdown");

    DANCEX11_LOG_DEBUG ("DAnCE LocalityManager shutdown request received for UUID <" << this->uuid_ << ">");

    // call shutdown handler
    if (this->sh_)
      this->sh_->shutdown ();

    if (DAnCEX11::State::instance ()->orb ())
    {
      DAnCEX11::State::instance ()->orb ()->shutdown (false);
    }
  }
}
