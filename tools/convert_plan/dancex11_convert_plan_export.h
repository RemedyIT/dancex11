// -*- C++ -*-
/**
 * @file    dancex11_convert_plan_export.h
 * @author  Martin Corino
 *
 * @brief   Generated
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_CONVERT_PLAN_EXPORT_H
#define DANCEX11_CONVERT_PLAN_EXPORT_H

#include "tao/x11/taox11_export_macros.h"

#if defined (TAOX11_AS_STATIC_LIBS) && !defined (DANCEX11_CONVERT_PLAN_HAS_DLL)
#  define DANCEX11_CONVERT_PLAN_HAS_DLL 0
#endif /* TAOX11_AS_STATIC_LIBS && DANCEX11_CONVERT_PLAN_HAS_DLL */

#if !defined (DANCEX11_CONVERT_PLAN_HAS_DLL)
#  define DANCEX11_CONVERT_PLAN_HAS_DLL 1
#endif /* ! DANCEX11_CONVERT_PLAN_HAS_DLL */

#if defined (DANCEX11_CONVERT_PLAN_HAS_DLL) && (DANCEX11_CONVERT_PLAN_HAS_DLL == 1)
#  if defined (DANCEX11_CONVERT_PLAN_BUILD_DLL)
#    define DAnCEX11_Convert_Plan_Export TAOX11_Proper_Export_Flag
#    define DANCEX11_CONVERT_PLAN_SINGLETON_DECLARATION(T) TAOX11_EXPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_CONVERT_PLAN_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) TAOX11_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* DANCEX11_CONVERT_PLAN_BUILD_DLL */
#    define DAnCEX11_Convert_Plan_Export TAOX11_Proper_Import_Flag
#    define DANCEX11_CONVERT_PLAN_SINGLETON_DECLARATION(T) TAOX11_IMPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_CONVERT_PLAN_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) TAOX11_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* DANCEX11_CONVERT_PLAN_BUILD_DLL */
#else /* DANCEX11_CONVERT_PLAN_HAS_DLL == 1 */
#  define DAnCEX11_Convert_Plan_Export
#  define DANCEX11_CONVERT_PLAN_SINGLETON_DECLARATION(T)
#  define DANCEX11_CONVERT_PLAN_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* DANCEX11_CONVERT_PLAN_HAS_DLL == 1 */

#endif /* DANCEX11_CONVERT_PLAN_EXPORT_H */

