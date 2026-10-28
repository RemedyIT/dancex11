/**
 * @file    dyncharsequence.h
 * @author  Johnny Willemsen
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 *
 * @brief   Helpers for the character-string representation of char sequences.
 */

#ifndef DANCEX11_CONFIG_HANDLERS_DYNCHARSEQUENCE_H
#define DANCEX11_CONFIG_HANDLERS_DYNCHARSEQUENCE_H

#include "Basic_Deployment_Data.hpp"
#include "common.h"

#include <string>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    inline std::string
    extract_char_sequence (const DAnCE::Config_Handlers::DataValue& value)
    {
      if (value.element ().empty ())
        return {};

      if (value.element ().size () != 1)
        throw Config_Error ("", "A char sequence must contain one string value");

      DAnCE::Config_Handlers::DataValue const& data = value.element ().front ();
      if (data.string ().size () != 1 ||
          !data._cxx_short ().empty () || !data._cxx_long ().empty () ||
          !data.ushort ().empty () || !data.ulong ().empty () ||
          !data._cxx_float ().empty () || !data._cxx_double ().empty () ||
          !data.boolean ().empty () || !data.octet ().empty () ||
          !data._cxx_enum ().empty () || !data.longlong ().empty () ||
          !data.ulonglong ().empty () || !data.longdouble ().empty () ||
          !data.element ().empty () || !data.member ().empty ())
        throw Config_Error ("", "A char sequence must contain exactly one string value");

      return data.string ().front ();
    }
  }
}

#endif
