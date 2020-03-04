// -*- C++ -*-
/**
 * @file   dancex11_ddh_application_manager_impl.h
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  Implementation of domain application manager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DDH_APPLICATIONMANAGER_IMPL_H_
#define DANCEX11_DDH_APPLICATIONMANAGER_IMPL_H_

#include "dancex11_node_locator.h"
#include "dancex11/deployment/deployment_nodemanagerC.h"
#include "dancex11/deployment/deployment_nodemanagerS.h"
#include "dancex11_ddh_application_impl.h"
#include "dancex11/deployment/deployment_domainapplicationmanagerS.h"
#include "tools/split_plan/node_splitter.h"
#include "tools/split_plan/split_plan.h"

#include <map>

namespace DAnCEX11
{
  class DomainApplicationManager_Impl;

  typedef DAnCEX11::Split_Plan < DAnCEX11::Node_Splitter > NodeSplitter;

  class DomainApplicationManager_Impl
  : public virtual CORBA::servant_traits< ::Deployment::DomainApplicationManager>::base_type
    {
    public:
      DomainApplicationManager_Impl (IDL::traits<PortableServer::POA>::ref_type poa,
                                     ::Deployment::DeploymentPlan plan,
                                     Node_Locator &nodes);

      virtual ~DomainApplicationManager_Impl();

      IDL::traits< ::Deployment::Application>::ref_type
      startLaunch (const ::Deployment::Properties& configProperty,
                   ::Deployment::Connections& providedReference) override;

      void destroyApplication (IDL::traits< ::Deployment::Application>::ref_type app) override;

      std::string getPlanUUID ();

      void preparePlan ();

      void destroyManager ();

      ::Deployment::Applications getApplications () override;

      ::Deployment::DeploymentPlan getPlan () override;

    private:
      const std::string
      findNode4NM (IDL::traits< ::Deployment::NodeManager >::ref_type nm);

      IDL::traits<PortableServer::POA>::ref_type poa_;
      Deployment::DeploymentPlan plan_;
      DomainApplication_Impl::TNam2Nm sub_app_mgr_;
      DomainApplication_Impl::TNm2Id node_ids_;
      Node_Locator &nodes_;
      typedef std::vector< CORBA::servant_reference<DomainApplication_Impl> > TApplications;
      TApplications running_app_;
      NodeSplitter split_plan_;
    };
  } //DAnCEX11

#endif /* DANCEX11_DDH_APPLICATIONMANAGER_IMPL_H_ */
