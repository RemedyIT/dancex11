// -*- C++ -*-
/**
 * @file    dancex11_configurator_export.h
 * @author  Martin Corino
 *
 * @brief   Generated
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_CONFIGURATOR_EXPORT_H
#define DANCEX11_CONFIGURATOR_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (DANCE_CONFIGURATOR_HAS_DLL)
#  define DANCE_CONFIGURATOR_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && DANCE_CONFIGURATOR_HAS_DLL */

#if !defined (DANCE_CONFIGURATOR_HAS_DLL)
#  define DANCE_CONFIGURATOR_HAS_DLL 1
#endif /* ! DANCE_CONFIGURATOR_HAS_DLL */

#if defined (DANCE_CONFIGURATOR_HAS_DLL) && (DANCE_CONFIGURATOR_HAS_DLL == 1)
#  if defined (DANCE_CONFIGURATOR_BUILD_DLL)
#    define DAnCE_Configurator_Export ACE_Proper_Export_Flag
#    define DANCE_CONFIGURATOR_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define DANCE_CONFIGURATOR_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* DANCE_CONFIGURATOR_BUILD_DLL */
#    define DAnCE_Configurator_Export ACE_Proper_Import_Flag
#    define DANCE_CONFIGURATOR_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define DANCE_CONFIGURATOR_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* DANCE_CONFIGURATOR_BUILD_DLL */
#else /* DANCE_CONFIGURATOR_HAS_DLL == 1 */
#  define DAnCE_Configurator_Export
#  define DANCE_CONFIGURATOR_SINGLETON_DECLARATION(T)
#  define DANCE_CONFIGURATOR_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* DANCE_CONFIGURATOR_HAS_DLL == 1 */

// Set DANCE_CONFIGURATOR_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (DANCE_CONFIGURATOR_NTRACE)
#  if (ACE_NTRACE == 1)
#    define DANCE_CONFIGURATOR_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define DANCE_CONFIGURATOR_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !DANCE_CONFIGURATOR_NTRACE */

#if (DANCE_CONFIGURATOR_NTRACE == 1)
#  define DANCE_CONFIGURATOR_TRACE(X)
#else /* (DANCE_CONFIGURATOR_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define DANCE_CONFIGURATOR_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (DANCE_CONFIGURATOR_NTRACE == 1) */

#endif /* DANCEX11_CONFIGURATOR_EXPORT_H */

// End of auto generated file.
