// -*- C++ -*-
/**
 * @file dancex11_ndh_manager_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 * @brief  Implementation of Deployment::NodeManager
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_NDH_MANAGER_IMPL_H_
#define DANCEX11_NDH_MANAGER_IMPL_H_

#include "dancex11/core/dancex11_nodemanagerS.h"
#include "dancex11/core/dancex11_utility.h"
#include "dancex11/scheduler/events/plugin_manager.h"
#include "dancex11_ndh_application_manager_impl.h"

#include <map>

namespace DAnCEX11
{
  class NodeManager_Impl :
    public virtual CORBA::servant_traits< DAnCEX11::NodeManager>::base_type
  {
  public:
    /// The fixed listener port is caused by the usage of CCM Object locator
    NodeManager_Impl (IDL::traits<PortableServer::POA>::ref_type poa,
                      std::string name,
                      std::string domain_nc,
                      std::shared_ptr<Plugin_Manager> plugins,
                      IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh);

    virtual ~NodeManager_Impl();

    void joinDomain (const ::Deployment::Domain & theDomain,
                     IDL::traits< ::Deployment::TargetManager>::ref_type manager,
                     IDL::traits< ::Deployment::Logger>::ref_type log,
                     int32_t updateInterval) override;

    void leaveDomain () override;

    IDL::traits< ::Deployment::ApplicationManager>::ref_type preparePlan (
            const ::Deployment::DeploymentPlan & plan,
            IDL::traits< ::Deployment::ResourceCommitmentManager>::ref_type resourceCommitment) override;

    void destroyManager (
            IDL::traits< ::Deployment::ApplicationManager>::ref_type appManager) override;

    ::Deployment::Resources getDynamicResources () override;

    void shutdown () override;

  private:
    IDL::traits<PortableServer::POA>::ref_type poa_;
    IDL::traits< ::DAnCEX11::ShutdownHandler>::ref_type sh_;
    std::string name_;
    std::string domain_nc_;
    typedef std::map<std::string, CORBA::servant_reference<NodeApplicationManager_Impl>> TManagers;
    TManagers managers_;
    std::shared_ptr<Plugin_Manager> plugins_;
  };
}

#endif /* DANCEX11_NDH_MANAGER_IMPL_H_ */
