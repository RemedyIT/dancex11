// -*- C++ -*-
/**
 * @file   convert_plan_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "ace/OS_NS_stdio.h"
#include "tao/CDR.h"

#include "tao/x11/base/tao_corba.h"

#include "convert_plan_impl.h"
#include "dancex11/deployment/deployment_planerrorC.h"
#include "dancex11/logger/log.h"

// from deployment_deploymentplanCP.h ; normally hidden for the user
TAO_BEGIN_VERSIONED_NAMESPACE_DECL

DEPLOYMENT_STUB_Export TAO_CORBA::Boolean operator<< (TAO_OutputCDR &, const Deployment::DeploymentPlan&);
DEPLOYMENT_STUB_Export TAO_CORBA::Boolean operator<< (TAO_OutputCDR &, const Deployment::Domain&);

TAO_END_VERSIONED_NAMESPACE_DECL

namespace DAnCEX11
{
  bool
  Convert_Plan::write_cdr_file(const std::string& output_filename,
                               const TAO_OutputCDR& output_cdr)
  {
    FILE *file = ACE_OS::fopen (output_filename.c_str (), "wb");

    if (file == 0)
    {
      DANCEX11_LOG_ERROR ("Convert_Plan::write_cdr_file - "
                          "failed to open file <" <<
                          output_filename << ">");
      return false;
    }

    // First write the byte order
    char byte_order = ACE_CDR_BYTE_ORDER;
    size_t n = ACE_OS::fwrite (std::addressof(byte_order), 1, sizeof(byte_order), file);

    // Next write the length
    ACE_UINT32 const cdr_length (ACE_Utils::truncate_cast<ACE_UINT32> (output_cdr.total_length ()));
    n += ACE_OS::fwrite (std::addressof(cdr_length), 1, sizeof (cdr_length), file);

    DANCEX11_LOG_DEBUG ("Convert_Plan::write_cdr_file - "
                        "Writing plan to file " << output_filename <<
                        " in " <<
                        (ACE_CDR_BYTE_ORDER ? ACE_TEXT("little") : ACE_TEXT("big")) <<
                        " endian format"
                        " and length " << cdr_length);

    // Now write the IDL structure.
    for (const ACE_Message_Block *output_mb = output_cdr.begin ();
          output_mb != nullptr;
          output_mb = output_mb->cont ())
    {
      n += ACE_OS::fwrite (output_mb->rd_ptr (),
                            1,
                            output_mb->length (),
                            file);
    }

    ACE_OS::fclose (file);

    size_t const total_size =
      sizeof (byte_order) + sizeof (cdr_length) + cdr_length;

    if (n != total_size)
    {
      DANCEX11_LOG_ERROR ("Convert_Plan::write_cdr_file - "
                          "Error: Unexpected number of bytes written: " <<
                          n << " instead of " << total_size);
      return false;
    }

    DANCEX11_LOG_DEBUG ("Convert_Plan::write_cdr_file - "
                        "Wrote successfully plan to file " << output_filename <<
                        " in " <<
                        (ACE_CDR_BYTE_ORDER ? ACE_TEXT("little") : ACE_TEXT("big")) <<
                        " endian format"
                        " and length " << total_size);
    return true;
  }

  bool
  Convert_Plan::write_cdr_plan (const std::string& output_filename,
                                const Deployment::DeploymentPlan &plan)
  {
    DANCEX11_LOG_TRACE ("Convert_Plan::write_cdr_plan");

    if (output_filename.empty ())
    {
      DANCEX11_LOG_ERROR ("Convert_Plan::write_cdr_plan - passed"
                          "empty file name");
      return false;
    }

    TAO_OutputCDR output_cdr;

    if (output_cdr << plan)
    {
      return write_cdr_file (output_filename, output_cdr);
    }
    else
    {
      DANCEX11_LOG_ERROR ("Convert_Plan::write_cdr_plan - "
                          "Failed to marshal deployment plan");
      return false;
    }

    return true;
  }

  bool
  Convert_Plan::write_cdr_domain (const std::string& output_filename,
                                  const Deployment::Domain &domain)
  {
    DANCEX11_LOG_TRACE ("Convert_Plan::write_cdr_domain");

    if (output_filename.empty ())
    {
      DANCEX11_LOG_ERROR ("Convert_Plan::write_cdr_domain - passed"
                          "empty file name");
      return false;
    }

    TAO_OutputCDR output_cdr;

    if (output_cdr << domain)
    {
      return write_cdr_file (output_filename, output_cdr);
    }
    else
    {
      DANCEX11_LOG_ERROR ("Convert_Plan::write_cdr_domain - "
                          "Failed to marshal deployment domain");
      return false;
    }

    return true;
  }

}
