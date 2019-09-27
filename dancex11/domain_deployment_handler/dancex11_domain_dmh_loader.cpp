/**
 * @file    dancex11_domain_dmh_loader.cpp
 * @author  Martin Corino
 *
 * @brief   Service loader for Domain DMH implementation.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11_domain_dmh_loader.h"
#include "dancex11_domain_deployment_handler.h"

#include "ace/Get_Opt.h"
#include "ace/Service_Config.h"

namespace DAnCEX11
{
  // Typedef to help declare the standardized factory name
  typedef Domain_DMHandler_Loader DAnCEX11_DeploymentHandler_Impl;

  // Define the static Service descriptor.
  // Note the standardized name for the factory method declaration.
  ACE_STATIC_SVC_DEFINE (Domain_DMHandler_Loader,
                         ACE_TEXT ("DANCEX11_DEPLOYMENT_HANDLER"),
                         ACE_SVC_OBJ_T,
                         &ACE_SVC_NAME (DAnCEX11_DeploymentHandler_Impl),
                         ACE_Service_Type::DELETE_THIS
                          | ACE_Service_Type::DELETE_OBJ,
                         0)
  // Define implementation for Service factory method and cleanup method
  // (gobbler).
  // Instead of the real classname a typedef is used to force generation
  // of the standardized factory method name.
  ACE_FACTORY_DEFINE (DANCEX11_DOMAIN_DH, DAnCEX11_DeploymentHandler_Impl)

  int Domain_DMHandler_Loader::init (int argc, ACE_TCHAR *argv[])
  {
    ACE_Get_Opt get_opts (argc,
                          argv,
                          ACE_TEXT("h"),
                          0,
                          0,
                          ACE_Get_Opt::RETURN_IN_ORDER);

    get_opts.long_option (ACE_TEXT("node"), ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("nodemap"), ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("mapfmt"), ACE_Get_Opt::ARG_REQUIRED);
    get_opts.long_option (ACE_TEXT("help"), 'h', ACE_Get_Opt::NO_ARG);

    int c;
    while ( (c = get_opts ()) != -1)
    {
      switch (c)
      {
          //X11_FUZZ: disable check_cout_cerr
        case 'h':
          {
            std::cerr << "Domain Deployment Handler options: "
                      << "-- [--node NODE=IOR [--node ...]] [--nodemap CDD] [--mapfmt FMT] [-h]" << std::endl
                      << "\t--node NODE=IOR\t\t\t\t Specify IOR for node with name NODE" << std::endl
                      << "\t--nodemap CDD\t\t\t\t Specify domain description file (.cdd) CDD" << std::endl
                      << "\t--mapfmt FMT\t\t\t\t Specify domain description file format FMT" << std::endl
                      << "\t-h|--help\t\t\t\t Show this info" << std::endl << std::endl;
            ACE_OS::exit (1);
          }
          break;
          //X11_FUZZ: enable check_cout_cerr

        case 0:
          if (ACE_OS::strcmp (get_opts.long_option (),
                              ACE_TEXT("node")) == 0)
          {
            DANCEX11_LOG_DEBUG ("Domain_DMHandler_Loader::init - " <<
                                "Adding node mapping: " << get_opts.opt_arg ());
            this->nodes_.push_back (get_opts.opt_arg ());
          }
          else if (ACE_OS::strcmp (get_opts.long_option (),
                                   ACE_TEXT("nodemap")) == 0)
          {
            DANCEX11_LOG_DEBUG ("Domain_DMHandler_Loader::init - " <<
                                "Provided CDD: " << get_opts.opt_arg ());
            this->cdd_ = get_opts.opt_arg ();
          }
          else if (ACE_OS::strcmp (get_opts.long_option (),
                                   ACE_TEXT("mapfmt")) == 0)
          {
            DANCEX11_LOG_DEBUG ("Domain_DMHandler_Loader::init - " <<
                                "Provided CDD format: " << get_opts.opt_arg ());
            this->cddfmt_ = get_opts.opt_arg ();
          }
          else
          {
            DANCEX11_LOG_WARNING ("Domain_DMHandler_Loader::init - " <<
                                  "Ignoring unknown long option: " <<
                                  get_opts.long_option ());
          }
          break;
      }
    }

    return 0;
  }

  CORBA::object_reference<DeploymentManagerHandler>
  Domain_DMHandler_Loader::create_handler ()
  {
    return CORBA::make_reference<DomainDeploymentHandler> (
              this->nodes_,
              this->cdd_,
              this->cddfmt_);
  }

  int
  Domain_DMHandler_Loader::Initializer (const ACE_TCHAR* svcname)
  {
    if (svcname)
    {
      ace_svc_desc_Domain_DMHandler_Loader.name_ = svcname;
    }
    return ACE_Service_Config::process_directive (ace_svc_desc_Domain_DMHandler_Loader);
  }
}
