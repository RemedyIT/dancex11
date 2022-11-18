// -*- C++ -*-
/**
 * @file installation_repository_manager.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_INSTALLATION_REPOSITORY_MANAGER_H
#define DANCEX11_INSTALLATION_REPOSITORY_MANAGER_H

#include "dance_artifact_installation_export.h"
#include "ace/Service_Object.h"
#include <string>
#include <istream>
#include <ostream>
#include <vector>

namespace DAnCEX11
  {
    class DAnCE_Artifact_Installation_Export InstallationRepository
      {
        public:
          using ref_t = std::shared_ptr<InstallationRepository>;
          using TLocations = std::vector<std::string>;

          virtual ~InstallationRepository () = default;

          virtual const std::string& plan () = 0;

          virtual const std::string& location () = 0;

          virtual std::istream* open_artifact (const std::string& location) = 0;
          virtual std::ostream* create_artifact (std::string& location, bool exclusive = false) = 0;
          virtual bool remove_artifact (const std::string& location) = 0;

          virtual bool has_artifact (const std::string& location) = 0;

          virtual TLocations artifacts (const std::string& folder = std::string()) = 0;

          virtual bool create_folder (std::string& location, bool exclusive = false) = 0;
          virtual bool remove_folder (const std::string& location) = 0;

          virtual bool has_folder (const std::string& location) = 0;

          virtual TLocations folders (const std::string& folder = std::string()) = 0;

          virtual bool remove () = 0;
      };

    class DAnCE_Artifact_Installation_Export InstallationRepositoryManager
      {
        public:
          using TRepositoryIds = InstallationRepository::TLocations;

          virtual ~InstallationRepositoryManager () = default;

          virtual void map_repository (const std::string& plan,
                                       const std::string& folder) = 0;

          virtual InstallationRepository::ref_t open_repository (const std::string& plan) = 0;
          virtual int remove_repository (const std::string& plan) = 0;

          virtual TRepositoryIds repositories () = 0;

        public:
          static InstallationRepositoryManager* instance ();

          static int register_manager_instance (InstallationRepositoryManager* mgr);
          static int deregister_manager_instance (InstallationRepositoryManager* mgr);

        private:
          static InstallationRepositoryManager* instance_;
      };

    class DAnCE_Artifact_Installation_Export InstallationRepositoryManagerSvc
      : public ACE_Service_Object
      {
        public:
          InstallationRepositoryManagerSvc () = default;
          ~InstallationRepositoryManagerSvc () override = default;

          /// Initialization hook.
          int init (int argc, ACE_TCHAR *argv[]) override;
          /// Deinitialization hook.
          int fini () override;

          virtual InstallationRepositoryManager* manager_instance () = 0;
      };
  }

#endif /* DANCEX11_INSTALLATION_REPOSITORY_MANAGER_H */
