// -*- C++ -*-
/**
 * @file locality_activator_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DAnCEX11_LOCALITY_ACTIVATOR_H_
#define DAnCEX11_LOCALITY_ACTIVATOR_H_

#pragma once


#include "ace/Process_Manager.h"
#include "ace/Event_Handler.h"
#include "ace/Condition_T.h"
#include "ace/OS_NS_sys_wait.h"
#include "ace/Synch_Traits.h"
#include "ace/Condition_T.h"
#include "dancex11/core/dancex11_localitymanagerS.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/core/dancex11_deploymentmanagerhandlerC.h"
#if defined (ACE_WIN32)
# include "ace/Task.h"
#endif

#include <set>

namespace DAnCEX11
{
  /**
   * @author Martin Corino <mcorino@remedy.nl>
   * @brief  Server activator for CIAOX11 locality managers.
   *
   * Implements the locality manager activation strategy
   * which is to spawn _new_ processes.  This is not thread-safe,
   * nor is it intended to be.  Containers are created serially,
   * so there will be only one actor *modifying* data at a particular
   * point in time.
   */
  class DAnCE_LocalityActivator_i final
    : public virtual CORBA::servant_traits< ::DAnCEX11::LocalityManagerActivator>::base_type
  {
  public:
    /// Constructor
    DAnCE_LocalityActivator_i (Deployment::Properties props,
                               bool multithreaded,
                               IDL::traits< CORBA::ORB>::ref_type orb,
                               IDL::traits< PortableServer::POA>::ref_type poa_);

    /// Destructor
    virtual ~DAnCE_LocalityActivator_i ();

    virtual
      void locality_manager_callback (
          IDL::traits< ::DAnCEX11::LocalityManager>::ref_type serverref,
          const std::string & server_UUID,
          ::Deployment::Properties& config) override;

    virtual void configuration_complete (const std::string &server_UUID) override;

    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
    create_locality_manager (
          const ::Deployment::DeploymentPlan &plan,
          uint32_t instanceRef,
          const ::Deployment::Properties & config);

    void remove_locality_manager (
          const ::Deployment::DeploymentPlan &plan);

    ::DAnCEX11::LocalityManager * get_locality_managers ();

  private:
    struct Server_Info;
    class Server_Child_Handler;

    struct Server_Info
    {
      enum ProcessStatus
        {
          ACTIVE,
          TERMINATE_REQUESTED,
          TERMINATED,
          INACTIVE
        };

      Server_Info (const ::Deployment::DeploymentPlan &plan,
                   uint32_t instanceRef)
        : condition_ (mutex_),
          plan_ (plan),
          plan_UUID_ (plan.UUID ()),
          instanceRef_ (instanceRef)
      {}
      ~Server_Info ();

      std::string uuid_;
      DAnCEX11::Utility::PROPERTY_MAP cmap_;
      IDL::traits< ::DAnCEX11::LocalityManager>::ref_type ref_;
      uint32_t activation_mode_ {};
      pid_t pid_ {ACE_INVALID_PID};
      ProcessStatus status_ {INACTIVE};
      ACE_SYNCH_MUTEX mutex_;
      ACE_Condition<ACE_SYNCH_MUTEX> condition_;
      const ::Deployment::DeploymentPlan &plan_;
      std::string plan_UUID_;
      uint32_t instanceRef_ {};
      IDL::traits<DAnCEX11::DeploymentManagerHandler>::ref_type dmh_;
    };

    typedef std::shared_ptr<Server_Info> Safe_Server_Info;

    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
    activate_remote_locality (Safe_Server_Info ssi);

    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
    activate_local_locality (Safe_Server_Info ssi);

    IDL::traits< ::DAnCEX11::LocalityManager>::ref_type
    activate_child_locality (Safe_Server_Info ssi);

    /// Builds command line options based on configuration information.
    /// May modify the uuid of the component server.
    std::string construct_command_line (Server_Info &si);

    /// Spawns the component server process, but does not wait for it
    /// to call back.
    pid_t spawn_locality_manager (std::unique_ptr<Server_Child_Handler> exit_handler,
                                  const std::string &cmd_line);

    /// This method is only applicable when our program is configured as
    /// singled threaded . Internally it uses a @c perform_work blocking
    /// call to wait for NA object to call back
    void single_threaded_wait_for_callback (const Server_Info &si,
                                            ACE_Time_Value &timeout);

    /// This method is only applicable when our program is configured as
    /// multiple threaded. Internally it waits on a conditional variable
    /// that could be modified by the callback servant which runs in
    /// another thread
    void multi_threaded_wait_for_callback (Server_Info &si,
                                           ACE_Time_Value &timeout);

    /**
    * @brief The _exit_ handler class for the locality manager child process
    * to detect and report process _exit_
    */
    class Server_Child_Handler : public virtual ACE_Event_Handler
    {
    public:
      Server_Child_Handler (Safe_Server_Info  si);
      virtual ~Server_Child_Handler ();

      virtual int handle_close (ACE_HANDLE, ACE_Reactor_Mask) override;

      virtual int handle_exit (ACE_Process *proc) override;

      const Server_Info& server_info () const
      {
        return *this->server_info_;
      }
    private:
      Safe_Server_Info server_info_;
    };

    struct _server_info
    {
      bool operator() (const Safe_Server_Info &a, const Safe_Server_Info &b) const
      {
        return a->uuid_ < b->uuid_;
      }
    };

    // Presumably, there won't be too many component servers per node application
    typedef std::set <Safe_Server_Info, _server_info> SERVER_INFOS;

    TAO_SYNCH_MUTEX container_mutex_;

    SERVER_INFOS server_infos_;

    ACE_Process_Manager process_manager_;
    // On Windows the detection of child exits through the ORBs
    // reactor is tricky/unreliable so we need a less elegant
    // approach.
#if defined (ACE_WIN32)
    class Watchdog : public ACE_Task_Base
    {
    public:
      Watchdog (ACE_Process_Manager&);
      ~Watchdog () = default;

      virtual int svc () override;

      bool start ();

      void stop ();
    private:
      bool stop_ {};
      ACE_Process_Manager &procman_;
    };
    Watchdog process_watcher_;
#endif

    /////*******NEW
    bool multithreaded_;

    IDL::traits<CORBA::ORB>::ref_type orb_;

    IDL::traits<PortableServer::POA>::ref_type poa_;

    Deployment::Properties properties_;

    TAO_SYNCH_MUTEX mutex_;

    ACE_Condition<TAO_SYNCH_MUTEX> condition_;
  };
}
#endif /* DAnCEX11_LOCALITY_ACTIVATOR_H_ */
