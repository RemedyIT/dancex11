/**
 * @file   dancex11_node_locator.h
 * @author Marijke Hengstmengel <mhengstmengel@remedy.nl>
 * @brief  locate nodes
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_NODE_LOCATOR_H_
#define DANCEX11_NODE_LOCATOR_H_

#include "orbsvcs/naming_server/CosNamingC.h"
#include "dancex11/deployment/deployment_nodemanagerC.h"

#include <map>

namespace DAnCEX11
{
  class Node_Locator
  {
  public:
    explicit Node_Locator (const std::string& nc);

    IDL::traits<Deployment::NodeManager>::ref_type locate_node (const std::string& node);

    bool process_cdd (const std::string &file, const std::string& = {});

    void store_ior (const std::string& name, const std::string& ior);

  private:
    IDL::traits<Deployment::NodeManager>::ref_type resolve_ior (const std::string& name,
                                               const std::string& ior);

    IDL::traits<Deployment::NodeManager>::ref_type ns_lookup (const std::string& name);

    bool get_resource_value (const std::string& type,
                             const ::Deployment::Resources &resources,
                             ::Deployment::Resource &val);

    typedef std::map<std::string, std::string> NODEMAP;

    NODEMAP nodes_;
    IDL::traits<CORBA::ORB>::ref_type orb_;
    IDL::traits<CosNaming::NamingContext>::ref_type nc_;
  };

}

#endif /* DANCEX11_NODE_LOCATOR_H_ */
