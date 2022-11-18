/**
 * @file   dancex11_ddh_application_impl.h
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  Domain application
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_DDH_APPLICATION_IMPL_H
#define DANCEX11_DDH_APPLICATION_IMPL_H

#pragma once

#include "dancex11/deployment/deployment_domainapplicationS.h"
#include "dancex11/deployment/deployment_nodeapplicationmanagerC.h"
#include "dancex11/deployment/deployment_nodeapplicationmanagerS.h"
#include "dancex11/deployment/deployment_nodemanagerC.h"

#include <map>
#include <unordered_map>
#include <set>

namespace DAnCEX11
  {
  class DomainApplication_Impl;

  class DomainApplication_Impl
  : public virtual CORBA::servant_traits< ::Deployment::DomainApplication>::base_type
    {
    public:
      using TNm2Id_PAIR = std::pair <IDL::traits< ::Deployment::NodeManager>::ref_type , std::string>;
      using TNam2Nm_PAIR = std::pair <IDL::traits< ::Deployment::NodeApplicationManager>::ref_type,IDL::traits< ::Deployment::NodeManager>::ref_type>;
      using TApp2Mgr_PAIR = std::pair <IDL::traits< ::Deployment::Application>::ref_type, IDL::traits< ::Deployment::NodeApplicationManager>::ref_type>;
      using TApp2Id_PAIR = std::pair <IDL::traits< ::Deployment::Application>::ref_type, std::string>;

      using TNam2Nm = std::vector <TNam2Nm_PAIR>;
      using TNm2Id = std::vector <TNm2Id_PAIR>;
      using TApp2Mgr = std::vector <TApp2Mgr_PAIR>;
      using TApp2Id = std::vector <TApp2Id_PAIR>;

      DomainApplication_Impl (const std::string& plan_uuid,
                              IDL::traits<PortableServer::POA>::ref_type poa,
                              TNam2Nm& nams,
                              TNm2Id& node_ids);

      ~DomainApplication_Impl() override;

      void startLaunch (const ::Deployment::Properties & configProperty,
                        ::Deployment::Connections& providedReference);

      void destroyApplication ();

      std::string getPlanUUID ();

      /**
      * The second step in launching an application in the
      * domain-level.  If the start parameter is true, the application
      * is started as well.  Raises the StartError exception if
      * launching or starting the application fails.
      */
      void
      finishLaunch (const ::Deployment::Connections & connections,
                    bool start) override;
      /**
      * Starts the application. Raises the StartError exception if
      * starting the application fails.
      */
      void start () override;

      /**
      * Stops the application. Raises the StopError exception if
      * stopping the application fails.
      */
      void stop () override;

    private:
      std::string findNodeTApp2Id(IDL::traits< ::Deployment::Application>::ref_type app);

      IDL::traits<PortableServer::POA>::ref_type poa_;
      const std::string planUUID_;
      TApp2Mgr node_applications_;
      TApp2Id app_node_ids_;
      TNam2Nm nams_;
      TNm2Id node_ids_;
    };

} // DAnCEX11

#endif /* DANCEX11_DDH_APPLICATION_IMPL_H */
