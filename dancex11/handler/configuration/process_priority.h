// -*- C++ -*-
/**
 * @file    process_priority.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @brief   A simple configuration plugin
 *
 * It will change the process priority.
 * It is only functional on Linux
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_PROCESS_PRIORITY_H
#define DAnCEX11_PROCESS_PRIORITY_H

#include "dancex11/core/dancex11_deploymentconfiguratorsC.h"

#include "dancex11_locality_configuration_export.h"

namespace DAnCEX11
{
  class DAnCEX11_Locality_Configuration_Export Process_Priority final
    : public DAnCEX11::DeploymentConfiguration
  {
  public:
    // Constructor
    Process_Priority () = default;

    // Destructor
    virtual ~Process_Priority () = default;

    std::string type () override;

    void configure (const ::Deployment::Property &prop) override;

    void close () override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Process_Priority (const Process_Priority&) = delete;
    Process_Priority (Process_Priority&&) = delete;
    Process_Priority& operator= (const Process_Priority& x) = delete;
    Process_Priority& operator= (Process_Priority&& x) = delete;
    //@}
  };
}

extern "C"
{
  void
  DAnCEX11_Locality_Configuration_Export create_process_priority (
    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type& plugin);
}

#endif /* DAnCEX11_PROCESS_PRIORITY_H */
