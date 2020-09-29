// -*- C++ -*-
/**
 * @file   dancex11_ndh_application_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 * @brief  Implementation of Deployment::NodeApplication
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_NDH_APPLICATION_IMPL_H_
#define DANCEX11_NDH_APPLICATION_IMPL_H_

#include "dancex11/deployment/deployment_nodeapplicationS.h"
#include "dancex11/deployment/deployment_deploymentplanC.h"
#include "dancex11/deployment/deploymentC.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/core/dancex11_localitymanagerC.h"
#include "dancex11/scheduler/events/plugin_event_scheduler.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "tools/split_plan/locality_splitter.h"
#include "tools/split_plan/split_plan.h"

#include <map>
#include <vector>

using namespace DAnCEX11::Utility;

namespace DAnCEX11
{

  using LocalitySplitter = DAnCEX11::Split_Plan <DAnCEX11::Locality_Splitter>;

  class NodeManager_Impl;

  class NodeApplication_Impl :
    public virtual CORBA::servant_traits< ::Deployment::NodeApplication>::base_type
  {
  public:
    NodeApplication_Impl (IDL::traits<PortableServer::POA>::ref_type poa,
                          std::string node_name,
                          Plugin_Event_Scheduler_T<Deployment_Scheduler> &scheduler);

    virtual ~NodeApplication_Impl();

    void finishLaunch (const ::Deployment::Connections & providedReference,
                       bool start) override;

    void start () override;

    void stop () override;

    void prepare_instances (const LocalitySplitter::TSubPlans & plans);

    void start_launch_instances (const Deployment::Properties & prop,
                                 Deployment::Connections & providedReference);

    void remove_instances ();

    void shutdown_instances ();

    using LOCALITY_MAP = std::map <std::string, IDL::traits< ::DAnCEX11::LocalityManager>::ref_type>;

    typedef std::pair <uint32_t, ::Deployment::DeploymentPlan> SUB_PLAN;
    typedef std::map <std::string, SUB_PLAN> PLAN_MAP;

  protected:
    void prepare_instance (const std::string & name,
                           const ::Deployment::DeploymentPlan &plan);

    IDL::traits<PortableServer::POA>::ref_type poa_;

    std::string node_name_;

    LOCALITY_MAP localities_;

    Plugin_Event_Scheduler_T<Deployment_Scheduler> &scheduler_;

    PLAN_MAP sub_plans_;

    uint32_t spawn_delay_;
  };
}

#endif /* DANCEX11_NDH_APPLICATION_IMPL_H_ */
