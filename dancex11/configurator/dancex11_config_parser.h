/**
 * @file    dancex11_config_parser.h
 * @author  Martin Corino
 *
 * @brief   Parser for DAnCEX11 deployment config
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_CONFIG_PARSER_H
#define DAnCEX11_CONFIG_PARSER_H

#pragma once

#include "dancex11_config_stream.h"

#include <vector>
#include <map>

namespace DAnCEX11
{
  template <typename CHAR_T,
            typename TR = std::char_traits<CHAR_T>>
  class Config_Parser_Base_T
  {
  public:
    using stream_base_type = Config_Stream_Base_T<CHAR_T, TR>;
    using char_type = typename stream_base_type::source_type::char_type;
    using traits_type = typename stream_base_type::source_type::traits_type;
    using string_type = typename stream_base_type::string_type;

    enum token_id : uint32_t
    {
      INone,
      // identifiers, values
      IText,

      // dynamic object types
      IInstanceHandler,
      IInterceptor,
      IConfigHandler,
      IServiceObject,
      IComponent,
      ILocalityManager,
      IContainer,

      // object proeprties
      IExecParam,
      IConfigProp,

      // component properties
      IConnection,
      IExternalConnection,
      INode,
      ILocality,
      IContainerID,
      IArtifact,

      // common properties
      IProperty,
    };

    enum type_id : uint32_t
    {
      TNone,
      TUser,

      TChar, TOctet, TBool,
      TInt16, TInt32, TInt64,
      TUInt16, TUInt32, TUInt64,
      TFloat, TDouble,
      TString
    };

    virtual ~Config_Parser_Base_T () = default;

    token_id parse_next (bool break_on_asgn=false);

    const string_type& current_token () const
    { return this->token_; }

    token_id current_token_id () const
    { return this->token_id_; }

    bool at_object_entry () const
    { return this->token_id_ >= IInstanceHandler ||
             this->token_id_ <= IContainer; }

    bool at_object_property () const
    { return this->token_id_ == IExecParam ||
             this->token_id_ == IConfigProp; }

    bool at_component_node () const
    { return this->token_id_ == INode; }

    bool at_component_locality () const
    { return this->token_id_ == ILocality; }

    bool at_component_container_id () const
    { return this->token_id_ == IContainerID; }

    bool at_component_artifact () const
    { return this->token_id_ == IArtifact; }

    bool at_component_connection () const
    { return this->token_id_ == IConnection ||
             this->token_id_ == IExternalConnection; }

    uint32_t line () const { return this->stream ().line (); }

    typename stream_base_type::int_type break_ch () const { return this->stream ().break_ch (); }

    operator void*() const
    { return (void*)this->stream (); }

    bool
    operator!() const
    { return !this->stream (); }

    bool
    good() const
    { return this->stream ().good (); }

    bool
    eof() const
    { return this->stream ().eof (); }

    bool
    fail() const
    { return this->stream ().fail (); }

    bool
    bad() const
    { return this->stream ().bad (); }


    static type_id token_to_type (const std::string&);

  protected:
    Config_Parser_Base_T () = default;

    virtual stream_base_type& stream () const = 0;

  private:
    using TokenMap = std::map<string_type, token_id>;
    using TypeMap = std::map<string_type, type_id>;

    static const TokenMap token_map_;
    static const TypeMap type_map_;

    string_type token_;
    token_id token_id_ { INone };
  };

  template <typename Stream_>
  class Config_Parser_T
    : public Config_Parser_Base_T<typename Stream_::char_type,
                                  typename Stream_::traits_type>
  {
  public:
    using base_type = Config_Parser_Base_T<typename Stream_::char_type,
        typename Stream_::traits_type>;
    using stream_type = Config_Stream_T<Stream_>;

    template <typename ...Args>
    Config_Parser_T(Args&& ...args)
      : stream_ (std::forward<Args> (args)...)
    { }
    ~Config_Parser_T () override = default;

  protected:
    typename base_type::stream_base_type& stream () const override
    { return const_cast<Config_Parser_T*> (this)->stream_; }

  private:
    stream_type stream_;
  };
}

#include "dancex11/configurator/dancex11_config_parser.cpp"

#endif /* DAnCEX11_CONFIG_PARSER_H */
