// -*- C++ -*-
/**
 * @file   dancex11_execution_manager_impl.h
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>, Martin Corino
 * @brief  Implementation of Execution manager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_EXECUTIONMANAGER_IMPL_H_
#define DANCEX11_EXECUTIONMANAGER_IMPL_H_

#include "dancex11/core/dancex11_executionmanagerS.h"
#include "dancex11_ddh_application_manager_impl.h"
#include "dancex11_node_locator.h"

#include <map>

namespace DAnCEX11
{
  class ExecutionManager_Impl
   : public virtual CORBA::servant_traits< DAnCEX11::ExecutionManager>::base_type
  {
    public:
      ExecutionManager_Impl (IDL::traits<PortableServer::POA>::ref_type poa,
                             std::string name,
                             std::string nc,
                             IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh);

      virtual ~ExecutionManager_Impl ();

      virtual IDL::traits< ::Deployment::ApplicationManager>::ref_type
      preparePlan (const ::Deployment::DeploymentPlan& plan,
                   IDL::traits< ::Deployment::ResourceCommitmentManager>::ref_type resourceCommitment) override;

      virtual ::Deployment::DomainApplicationManagers getManagers () override;
      virtual void destroyManager (IDL::traits< ::Deployment::ApplicationManager>::ref_type manager) override;
      void add_node_manager (const std::string& name, const std::string& ior);

      void load_cdd (const std::string &filename, const std::string& = {});

      virtual void shutdown () override;

      typedef std::map<std::string, CORBA::servant_reference<DomainApplicationManager_Impl>>
        TDomainManagers;
      IDL::traits<PortableServer::POA>::ref_type poa_;
      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh_;
      std::string name_;
      TDomainManagers managers_;
      Node_Locator locator_;
  };

}; //DAnCEX11

#endif /*DANCEX11_EXECUTIONMANAGER_IMPL_H_*/
