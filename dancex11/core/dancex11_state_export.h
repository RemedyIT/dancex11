// -*- C++ -*-
/**
 * @file    dancex11_state_export.h
 * @author  Martin Corino
 *
 * @brief   Generated
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_STATE_EXPORT_H
#define DANCEX11_STATE_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (DANCEX11_STATE_HAS_DLL)
#  define DANCEX11_STATE_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && DANCEX11_STATE_HAS_DLL */

#if !defined (DANCEX11_STATE_HAS_DLL)
#  define DANCEX11_STATE_HAS_DLL 1
#endif /* ! DANCEX11_STATE_HAS_DLL */

#if defined (DANCEX11_STATE_HAS_DLL) && (DANCEX11_STATE_HAS_DLL == 1)
#  if defined (DANCEX11_STATE_BUILD_DLL)
#    define DANCEX11_STATE_Export ACE_Proper_Export_Flag
#    define DANCEX11_STATE_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_STATE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* DANCEX11_STATE_BUILD_DLL */
#    define DANCEX11_STATE_Export ACE_Proper_Import_Flag
#    define DANCEX11_STATE_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_STATE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* DANCEX11_STATE_BUILD_DLL */
#else /* DANCEX11_STATE_HAS_DLL == 1 */
#  define DANCEX11_STATE_Export
#  define DANCEX11_STATE_SINGLETON_DECLARATION(T)
#  define DANCEX11_STATE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* DANCEX11_STATE_HAS_DLL == 1 */

// Set DANCEX11_STATE_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (DANCEX11_STATE_NTRACE)
#  if (ACE_NTRACE == 1)
#    define DANCEX11_STATE_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define DANCEX11_STATE_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !DANCEX11_STATE_NTRACE */

#if (DANCEX11_STATE_NTRACE == 1)
#  define DANCEX11_STATE_TRACE(X)
#else /* (DANCEX11_STATE_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define DANCEX11_STATE_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (DANCEX11_STATE_NTRACE == 1) */

#endif /* DANCEX11_STATE_EXPORT_H */

// End of auto generated file.
