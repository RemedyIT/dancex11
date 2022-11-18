// -*- C++ -*-
/**
 * @file    process_name.h
 * @author  Johnny Willemsen, Martin Corino
 *
 * @brief   A simple configuration plugin
 *
 * It will change the process name.
 * It is only functional on Linux
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_PROCESS_NAME_H
#define DAnCEX11_PROCESS_NAME_H

#include "dancex11/core/dancex11_deploymentconfiguratorsC.h"

#include "dancex11_locality_configuration_export.h"

namespace DAnCEX11
{
  class DAnCEX11_Locality_Configuration_Export Process_Name final
    : public DAnCEX11::DeploymentConfiguration
  {
  public:
    // Constructor
    Process_Name () = default;

    // Destructor
    ~Process_Name () override = default;

    std::string type () override;

    void configure (const Deployment::Property & prop) override;

    void close () override;

    //@{
    /** Illegal to be called. Deleted explicitly to let the compiler detect any violation */
    Process_Name (const Process_Name&) = delete;
    Process_Name (Process_Name&&) = delete;
    Process_Name& operator= (const Process_Name& x) = delete;
    Process_Name& operator= (Process_Name&& x) = delete;
    //@}
  };
}

extern "C"
{
  void
  DAnCEX11_Locality_Configuration_Export create_process_name (
    IDL::traits<DAnCEX11::DeploymentConfiguration>::ref_type& plugin);
}

#endif /* DAnCEX11_PROCESS_NAME_H */
