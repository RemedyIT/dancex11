// -*- C++ -*-
/**
 * @file    dancex11_locality_configuration_export.h
 * @author  Johnny Willemsen
 *
 * @brief   Generated
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */
#ifndef DANCEX11_LOCALITY_CONFIGURATION_EXPORT_H
#define DANCEX11_LOCALITY_CONFIGURATION_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL)
#  define DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL */

#if !defined (DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL)
#  define DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL 1
#endif /* ! DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL */

#if defined (DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL) && (DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL == 1)
#  if defined (DANCEX11_LOCALITY_CONFIGURATION_BUILD_DLL)
#    define DAnCEX11_Locality_Configuration_Export ACE_Proper_Export_Flag
#    define DANCEX11_LOCALITY_CONFIGURATION_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_LOCALITY_CONFIGURATION_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* DANCEX11_LOCALITY_CONFIGURATION_BUILD_DLL */
#    define DAnCEX11_Locality_Configuration_Export ACE_Proper_Import_Flag
#    define DANCEX11_LOCALITY_CONFIGURATION_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_LOCALITY_CONFIGURATION_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* DANCEX11_LOCALITY_CONFIGURATION_BUILD_DLL */
#else /* DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL == 1 */
#  define DAnCEX11_Locality_Configuration_Export
#  define DANCEX11_LOCALITY_CONFIGURATION_SINGLETON_DECLARATION(T)
#  define DANCEX11_LOCALITY_CONFIGURATION_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* DANCEX11_LOCALITY_CONFIGURATION_HAS_DLL == 1 */

// Set DANCEX11_LOCALITY_CONFIGURATION_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (DANCEX11_LOCALITY_CONFIGURATION_NTRACE)
#  if (ACE_NTRACE == 1)
#    define DANCEX11_LOCALITY_CONFIGURATION_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define DANCEX11_LOCALITY_CONFIGURATION_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !DANCEX11_LOCALITY_CONFIGURATION_NTRACE */

#if (DANCEX11_LOCALITY_CONFIGURATION_NTRACE == 1)
#  define DANCEX11_LOCALITY_CONFIGURATION_TRACE(X)
#else /* (DANCEX11_LOCALITY_CONFIGURATION_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define DANCEX11_LOCALITY_CONFIGURATION_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (DANCEX11_LOCALITY_CONFIGURATION_NTRACE == 1) */

#endif /* DANCEX11_LOCALITY_CONFIGURATION_EXPORT_H */

// End of auto generated file.
