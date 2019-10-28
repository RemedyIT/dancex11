/**
 * @file    common.h
 * @author  Marijke Hengstmengel
 *
 * @brief   Some common definitions for all config_handlers.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef CONFIG_HANDLERS_COMMON_H
#define CONFIG_HANDLERS_COMMON_H

#include <string>

namespace DAnCEX11
{
  namespace Config_Handlers
  {
    /**
     * @class Config_Error
     * @brief Exception to communicate a config_handlers error
     */
    class Config_Error
    {
    public:
      Config_Error (std::string name,
                    std::string error) :
        name_ (std::move(name)),
        error_ (std::move(error))
      {};

      const std::string add_name (const std::string &name) const
      {
        return name + ":" + this->name_;
      }

      const std::string& get_name () const
      {
        return this->name_;
      }
      const std::string& get_error () const
      {
        return this->error_;
      }
    private:
      /// Some identifying information about the element that
      /// caused the error
      std::string name_;

      /// A human readable error message
      std::string const error_;
    };
  }
}
#endif /* CONFIG_HANDLERS_COMMON_H */
