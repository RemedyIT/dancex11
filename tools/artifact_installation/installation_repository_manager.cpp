// -*- C++ -*-
/**
 * @file installation_repository_manager.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "installation_repository_manager.h"
#include "dancex11/logger/log.h"

namespace DAnCEX11
{
  /*
   *  InstallationRepositoryManager
   */

  InstallationRepositoryManager* InstallationRepositoryManager::instance_ = 0;

  InstallationRepositoryManager* InstallationRepositoryManager::instance ()
  {
    DANCEX11_LOG_TRACE ("InstallationRepository::instance");
    return instance_;
  }

  int InstallationRepositoryManager::register_manager_instance (InstallationRepositoryManager* mgr)
  {
    DANCEX11_LOG_TRACE ("InstallationRepository::register_manager_instance");

    if (mgr != 0)
      {
        DANCEX11_LOG_TRACE ("InstallationRepositoryManager::register_manager_instance -" << mgr);

        instance_ = mgr;
        return 0;
      }

    return -1;
  }

  int InstallationRepositoryManager::deregister_manager_instance (InstallationRepositoryManager* mgr)
  {
    DANCEX11_LOG_TRACE ("InstallationRepository::deregister_manager_instance");

    if (instance_ == mgr)
      {
        DANCEX11_LOG_TRACE ("InstallationRepositoryManager::deregister_manager_instance -" << mgr);
        instance_ = 0;
        return 0;
      }
    return -1;
  }

  /*
   *  InstallationRepositoryManagerSvc
   */
  /// Initialization hook.
  int InstallationRepositoryManagerSvc::init (int /*argc*/, ACE_TCHAR * /*argv*/[])
  {
    DANCEX11_LOG_TRACE ("InstallationRepositoryManagerSvc::init");
    return InstallationRepositoryManager::register_manager_instance (
      this->manager_instance ());
  }

  /// Deinitialization hook.
  int InstallationRepositoryManagerSvc::fini ()
  {
    DANCEX11_LOG_TRACE ("InstallationRepositoryManagerSvc::fini");
    return InstallationRepositoryManager::deregister_manager_instance (
      this->manager_instance ());
  }
}
