/**
 * @file    log.h
 * @author  Marijke Hengstmengel
 *
 * @brief   DANCEX11 logger stream implementations
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_LOGGER_LOG_H
#define DANCEX11_LOGGER_LOG_H

#include "tao/x11/logger/log_base.h"
#include "dancex11/logger/dancex11_logger_export.h"

// Priorities for logging levels defined in  tao/x11/logger/log_base.h:
/// P_PANIC : Used for errors that cause the fatal shutdown of any portion
/// of the infrastructure.

/// P_CRITICAL : Used for deployment errors at the point the error
/// exits the process in question, or when a decision is made to
/// cause the deployment to fail.

/// P_ERROR : Used for non-fatal deployment errors that do not cause deployment
/// failure and log detailed error information at the point of failure.


/// P_WARNING : used to indicate that a questionable situation that doesn't cause
/// deployment failure, but can cause undefined conditions.

/// P_INFO : Used to indicate that a ` deployment event has completed.

/// P_DEBUG : Used to display important configuration information that impacts
/// the deployment process.

/// P_TRACE: Used to trace the execution of DAnCE code.
namespace DAnCEX11
{

#define dancex11_trace      x11_logger::trace ()
#define dancex11_debug      x11_logger::debug ()
#define dancex11_info       x11_logger::info ()
#define dancex11_warning    x11_logger::warning ()
#define dancex11_error      x11_logger::error ()
#define dancex11_critical   x11_logger::critical ()
#define dancex11_panic      x11_logger::panic ()

#define dancex11_trace_w    x11_logger::trace_w ()
#define dancex11_debug_w    x11_logger::debug_w ()
#define dancex11_info_w     x11_logger::info_w ()
#define dancex11_warning_w  x11_logger::warning_w ()
#define dancex11_error_w    x11_logger::error_w ()
#define dancex11_critical_w x11_logger::critical_w ()
#define dancex11_panic_w    x11_logger::panic_w ()

#if !defined(X11_NLOGGING)

  namespace dancex11_logger
  {
    class DANCEX11_Logger_Export DANCEX11_Log_Msg
    : public x11_logger::Log_Module
    {
    public:
      virtual ~DANCEX11_Log_Msg();

      using log_type = x11_logger::Log_Type_T<DANCEX11_Log_Msg>;

      static DANCEX11_Log_Msg* getInstance();

    private:
      DANCEX11_Log_Msg();
    };

  } // namespace dancex11_logger

#endif /* !X11_NLOGGING */
} // namespace DANCEX11_NAMESPACE

#if defined(X11_NLOGGING)
# define DANCEX11_LOGGER   x11_logger::NULL_LogType
#else
#define DANCEX11_LOGGER \
  DAnCEX11::dancex11_logger::DANCEX11_Log_Msg::log_type
#endif /* !X11_NLOGGING */

// CORE logging
#define DANCEX11_LOG_TRACE( __stmt__) \
  X11_LOG_TRACE( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_DEBUG( __stmt__) \
  X11_LOG_DEBUG(DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_INFO( __stmt__) \
  X11_LOG_INFO( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_WARNING( __stmt__) \
  X11_LOG_WARNING ( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_ERROR( __stmt__) \
  X11_LOG_ERROR( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_CRITICAL( __stmt__) \
  X11_LOG_CRITICAL( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_PANIC( __stmt__) \
  X11_LOG_PANIC( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_TRACE_W( __stmt__) \
   X11_LOG_TRACE_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_DEBUG_W( __stmt__) \
  X11_LOG_DEBUG_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_INFO_W( __stmt__) \
  X11_LOG_INFO_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_WARNING_W( __stmt__) \
  X11_LOG_WARNING_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_ERROR_W( __stmt__) \
  X11_LOG_ERROR_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_CRITICAL_W( __stmt__) \
  X11_LOG_CRITICAL_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_LOG_PANIC_W( __stmt__) \
  X11_LOG_PANIC_W( DANCEX11_LOGGER, __stmt__)

#define DANCEX11_CALL_TRACE(__call__) \
  X11_CALL_TRACE(DANCEX11_LOGGER, __call__)

#define DANCEX11_CALL_TRACE_W(__call__) \
  X11_CALL_TRACE_W(DANCEX11_LOGGER, __call__)

#endif /* DANCEX11_LOGGER_LOG_H */
