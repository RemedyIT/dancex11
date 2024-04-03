// -*- C++ -*-
/**
 * @file    artifact_installation_handler_export.h
 * @author  Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DANCEX11_ARTIFACT_INSTALLATION_HANDLER_EXPORT_H
#define DANCEX11_ARTIFACT_INSTALLATION_HANDLER_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (ARTIFACT_INSTALLATION_HANDLER_HAS_DLL)
#  define ARTIFACT_INSTALLATION_HANDLER_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && ARTIFACT_INSTALLATION_HANDLER_HAS_DLL */

#if !defined (ARTIFACT_INSTALLATION_HANDLER_HAS_DLL)
#  define ARTIFACT_INSTALLATION_HANDLER_HAS_DLL 1
#endif /* ! ARTIFACT_INSTALLATION_HANDLER_HAS_DLL */

#if defined (ARTIFACT_INSTALLATION_HANDLER_HAS_DLL) && (ARTIFACT_INSTALLATION_HANDLER_HAS_DLL == 1)
#  if defined (ARTIFACT_INSTALLATION_HANDLER_BUILD_DLL)
#    define Artifact_Installation_Handler_Export ACE_Proper_Export_Flag
#    define ARTIFACT_INSTALLATION_HANDLER_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define ARTIFACT_INSTALLATION_HANDLER_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* ARTIFACT_INSTALLATION_HANDLER_BUILD_DLL */
#    define Artifact_Installation_Handler_Export ACE_Proper_Import_Flag
#    define ARTIFACT_INSTALLATION_HANDLER_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define ARTIFACT_INSTALLATION_HANDLER_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* ARTIFACT_INSTALLATION_HANDLER_BUILD_DLL */
#else /* ARTIFACT_INSTALLATION_HANDLER_HAS_DLL == 1 */
#  define Artifact_Installation_Handler_Export
#  define ARTIFACT_INSTALLATION_HANDLER_SINGLETON_DECLARATION(T)
#  define ARTIFACT_INSTALLATION_HANDLER_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* ARTIFACT_INSTALLATION_HANDLER_HAS_DLL == 1 */

#endif /* DANCEX11_ARTIFACT_INSTALLATION_HANDLER_EXPORT_H */

// End of auto generated file.
