/**
 * @file    dancex11_locality_dmh_loader.cpp
 * @author  Martin Corino
 *
 * @brief   Service loader for Locality DMH implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_locality_dmh_loader.h"
#include "dancex11_locality_deployment_handler.h"

#include "ace/Get_Opt.h"
#include "ace/Service_Config.h"

#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  // Type to help declare the standardized factory name
  using DAnCEX11_DeploymentHandler_Impl = Locality_DMHandler_Loader;

  // Define the static Service descriptor.
  // Note the standardized name for the factory method declaration.
  ACE_STATIC_SVC_DEFINE (Locality_DMHandler_Loader,
                         ACE_TEXT ("DANCEX11_DEPLOYMENT_HANDLER"),
                         ACE_SVC_OBJ_T,
                         &ACE_SVC_NAME (DAnCEX11_DeploymentHandler_Impl),
                         ACE_Service_Type::DELETE_THIS
                          | ACE_Service_Type::DELETE_OBJ,
                         0)
  // Define implementation for Service factory method and cleanup method
  // (gobbler).
  // Instead of the real classname a type is used to force generation
  // of the standardized factory method name.
  ACE_FACTORY_DEFINE (DANCEX11_LOCALITY_DH, DAnCEX11_DeploymentHandler_Impl)

  int Locality_DMHandler_Loader::init (int argc, ACE_TCHAR *argv[])
  {
    ACE_Get_Opt get_opts (argc,
                          argv,
                          ACE_TEXT("u:r:h"),
                          0,
                          0,
                          ACE_Get_Opt::RETURN_IN_ORDER);

    get_opts.long_option (ACE_TEXT("uuid"), 'u', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("activator"), 'r', ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("help"), 'h', ACE_Get_Opt::NO_ARG);

    int c;
    while ( (c = get_opts ()) != -1)
    {
      switch (c)
      {
        case 'u':
          DANCEX11_LOG_DEBUG ("Locality_DMHandler_Loader::init - " <<
                              "Provided UUID: " << get_opts.opt_arg ());
          this->uuid_ = get_opts.opt_arg ();
          break;

        case 'r':
          DANCEX11_LOG_DEBUG ("Locality_DMHandler_Loader::init - " <<
                              "Provided Activator reference: " << get_opts.opt_arg ());
          this->activator_ior_ = get_opts.opt_arg ();
          break;

          //X11_FUZZ: disable check_cout_cerr
        case 'h':
          {
            std::cerr << "Locality Deployment Handler options: "
                      << "-- [--uuid UUID] [--activator ACTIVATOR] [-h]" << std::endl
                      << "\t-u|--uuid UUID\t\t\t\t Specify locality instance UUID" << std::endl
                      << "\t-r|--activator ACTIVATOR\t\t Specify Activator instance IOR" << std::endl
                      << "\t-h\t\t\t\t\t Show this info" << std::endl << std::endl;
            ACE_OS::exit (1);
          }
          break;
          //X11_FUZZ: enable check_cout_cerr

        case 0:
          if (ACE_OS::strcmp (get_opts.long_option (),
                              ACE_TEXT("uuid")) == 0)
          {
            DANCEX11_LOG_DEBUG ("Locality_DMHandler_Loader::init - " <<
                                "Provided UUID: " << get_opts.opt_arg ());
            this->uuid_ = get_opts.opt_arg ();
          }
          else if (ACE_OS::strcmp (get_opts.long_option (),
                                   ACE_TEXT("activator")) == 0)
          {
            DANCEX11_LOG_DEBUG ("Locality_DMHandler_Loader::init - " <<
                                "Provided Activator reference: " << get_opts.opt_arg ());
            this->activator_ior_ = get_opts.opt_arg ();
          }
          else
          {
            DANCEX11_LOG_WARNING ("Locality_DMHandler_Loader::init - " <<
                                  "Ignoring unknown long option: " <<
                                  get_opts.long_option ());
          }
          break;
      }
    }

    return 0;
  }

  CORBA::object_reference<DeploymentManagerHandler>
  Locality_DMHandler_Loader::create_handler ()
  {
    return CORBA::make_reference<LocalityDeploymentHandler> (
        this->uuid_,
        this->activator_ior_);
  }

  int
  Locality_DMHandler_Loader::Initializer (const ACE_TCHAR* svcname)
  {
    if (svcname)
    {
      ace_svc_desc_Locality_DMHandler_Loader.name_ = svcname;
    }
    return ACE_Service_Config::process_directive (ace_svc_desc_Locality_DMHandler_Loader);
  }
}
