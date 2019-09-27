/**
 * @file    config_test.cpp
 * @author  Martin Corino
 *
 * @brief   Primary tester for DAnCEX11 Configurator
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11_config_loader.h"

#include <fstream>
#include <iostream>

//X11_FUZZ: disable check_cout_cerr

int main(int /*argc*/, char* argv[])
{
  DAnCEX11::Config_Loader cfg;
  Deployment::DeploymentPlan plugins, components;

  if (cfg.load_deployment_config (argv[1], plugins, components))
  {
    std::cout << "Plugin plan:" << std::endl <<
                 IDL::traits <Deployment::DeploymentPlan>::write (plugins) << std::endl
              << "-----=====-----" << std::endl
              << "Component plan:" << std::endl <<
                 IDL::traits <Deployment::DeploymentPlan>::write (components) << std::endl
              << "-----=====-----" << std::endl;
  }
  else
  {
    std::cerr << "Loading deployment config [" << argv[1] << "] FAILED!" << std::endl;
  }

  return 0;
}

//X11_FUZZ: enable check_cout_cerr
