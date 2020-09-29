// -*- C++ -*-
/**
 * @file installation_repository_manager_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#ifndef DANCEX11_INSTALLATION_REPOSITORY_MANAGER_IMPL_H
#define DANCEX11_INSTALLATION_REPOSITORY_MANAGER_IMPL_H

#include "installation_repository_manager.h"
#include "installation_repository_manager_export.h"

#include <map>

namespace DAnCEX11
{
  class Installation_Repository_Manager_Export InstallationRepository_Impl
    : public InstallationRepository
  {
  public:
    virtual ~InstallationRepository_Impl () = default;

    const std::string& plan () override;

    const std::string& location () override;

    std::istream* open_artifact (const std::string& location) override;
    std::ostream* create_artifact (std::string& location, bool exclusive = false) override;
    bool remove_artifact (const std::string& location) override;

    bool has_artifact (const std::string& location) override;

    TLocations artifacts (const std::string& folder = std::string()) override;

    bool create_folder (std::string& location, bool exclusive = false) override;
    bool remove_folder (const std::string& location) override;

    bool has_folder (const std::string& location) override;

    TLocations folders (const std::string& folder = std::string()) override;

    bool remove () override;

  protected:
    std::string normalize (const std::string& location);
    std::string make_full_path (const std::string& location);
    std::string get_folder (const std::string& location);
    std::string join (const std::string& base, const std::string& location);

    InstallationRepository_Impl () = default;

    InstallationRepository_Impl (std::string plan,
                                 std::string location);

    friend class InstallationRepositoryManager_Impl;

    std::string location_;
    std::string plan_;
  };

  class Installation_Repository_Manager_Export InstallationRepositoryManager_Impl
    : public InstallationRepositoryManager,
      private InstallationRepository_Impl
  {
  public:
    virtual ~InstallationRepositoryManager_Impl () = default;

    void map_repository (const std::string& plan,
                         const std::string& folder) override;

    InstallationRepository::ref_t open_repository (const std::string& plan) override;
    int remove_repository (const std::string& plan) override;

    TRepositoryIds repositories () override;

    static void set_default_basedir (const char *);

    static InstallationRepositoryManager_Impl* getInstance ();

  private:
    InstallationRepositoryManager_Impl ();

    using repo_map_t = std::map<std::string, std::string>;
    repo_map_t repository_map_;

    static const char * default_basedir;
    static const char * env_basedir_var;
  };

  class Installation_Repository_Manager_Export InstallationRepositoryManagerSvc_Impl
    : public InstallationRepositoryManagerSvc
  {
  public:
    InstallationRepositoryManagerSvc_Impl () = default;
    virtual ~InstallationRepositoryManagerSvc_Impl () = default;

    /// Initializes handler on dynamic loading.
    int init (int argc, ACE_TCHAR *argv[]) override;

    InstallationRepositoryManager* manager_instance () override;

    static int Initializer ();
  };

  ACE_STATIC_SVC_DECLARE (InstallationRepositoryManagerSvc_Impl)
  ACE_FACTORY_DECLARE (Installation_Repository_Manager, InstallationRepositoryManagerSvc_Impl)
}

#endif /* DANCEX11_INSTALLATION_REPOSITORY_MANAGER_IMPL_H */
