/**
 * @file    plugin_conf.cpp
 * @author  Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "plugin_conf.h"

#include "ace/DLL.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_propertiesC.h"

#include "inst_handler_impl.h"
#include "interceptor_handler_impl.h"
#include "config_handler_impl.h"
#include "service_object_handler_impl.h"

#include <vector>
#include <fstream>

namespace DAnCEX11
{
  void
  Plugin_Configurator::load_from_plan (
      Plugin_Manager& plugins,
      const Deployment::DeploymentPlan &plan)
  {
    DANCEX11_LOG_TRACE ("Plugin_Configurator::load_from_plan");

    DAnCEX11::Inst_Handler_Impl inst_handler (plugins);
    DAnCEX11::Interceptor_Handler_Impl interceptor_handler (plugins);
    DAnCEX11::Config_Handler_Impl config_handler (plugins);
    DAnCEX11::Service_Object_Handler_Impl svc_object_handler (plugins);

    // configure plugin manager with plan info properties
    plugins.add_configuration (plan.infoProperty ());

    uint32_t i {};
    for (const Deployment::InstanceDeploymentDescription& idd: plan.instance ())
    {
      if (idd.implementationRef () >= plan.implementation ().size ())
      {
        DANCEX11_LOG_ERROR ("Plugin_Configurator::deploy_plan - " <<
                            "Invalid implementationRef for instance <" <<
                            idd.name () << ">");
      }
      else
      {
        const Deployment::MonolithicDeploymentDescription &mdd =
            plan.implementation ()[idd.implementationRef ()];

        std::string inst_type =
          DAnCEX11::Utility::get_instance_type (mdd.execParameter ());

        if (inst_type == DAnCEX11::DANCE_INSTALLATIONHANDLER)
        {
          CORBA::Any any;
          inst_handler.install_instance (plan, i, any);
        }
        else if (inst_type == DAnCEX11::DANCE_DEPLOYMENTINTERCEPTOR)
        {
          CORBA::Any any;
          interceptor_handler.install_instance (plan, i, any);
        }
        else if (inst_type == DAnCEX11::DANCE_CONFIGPLUGIN)
        {
          CORBA::Any any;
          config_handler.install_instance (plan, i, any);
        }
        else if (inst_type == DAnCEX11::DANCE_SERVICEOBJECT)
        {
          CORBA::Any any;
          svc_object_handler.install_instance (plan, i, any);
        }
      }
      ++i;
    }
  }
}
