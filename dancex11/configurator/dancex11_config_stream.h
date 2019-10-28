/**
 * @file    dancex11_config_stream.h
 * @author  Martin Corino
 *
 * @brief   Custom stream class for DAnCEX11 deployment config
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_CONFIG_STREAM_H
#define DAnCEX11_CONFIG_STREAM_H

#pragma once

#include "dancex11/configurator/dancex11_configurator_export.h"

#include <string>
#include <istream>
#include <fstream>

namespace DAnCEX11
{
  template <typename CHAR_T,
            typename TR = std::char_traits<CHAR_T>>
  class Config_Stream_Base_T
  {
  public:
    typedef std::basic_string<CHAR_T, TR> string_type;
    typedef std::basic_istream<CHAR_T, TR> source_type;
    typedef typename source_type::int_type int_type;

    virtual ~Config_Stream_Base_T () = default;

    string_type next_token (bool break_on_asgn=false) { return {}; }

    uint32_t line () const { return this->line_; }

    int_type break_ch () const { return this->break_; }

    operator void*() const
    { return this->good() ? const_cast<Config_Stream_Base_T*>(this) : 0; }

    bool
    operator!() const
    { return !good (); }

    bool
    good() const
    { return this->source ().good (); }

    bool
    eof() const
    { return this->source ().eof (); }

    bool
    fail() const
    { return this->source ().fail (); }

    bool
    bad() const
    { return this->source ().bad (); }

  protected:
    Config_Stream_Base_T () = default;

    int_type get_escape () { return 0; }

    virtual source_type& source () const = 0;

    uint32_t line_ {};
    int_type break_ {};
  };

  template <typename Stream_>
  class Config_Stream_T :
      public Config_Stream_Base_T<typename Stream_::char_type,
                                  typename Stream_::traits_type>
  {
  public:
    typedef Config_Stream_Base_T<
        typename Stream_::char_type,
        typename Stream_::traits_type>  base;

    template <typename ...Args>
    Config_Stream_T(Args&& ...args)
      : Config_Stream_Base_T<typename Stream_::char_type,
                             typename Stream_::traits_type> (),
        source_ (std::forward<Args> (args)...)
    { this->source_ >> std::skipws; }

    virtual ~Config_Stream_T () = default;

  protected:
    virtual typename base::source_type& source () const
    { return const_cast<Config_Stream_T*> (this)->source_; }

    Stream_ source_;
  };

  template <>
  DAnCE_Configurator_Export Config_Stream_Base_T<char>::string_type
  Config_Stream_Base_T<char>::next_token (bool break_on_asgn);

  template <>
  DAnCE_Configurator_Export Config_Stream_Base_T<char>::int_type
  Config_Stream_Base_T<char>::get_escape ();
}

#endif /* DAnCEX11_CONFIG_STREAM_H */
