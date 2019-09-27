/**
 * @file    dancex11_config_stream.cpp
 * @author  Martin Corino
 *
 * @brief   Custom stream class for DAnCEX11 deployment config
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11/configurator/dancex11_config_stream.h"

#include <sstream>
#include <cctype>

#include <iostream>

namespace DAnCEX11
{
  namespace cfgstr
  {
    inline bool ishex(Config_Stream_Base_T<char>::int_type c)
    {
      return ((c|0x20)>='a' && (c|0x20)<='f') || std::isdigit(c);
    }

    inline bool isbreak(Config_Stream_Base_T<char>::int_type c, bool break_on_asgn=false)
    {
      return std::iswspace (c) ||
             (':' == c) ||
             ('[' == c) ||
             (']' == c) ||
             (',' == c) ||
             ('<' == c) ||
             ('>' == c) ||
             (break_on_asgn && '=' == c);
    }
  }


  template <>
  Config_Stream_Base_T<wchar_t>::string_type
  Config_Stream_Base_T<wchar_t>::next_token (bool)
  {
    return {};
  }

  template <>
  Config_Stream_Base_T<char>::int_type
  Config_Stream_Base_T<char>::get_escape ()
  {
    source_type & in = this->source ();
    in.ignore (1);
    int_type c = in.peek ();
    switch (c)
    {
      case 'x':
        {
          in.ignore (1);
          char hbuf[3];
          int n {};
          while (n<2 && in.good () && cfgstr::ishex (in.peek ()))
          {
            hbuf[n++] = in.get ();
          }
          hbuf[n] = '\0';
          std::istringstream is (hbuf);
          int i;
          if (is >> std::hex >> i)
          {
            return i;
          }
        }
        break;
      case 'n':
        return '\n';
      case 'r':
        return '\r';
      case 'f':
        return '\f';
      case 't':
        return '\t';
      case 'v':
        return '\v';
      case 'b':
        return '\b';
      case 'a':
        return '\a';
      default:
        if (std::isdigit (c))
        {
          char obuf[4];
          int n {1};
          obuf[0] = in.get ();
          while (n<3 && in.good () && std::isdigit (in.peek ()))
          {
            obuf[n++] = in.get ();
          }
          obuf[n] = '\0';
          std::istringstream is (obuf);
          int i;
          if (is >> std::oct >> i)
          {
            return i;
          }
        }
        else
        {
          return in.get ();
        }
        break;
    }
    return {}; // never reached
  }

  template <>
  Config_Stream_Base_T<char>::string_type
  Config_Stream_Base_T<char>::next_token (bool break_on_asgn)
  {
    source_type & in = this->source ();
    string_type token;
    while (in.good ())
    {
      int_type c = in.peek ();

      // skip whitespace
      while (std::iswspace (c))
      {
        if (c == '\n')
          this->line_++;
        in.ignore (1);
        c = in.peek ();
      }

      //std::cout << "peek char [" << c << "]" << std::endl;
      if (c == '/')
      {
        in.get ();
        if (in.peek () != '/')
        {
          in.unget ();
          c = 0;
        }
      }
      if (c == '#' || c =='/')
      {
        // comment line - read and discard
        string_type line;
        std::getline (in, line);
        this->line_++;
      }
      else if (c != source_type::traits_type::eof ())
      {
        // token encountered
        switch (c)
        {
          // quoted string values
          case '\'':
          case '"':
            {
              int_type qt = in.get ();
              token.append (1, qt);
              do
              {
                if (in.peek () == '\\')
                {
                  c = 0;
                  token.append (1, this->get_escape ());
                }
                else
                {
                  token.append (1, c = in.get ());
                }
              } while (in.good () && c != qt);
              this->break_ = ' ';
              return token;
            }
            break;

          // array value sequences
          case '[':
          case ']':
            // report array start/end
            token.append (1, in.get ());
            this->break_ = ' ';
            return token;

          default:
            if (cfgstr::isbreak (c, break_on_asgn))
            {
              //  skip and continue
              in.ignore (1);
            }
            else
            {
              // identifier; extract till next break
              do
              {
                token.append (1, in.get ());
                c = in.peek ();
              } while (in.good () && !cfgstr::isbreak (c, break_on_asgn));

              // skip whitespace till next break
              while (in.good () && std::iswspace (c))
              {
                if (c == '\n')
                  this->line_++;
                in.ignore (1);
                c = in.peek ();
              }
              if (in.good () && cfgstr::isbreak (c, break_on_asgn) &&
                  c != '[' && c != ']')
              {
                this->break_ = in.get ();
              }
              else
              {
                this->break_ = ' ';
              }

              return token;
            }
            break;
        }
      }
    }
    return {};
  }

}
