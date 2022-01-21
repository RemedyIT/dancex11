// -*- C++ -*-
/**
 * @file installation_repository_manager_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "installation_repository_manager_impl.h"
#include "dancex11/logger/log.h"
#include "ace/Get_Opt.h"
#include "ace/Service_Config.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/Dirent.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_sys_stat.h"
#include <fstream>

namespace DAnCEX11
{
  /*
   *  InstallationRepositoryManagerSvc_Impl
   */

  ACE_STATIC_SVC_DEFINE (InstallationRepositoryManagerSvc_Impl,
                         ACE_TEXT ("InstallationRepositoryManager"),
                         ACE_SVC_OBJ_T,
                         &ACE_SVC_NAME (InstallationRepositoryManagerSvc_Impl),
                         ACE_Service_Type::DELETE_THIS
                          | ACE_Service_Type::DELETE_OBJ,
                         0)
  ACE_FACTORY_DEFINE (Installation_Repository_Manager, InstallationRepositoryManagerSvc_Impl)

  int InstallationRepositoryManagerSvc_Impl::init (int argc, ACE_TCHAR *argv[])
  {
    ACE_Get_Opt get_opts (argc,
                          argv,
                          ACE_TEXT("b:"),
                          0,
                          0,
                          ACE_Get_Opt::RETURN_IN_ORDER);

    int c;
    while ( (c = get_opts ()) != -1)
    {
      switch (c)
      {
        case 'b':
          DANCEX11_LOG_DEBUG ("InstallationRepositoryManagerSvc_Impl::init - " <<
                              "Provided default repository base dir: " << get_opts.opt_arg ());
          InstallationRepositoryManager_Impl::set_default_basedir (get_opts.opt_arg ());
          break;
      }
    }
    // base initialization
    return this->InstallationRepositoryManagerSvc::init (argc, argv);
  }

  InstallationRepositoryManager* InstallationRepositoryManagerSvc_Impl::manager_instance ()
  {
    return InstallationRepositoryManager_Impl::getInstance ();
  }

  int InstallationRepositoryManagerSvc_Impl::Initializer ()
  {
    return ACE_Service_Config::process_directive (ace_svc_desc_InstallationRepositoryManagerSvc_Impl);
  }

  /*
   *  InstallationRepositoryManager_Impl
   */

  const char * InstallationRepositoryManager_Impl::default_basedir = "./.dancex11_artifacts";
  const char * InstallationRepositoryManager_Impl::env_basedir_var = "DANCE_INSTALLATION_REPO_BASEDIR";

  InstallationRepositoryManager_Impl* InstallationRepositoryManager_Impl::getInstance ()
  {
    static InstallationRepositoryManager_Impl instance_;

    return std::addressof(instance_);
  }

  void InstallationRepositoryManager_Impl::set_default_basedir (const char * dir)
  {
    InstallationRepositoryManager_Impl::default_basedir = dir;
  }

  InstallationRepositoryManager_Impl::InstallationRepositoryManager_Impl ()
    : InstallationRepository_Impl ()
  {
    char * env_basedir = ACE_OS::getenv (env_basedir_var);
    if (env_basedir)
      {
        this->location_ = env_basedir;
      }
    else
      {
        this->location_ = default_basedir;
      }
    if (ACE_OS::access (this->location_.c_str (), F_OK) != 0)
      {
        ACE_OS::mkdir (this->location_.c_str ());
      }
    char* rp = ACE_OS::realpath (this->location_.c_str (), 0);
    this->location_ = rp;
    ACE_OS::free (rp);
  }

  void InstallationRepositoryManager_Impl::map_repository (
      const std::string& plan,
      const std::string& folder)
  {
    if (folder.empty ())
    {
      // remove any existing mapping
      repo_map_t::iterator it = this->repository_map_.find (plan);
      if (it != this->repository_map_.end ())
        this->repository_map_.erase (it);

    }
    else
    {
      // add mapping
      this->repository_map_[plan] = folder;
    }
  }

  InstallationRepository::ref_t
  InstallationRepositoryManager_Impl::open_repository (const std::string& plan)
  {
    InstallationRepository::ref_t r_reference (nullptr);
    std::string path = plan;
    // check for repo mapping
    repo_map_t::iterator it = this->repository_map_.find (plan);
    if (it != this->repository_map_.end ())
    {
      // use path mapping for repo
      path = it->second;
    }
    if (!this->has_folder (path))
    {
      if (!this->create_folder (path))
      {
        return r_reference;
      }
    }
    else
    {
      path = this->normalize (path);
    }

    InstallationRepository* rptr {};
    ACE_NEW_NORETURN (rptr,
                      InstallationRepository_Impl (plan, path));
    if (rptr)
    {
      r_reference = InstallationRepository::ref_t (rptr);
    }
    return r_reference;
  }

  int InstallationRepositoryManager_Impl::remove_repository (const std::string& plan)
  {
    std::string path = plan;
    // check for repo mapping
    repo_map_t::iterator it = this->repository_map_.find (plan);
    if (it != this->repository_map_.end ())
    {
      // use path mapping for repo
      path = it->second;
    }
    return this->remove_folder (path);
  }

  InstallationRepositoryManager::TRepositoryIds
  InstallationRepositoryManager_Impl::repositories ()
  {
    return this->folders ();
  }

  /*
   *  InstallationRepository_Impl
   */
  InstallationRepository_Impl::InstallationRepository_Impl (
      std::string plan,
      std::string location)
    : location_ (std::move(location)),
      plan_ (std::move(plan))
  {
  }

  std::string InstallationRepository_Impl::normalize (const std::string& location)
  {
    // location must either be an absolute path with a base matching
    // the repo location or a relative path from the repo location
    // check and return an absolute path in either case
    if (location.find (this->location_) != 0)
      {
        // should be relative path
        // this will fail later on when the path is not a correct relative
        // path
        return this->make_full_path (location);
      }
    return location;
  }

  std::string InstallationRepository_Impl::make_full_path (const std::string& location)
  {
    return this->join (this->location_, location);
  }

  std::string InstallationRepository_Impl::join (const std::string& base,
                                                 const std::string& location)
  {
    return location.empty () ?
            base :
            base + ACE_DIRECTORY_SEPARATOR_STR_A + location;
  }

  std::string InstallationRepository_Impl::get_folder (const std::string& location)
  {
    std::string::size_type n = location.rfind (ACE_DIRECTORY_SEPARATOR_STR_A);
    if (n != std::string::npos)
      {
        return location.substr (0, n);
      }
    return std::string();
  }

  const std::string& InstallationRepository_Impl::plan ()
  {
    return this->plan_;
  }

  const std::string& InstallationRepository_Impl::location ()
  {
    return this->location_;
  }

  std::istream* InstallationRepository_Impl::open_artifact (const std::string& location)
  {
    std::unique_ptr<ifstream> safe_ifs;
    // check that artifact actually exists
    std::string path = this->normalize (location);
    if (this->has_artifact (path))
    {
      ifstream* ifs {};
      ACE_NEW_RETURN (ifs,
                      ifstream (path.c_str (),
                                std::ios_base::in |
                                    std::ios_base::binary),
                      0);
      safe_ifs = std::unique_ptr<ifstream>(ifs);
      if (!(*ifs))
      {
        return nullptr;
      }
    }
    return safe_ifs.release ();
  }

  std::ostream* InstallationRepository_Impl::create_artifact (std::string& location, bool exclusive)
  {
    std::string path = this->normalize (location);
    ofstream* ofs {};
    // check that no artifact or folder already exists with this path
    if (ACE_OS::access (path.c_str (), F_OK) != 0)
    {
      std::string folder = this->get_folder (path);

      if (!this->has_folder (folder))
      {
        if (!this->create_folder (folder))
        {
          return ofs;
        }
      }
    }
    else
    {
      // if we want exclusiveness or its not an artifact (but a folder probably)
      // it's a nogo
      if (exclusive || !this->has_artifact (path))
        return ofs;
    }

    ACE_NEW_RETURN (ofs,
                    ofstream (path.c_str (),
                              std::ios_base::out |
                                std::ios_base::binary),
                    0);
    std::unique_ptr<ofstream> safe_ofs (ofs);
    if (!(*ofs))
    {
      return 0;
    }

    // everything succeeded
    location = path;
    return safe_ofs.release ();
  }

  bool InstallationRepository_Impl::remove_artifact (const std::string& location)
  {
    std::string path = this->normalize (location);
    return ACE_OS::unlink (path.c_str ()) == 0;
  }

  bool InstallationRepository_Impl::has_artifact (const std::string& location)
  {
    std::string path = this->normalize (location);
    ACE_stat stat;
    if (ACE_OS::stat (path.c_str (), &stat) == 0)
    {
      if ((stat.st_mode & S_IFMT) == S_IFREG)
      {
        return true;
      }
    }
    return false;
  }

  InstallationRepository::TLocations
  InstallationRepository_Impl::artifacts (const std::string& folder)
  {
    TLocations locs;

    std::string path = this->normalize (folder);
    ACE_Dirent dir (ACE_TEXT_CHAR_TO_TCHAR (path.c_str ()));
    ACE_DIRENT* dp {};
    while ((dp = dir.read ()) != 0)
    {
#if defined (ACE_HAS_TCHAR_DIRENT)
      std::string entry = this->join (path, ACE_TEXT_ALWAYS_CHAR (dp->d_name));
#else
      std::string entry = this->join (path, dp->d_name);
#endif
      if (this->has_artifact (entry))
      {
        locs.push_back (entry);
      }
    }

    return locs;
  }

  bool InstallationRepository_Impl::create_folder (std::string& location, bool exclusive)
  {
    std::string path = this->normalize (location);
    // check that no artifact or folder already exists with this path
    if (ACE_OS::access (path.c_str (), F_OK) != 0)
    {
      // check existence of parent folder
      std::string folder = this->get_folder (path);
      if (!this->has_folder (folder))
      {
        if (!this->create_folder (folder))
        {
          return false;
        }
      }

      // create folder
      if (ACE_OS::mkdir (path.c_str ()) != 0)
      {
        return false;
      }
    }
    else
    {
      // if we want exclusiveness or its not a folder (but an artifact probably)
      // it's a nogo
      if (exclusive || !this->has_folder (path))
        return false;

      // it's ok
    }

    // everything succeeded
    location = path;
    return true;
  }

  bool InstallationRepository_Impl::remove_folder (const std::string& location)
  {
    std::string path = this->normalize (location);
    if (this->has_folder (path))
    {
      TLocations locs = this->artifacts (path);
      for (TLocations::iterator itloc = locs.begin ();
          itloc != locs.end ();
          ++itloc)
      {
        if (!this->remove_artifact (*itloc))
        {
          return false;
        }
      }

      locs = this->folders (path);
      for (TLocations::iterator itloc = locs.begin ();
          itloc != locs.end ();
          ++itloc)
      {
        if (!this->remove_folder (*itloc))
        {
          return false;
        }
      }

      return ACE_OS::rmdir (path.c_str ()) == 0;
    }
    return false;
  }

  bool InstallationRepository_Impl::remove ()
  {
    return this->remove_folder (this->location_);
  }

  bool InstallationRepository_Impl::has_folder (const std::string& location)
  {
    std::string path = this->normalize (location);
    ACE_stat stat;
    if (ACE_OS::stat (path.c_str (), &stat) == 0)
    {
      if ((stat.st_mode & S_IFMT) == S_IFDIR)
      {
        return true;
      }
    }
    return false;
  }

  InstallationRepository::TLocations
  InstallationRepository_Impl::folders (const std::string& folder)
  {
    TLocations locs;

    std::string path = this->normalize (folder);
    ACE_Dirent dir (ACE_TEXT_CHAR_TO_TCHAR (path.c_str ()));
    ACE_DIRENT* dp {};
    while ((dp = dir.read ()) != 0)
    {
#if defined (ACE_HAS_TCHAR_DIRENT)
      std::string entry = this->join (path, ACE_TEXT_ALWAYS_CHAR (dp->d_name));
#else
      std::string entry = this->join (path, dp->d_name);
#endif
      if (this->has_folder (entry))
      {
        locs.push_back (entry);
      }
    }

    return locs;
  }

}
