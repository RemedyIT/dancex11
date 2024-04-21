// -*- C++ -*-
/**
 * @file    dancex11_plan_loader_stub_export.h
 * @author  Martin Corino
 *
 * @brief   Generated
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef __RIDL_DANCEX11_PLAN_LOADER_STUB_EXPORT_H_INCLUDED__
#define __RIDL_DANCEX11_PLAN_LOADER_STUB_EXPORT_H_INCLUDED__

#include "tao/x11/base/taox11_export_macros.h"

#if defined (TAOX11_AS_STATIC_LIBS) && !defined (DANCEX11_PLAN_LOADER_STUB_HAS_DLL)
#  define DANCEX11_PLAN_LOADER_STUB_HAS_DLL 0
#endif /* TAOX11_AS_STATIC_LIBS && DANCEX11_PLAN_LOADER_STUB_HAS_DLL */

#if !defined (DANCEX11_PLAN_LOADER_STUB_HAS_DLL)
#  define DANCEX11_PLAN_LOADER_STUB_HAS_DLL 1
#endif /* ! DANCEX11_PLAN_LOADER_STUB_HAS_DLL */

#if defined (DANCEX11_PLAN_LOADER_STUB_HAS_DLL) && (DANCEX11_PLAN_LOADER_STUB_HAS_DLL == 1)
#  if defined (DANCEX11_PLAN_LOADER_STUB_BUILD_DLL)
#    define DANCEX11_PLAN_LOADER_STUB_Export TAOX11_Proper_Export_Flag
#    define DANCEX11_PLAN_LOADER_STUB_SINGLETON_DECLARATION(T) TAOX11_EXPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_PLAN_LOADER_STUB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) TAOX11_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* DANCEX11_PLAN_LOADER_STUB_BUILD_DLL */
#    define DANCEX11_PLAN_LOADER_STUB_Export TAOX11_Proper_Import_Flag
#    define DANCEX11_PLAN_LOADER_STUB_SINGLETON_DECLARATION(T) TAOX11_IMPORT_SINGLETON_DECLARATION (T)
#    define DANCEX11_PLAN_LOADER_STUB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) TAOX11_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* DANCEX11_PLAN_LOADER_STUB_BUILD_DLL */
#else /* DANCEX11_PLAN_LOADER_STUB_HAS_DLL == 1 */
#  define DANCEX11_PLAN_LOADER_STUB_Export
#  define DANCEX11_PLAN_LOADER_STUB_SINGLETON_DECLARATION(T)
#  define DANCEX11_PLAN_LOADER_STUB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* DANCEX11_PLAN_LOADER_STUB_HAS_DLL == 1 */

#endif /* __RIDL_DANCEX11_PLAN_LOADER_STUB_EXPORT_H_INCLUDED__ */
