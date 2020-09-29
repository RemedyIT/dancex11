/**
 * @file    dancex11_config_util.h
 * @author  Martin Corino
 *
 * @brief   Utility templates for Config_Loader
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_CONFIG_UTIL_H_
#define DANCEX11_CONFIG_UTIL_H_

#pragma once

#include "dancex11_config_loader.h"

#include <sstream>

namespace DAnCEX11
{
  namespace str
  {
    template <typename Char_T>
    bool icasecmp(const std::basic_string<Char_T>& l, const std::basic_string<Char_T>& r)
    {
        using valtype =  typename std::basic_string<Char_T>::value_type;
        return l.size() == r.size()
            && std::equal(l.cbegin(), l.cend(), r.cbegin(),
                [](valtype l1, valtype r1)
                    { return toupper(l1) == toupper(r1); });
    }

    template <typename Char_T>
    bool icasencmp(const std::basic_string<Char_T>& l, const std::basic_string<Char_T>& r, uint32_t n)
    {
        using valtype = typename std::basic_string<Char_T>::value_type;
        return (l.size() >= n) && (r.size() >= n)
            && std::equal(l.cbegin(), l.cbegin()+n, r.cbegin(),
                [](valtype l1, valtype r1)
                    { return toupper(l1) == toupper(r1); });
    }


    template <typename Char_T>
    typename std::basic_string<Char_T>::value_type
    parse_character (const std::basic_string<Char_T>& token)
    {
      return token[1];
    }

    template <typename Char_T>
    bool
    parse_boolean (const std::basic_string<Char_T>& token)
    {
      return token[0] == 't' || token[0] == 'T';
    }

    template <typename Char_T>
    int64_t
    parse_signed (const std::basic_string<Char_T>& token)
    {
      std::basic_istringstream<Char_T> is (token);
      int64_t v;
      is >> std::dec >> v;
      return v;
    }

    template <typename Char_T>
    uint64_t
    parse_unsigned (const std::basic_string<Char_T>& token)
    {
      std::basic_istringstream<Char_T> is (token);
      uint64_t v;
      is >> std::dec >> v;
      return v;
    }

    template <typename Char_T>
    uint64_t
    parse_hexadecimal (const std::basic_string<Char_T>& token)
    {
      std::basic_istringstream<Char_T> is (token);
      uint64_t v;
      is >> std::hex >> v;
      return v;
    }

    template <typename Char_T>
    uint64_t
    parse_octal (const std::basic_string<Char_T>& token)
    {
      std::basic_istringstream<Char_T> is (token);
      uint64_t v;
      is >> std::oct >> v;
      return v;
    }

    template <typename Char_T>
    double
    parse_double (const std::basic_string<Char_T>& token)
    {
      std::basic_istringstream<Char_T> is (token);
      double v;
      is >> v;
      return v;
    }

    template <typename Char_T>
    std::basic_string<Char_T>
    parse_string (const std::basic_string<Char_T>& token)
    {
      return token.substr (1, token.size ()-2);
    }
  } // str

  namespace cfg
  {
    template <typename Char_T>
    std::basic_string<Char_T>
    extension (const std::basic_string<Char_T>& path)
    {
      typename std::basic_string<Char_T>::size_type n = path.rfind ('.');
      return (n == std::basic_string<Char_T>::npos) ? "" : path.substr (n+1);
    }

    template <typename Char_T>
    std::basic_string<Char_T>
    basename (const std::basic_string<Char_T>& path, bool no_ext = false)
    {
      typename std::basic_string<Char_T>::size_type n = std::basic_string<Char_T>::npos;
      typename std::basic_string<Char_T>::size_type off = path.rfind (ACE_DIRECTORY_SEPARATOR_CHAR_A);
      if (off == std::basic_string<Char_T>::npos)
        off = 0;
      else
        off += 1;
      if (no_ext)
      {
        n = path.rfind ('.');
        if (n == std::basic_string<Char_T>::npos || n < off)
          n = std::basic_string<Char_T>::npos;
        else
          n -= off;
      }
      return path.substr (off, n);
    }

    template <typename Char_T>
    char
    parse_char (const std::basic_string<Char_T>& token,
                     Config_Loader::literal_type lt)
    {
      using literal_type = Config_Loader::literal_type;
      switch (lt.base ())
      {
        case literal_type::lt_char:
          return str::parse_character (token);
        case literal_type::lt_bool:
          return str::parse_boolean (token) ? 'T' : 'F';
        case literal_type::lt_int:
          return ACE_Utils::truncate_cast<char> (str::parse_signed (token));
        case literal_type::lt_uint:
          return ACE_Utils::truncate_cast<char> (str::parse_unsigned (token));
        case literal_type::lt_hex:
          return ACE_Utils::truncate_cast<char> (str::parse_hexadecimal (token));
        case literal_type::lt_oct:
          return ACE_Utils::truncate_cast<char> (str::parse_octal (token));
        case literal_type::lt_dec:
          return {};
        case literal_type::lt_str:
          return str::parse_string (token)[0];
      }
      return {};
    }

    template <typename UInt_T, typename Char_T>
    UInt_T
    parse_uint (const std::basic_string<Char_T>& token,
                 Config_Loader::literal_type lt)
    {
      using literal_type = Config_Loader::literal_type;
      switch (lt.base ())
      {
        case literal_type::lt_char:
          return str::parse_character (token);
        case literal_type::lt_bool:
          return str::parse_boolean (token) ? 1 : 0;
        case literal_type::lt_int:
          return ACE_Utils::truncate_cast<UInt_T> (str::parse_signed (token));
        case literal_type::lt_uint:
          return ACE_Utils::truncate_cast<UInt_T> (str::parse_unsigned (token));
        case literal_type::lt_hex:
          return ACE_Utils::truncate_cast<UInt_T> (str::parse_hexadecimal (token));
        case literal_type::lt_oct:
          return ACE_Utils::truncate_cast<UInt_T> (str::parse_octal (token));
        case literal_type::lt_dec:
          return (UInt_T)str::parse_double (token);
        case literal_type::lt_str:
          return ACE_Utils::truncate_cast<UInt_T> (str::parse_unsigned(str::parse_string (token)));
      }
      return {};
    }

    template <typename Char_T>
    bool
    parse_bool (const std::basic_string<Char_T>& token,
                Config_Loader::literal_type lt)
    {
      using literal_type = Config_Loader::literal_type;
      switch (lt.base ())
      {
        case literal_type::lt_char:
          {
            typename std::basic_string<Char_T>::value_type c =
                str::parse_character (token);
            return (c == 't' || c == 'T' || c == '1');
          }
        case literal_type::lt_bool:
          return str::parse_boolean (token);
        case literal_type::lt_int:
          return str::parse_signed (token);
        case literal_type::lt_uint:
          return str::parse_unsigned (token);
        case literal_type::lt_hex:
          return str::parse_hexadecimal (token);
        case literal_type::lt_oct:
          return str::parse_octal (token);
        case literal_type::lt_dec:
          return {};
        case literal_type::lt_str:
          {
            std::basic_string<Char_T> s =
                str::parse_string (token);
            std::basic_string<Char_T> true_string {"true"};
            return str::icasecmp (s, true_string);
          }
      }
      return {};
    }

    template <typename Int_T, typename Char_T>
    Int_T
    parse_int (const std::basic_string<Char_T>& token,
                 Config_Loader::literal_type lt)
    {
      using literal_type = Config_Loader::literal_type;
      switch (lt.base ())
      {
        case literal_type::lt_char:
          return str::parse_character (token);
        case literal_type::lt_bool:
          return str::parse_boolean (token) ? 1 : 0;
        case literal_type::lt_int:
          return ACE_Utils::truncate_cast<Int_T> (str::parse_signed (token));
        case literal_type::lt_uint:
          return ACE_Utils::truncate_cast<Int_T> (str::parse_unsigned (token));
        case literal_type::lt_hex:
          return ACE_Utils::truncate_cast<Int_T> (str::parse_hexadecimal (token));
        case literal_type::lt_oct:
          return ACE_Utils::truncate_cast<Int_T> (str::parse_octal (token));
        case literal_type::lt_dec:
          return (Int_T)str::parse_double (token);
        case literal_type::lt_str:
          return ACE_Utils::truncate_cast<Int_T> (str::parse_signed(str::parse_string (token)));
      }
      return {};
    }

    template <typename Dec_T, typename Char_T>
    Dec_T
    parse_dec (const std::basic_string<Char_T>& token,
                 Config_Loader::literal_type lt)
    {
      using literal_type = Config_Loader::literal_type;
      switch (lt.base ())
      {
        case literal_type::lt_char:
          return str::parse_character (token);
        case literal_type::lt_bool:
          return str::parse_boolean (token) ? (Dec_T)1.0 : (Dec_T)0.0;
        case literal_type::lt_int:
          return (Dec_T)str::parse_signed (token);
        case literal_type::lt_uint:
          return (Dec_T)str::parse_unsigned (token);
        case literal_type::lt_hex:
          return (Dec_T)str::parse_hexadecimal (token);
        case literal_type::lt_oct:
          return (Dec_T)str::parse_octal (token);
        case literal_type::lt_dec:
          return (Dec_T)str::parse_double (token);
        case literal_type::lt_str:
          return (Dec_T)str::parse_signed(str::parse_string (token));
      }
      return {};
    }

    template <typename Char_T>
    std::string
    parse_string (const std::basic_string<Char_T>& token,
                  Config_Loader::literal_type lt)
    {
      using literal_type = Config_Loader::literal_type;
      switch (lt.base ())
      {
        case literal_type::lt_char:
          {
            typename std::basic_string<Char_T>::value_type c =
                str::parse_character (token);
            return { c };
          }
        case literal_type::lt_bool:
          return token;
        case literal_type::lt_int:
          return token;
        case literal_type::lt_uint:
          return token;
        case literal_type::lt_hex:
          return token;
        case literal_type::lt_oct:
          return token;
        case literal_type::lt_dec:
          return token;
        case literal_type::lt_str:
          return str::parse_string (token);
      }
      return {};
    }
  } // cfg
} // DAnCEX11

#endif /* DANCEX11_CONFIG_UTIL_H_ */
