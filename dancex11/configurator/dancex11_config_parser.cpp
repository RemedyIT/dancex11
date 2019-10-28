/**
 * @file    dancex11_config_parser.cpp
 * @author  Martin Corino
 *
 * @brief   Parser for DAnCEX11 deployment config
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11_config_parser.h"

namespace DAnCEX11
{
  template <typename CHAR_T, typename TR>
  const typename Config_Parser_Base_T<CHAR_T, TR>::TokenMap
    Config_Parser_Base_T<CHAR_T, TR>::token_map_ = {
        { "nl.remedy.it.DnCX11.InstanceHandler", Config_Parser_Base_T<CHAR_T, TR>::IInstanceHandler },
        { "nl.remedy.it.DnCX11.DeploymentInterceptor", Config_Parser_Base_T<CHAR_T, TR>::IInterceptor },
        { "nl.remedy.it.DnCX11.ConfigPlugin", Config_Parser_Base_T<CHAR_T, TR>::IConfigHandler },
        { "nl.remedy.it.DnCX11.ServiceObject", Config_Parser_Base_T<CHAR_T, TR>::IServiceObject },
        { "nl.remedy.it.CCM.Component", Config_Parser_Base_T<CHAR_T, TR>::IComponent },
        { "nl.remedy.it.DnCX11.LocalityManager", Config_Parser_Base_T<CHAR_T, TR>::ILocalityManager },
        { "nl.remedy.it.CCM.Container", Config_Parser_Base_T<CHAR_T, TR>::IContainer },

        { "nl.remedy.it.DnCX11.ExecParameter", Config_Parser_Base_T<CHAR_T, TR>::IExecParam },
        { "nl.remedy.it.DnCX11.ConfigProperty", Config_Parser_Base_T<CHAR_T, TR>::IConfigProp },
        { "nl.remedy.it.DnCX11.Connection", Config_Parser_Base_T<CHAR_T, TR>::IConnection },
        { "nl.remedy.it.DnCX11.ExternalConnection", Config_Parser_Base_T<CHAR_T, TR>::IExternalConnection },
        { "nl.remedy.it.DnCX11.Node", Config_Parser_Base_T<CHAR_T, TR>::INode },
        { "nl.remedy.it.DnCX11.Locality", Config_Parser_Base_T<CHAR_T, TR>::ILocality },
        { "nl.remedy.it.DnCX11.Container", Config_Parser_Base_T<CHAR_T, TR>::IContainerID },
        { "nl.remedy.it.DnCX11.Artifact", Config_Parser_Base_T<CHAR_T, TR>::IArtifact },

        { "nl.remedy.it.DnCX11.Property", Config_Parser_Base_T<CHAR_T, TR>::IProperty }
    };

  template <typename CHAR_T, typename TR>
  const typename Config_Parser_Base_T<CHAR_T, TR>::TypeMap
    Config_Parser_Base_T<CHAR_T, TR>::type_map_ = {
        { "char", Config_Parser_Base_T<CHAR_T, TR>::TChar },
        { "octet", Config_Parser_Base_T<CHAR_T, TR>::TOctet },
        { "bool", Config_Parser_Base_T<CHAR_T, TR>::TBool },

        { "int16", Config_Parser_Base_T<CHAR_T, TR>::TInt16 },
        { "int32", Config_Parser_Base_T<CHAR_T, TR>::TInt32 },
        { "int64", Config_Parser_Base_T<CHAR_T, TR>::TInt64 },

        { "uint16", Config_Parser_Base_T<CHAR_T, TR>::TUInt16 },
        { "uint32", Config_Parser_Base_T<CHAR_T, TR>::TUInt32 },
        { "uint64", Config_Parser_Base_T<CHAR_T, TR>::TUInt64 },

        { "float", Config_Parser_Base_T<CHAR_T, TR>::TFloat },
        { "double", Config_Parser_Base_T<CHAR_T, TR>::TDouble },

        { "string", Config_Parser_Base_T<CHAR_T, TR>::TString }
    };

  template <typename CHAR_T, typename TR>
  typename Config_Parser_Base_T<CHAR_T, TR>::token_id
  Config_Parser_Base_T<CHAR_T, TR>::parse_next (bool break_on_asgn)
  {
    this->token_ = this->stream ().next_token (break_on_asgn);
    if (!this->bad () && !this->token_.empty ())
    {
      typename TokenMap::const_iterator it = token_map_.find (this->token_);
      this->token_id_ = (it == token_map_.end ()) ? IText : it->second;
    }
    else
    {
      this->token_id_ = INone;
    }
    return this->token_id_;
  }

  template <typename CHAR_T, typename TR>
  typename Config_Parser_Base_T<CHAR_T, TR>::type_id
  Config_Parser_Base_T<CHAR_T, TR>::token_to_type (const std::string& token)
  {
    if (!token.empty ())
    {
      typename TypeMap::const_iterator it = type_map_.find (token);
      return (it == type_map_.end ()) ? TUser : it->second;
    }
    return TNone;
  }

}
