/**
 * @file    log.cpp
 * @author  Marijke Hengstmengel
 *
 * @brief   DANCEX11 logger stream implementations
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11/logger/log.h"

namespace DAnCEX11
{
#if !defined(X11_NLOGGING)

  namespace dancex11_logger
  {
    DANCEX11_Log_Msg* DANCEX11_Log_Msg::getInstance()
    {
      static DANCEX11_Log_Msg instance_;

      return std::addressof(instance_);
    }

    DANCEX11_Log_Msg::DANCEX11_Log_Msg()
    : x11_logger::Log_Module ("DANCEX11")
    {
    }

    DANCEX11_Log_Msg::~DANCEX11_Log_Msg()
    {
    }
  } // namespace dancex11_logger

#endif
} //namespace DAnCEX11
