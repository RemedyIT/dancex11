// -*- C++ -*-
/**
 * @file file_installation_handler.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "file_installation_handler.h"
#include "installation_repository_manager.h"
#include "dancex11/logger/log.h"
#include "ace/ACE.h"
#include "ace/Service_Config.h"
#include "ace/OS_NS_unistd.h"
#include <fstream>

namespace DAnCEX11
  {
    /*
     *  FileInstallationHandlerSvc
     */
    FileInstallationHandler* FileInstallationHandler::getInstance ()
    {
      static FileInstallationHandler instance_;

      return std::addressof(instance_);
    }

    ACE_STATIC_SVC_DEFINE (FileInstallationHandlerSvc,
                           ACE_TEXT ("FileInstallationHandler"),
                           ACE_SVC_OBJ_T,
                           &ACE_SVC_NAME (FileInstallationHandlerSvc),
                           ACE_Service_Type::DELETE_THIS
                           | ACE_Service_Type::DELETE_OBJ,
                           0)
    ACE_FACTORY_DEFINE (Artifact_Installation_Handler, FileInstallationHandlerSvc)


    ArtifactInstallationHandler* FileInstallationHandlerSvc::handler_instance ()
    {
      return FileInstallationHandler::getInstance ();
    }

    int FileInstallationHandlerSvc::Initializer ()
    {
      return ACE_Service_Config::process_directive (ace_svc_desc_FileInstallationHandlerSvc);
    }

    /*
     *  FileInstallationHandler
     */

    const std::string FileInstallationHandler::protocol ("file");

    const std::string& FileInstallationHandler::protocol_prefix ()
    {
      return FileInstallationHandler::protocol;
    }

    void FileInstallationHandler::initialize ()
    {
    }

    void FileInstallationHandler::clear ()
    {
    }

    void FileInstallationHandler::install (const std::string& plan_uuid,
                                           std::string& location,
                                           const TPropertyMap& properties)
    {
      InstallationRepository::ref_t repo (
        InstallationRepositoryManager::instance ()->open_repository (plan_uuid));

      if (repo)
        {
          ArtifactInstallationProperties propreader (properties, this->protocol_prefix());

          ifstream* ifs {};
          ACE_NEW_NORETURN (ifs,
                            ifstream (location.c_str (),
                                      std::ios_base::in |
                                      std::ios_base::binary));
          std::unique_ptr<ifstream> pifs (ifs);
          if (pifs.get ()!=0 && (*pifs))
            {
              std::string path = propreader.get_property (
                  ArtifactInstallationProperties::PROP_NAME);
              if (path.empty ())
                path = this->basename (location);
              bool exclusive = false;
              const std::string& propval = propreader.get_property (
                  ArtifactInstallationProperties::PROP_EXCLUSIVE);
              exclusive = (!propval.empty () && (propval == "yes" || propval == "true"));
              std::unique_ptr<ostream> pofs (repo->create_artifact(path, exclusive));
              if (pofs.get ()!=0 && (*pofs))
                {
                  // copy artifact content
                  (*pofs) << pifs->rdbuf ();
                  // check state
                  if (!(*pofs))
                    {
                      std::string err = "failed to copy artifact from ";
                      err += location;
                      err += " to ";
                      err += path;
                      throw Deployment::PlanError (plan_uuid.c_str (), err.c_str ());
                    }
                  location = path;
                }
              else
                {
                  std::string err = "failed to create repository artifact version for ";
                  err += path;
                  throw Deployment::PlanError (plan_uuid.c_str (), err.c_str ());
                }
            }
          else
            {
              std::string err = "failed to open artifact location : ";
              err += location;
              throw Deployment::PlanError (plan_uuid.c_str (), err.c_str ());
            }

        }
      else
        {
          throw Deployment::PlanError (plan_uuid.c_str (), "failed to open installation repository");
        }
    }

    void FileInstallationHandler::remove (const std::string& plan_uuid,
                                          const std::string& location)
    {
      InstallationRepository::ref_t repo (
        InstallationRepositoryManager::instance ()->open_repository (plan_uuid));

      if (repo)
        {
          if (repo->has_artifact (location))
            {
              if (!repo->remove_artifact (location))
                {
                  std::string err = "failed to remove artifact : ";
                  err += location;
                  throw Deployment::PlanError (plan_uuid.c_str (), err.c_str ());
                }
            }
          else
            {
              std::string err = "cannot remove non-existing artifact : ";
              err += location;
              throw Deployment::PlanError (plan_uuid.c_str (), err.c_str ());
            }
        }
      else
        {
          throw Deployment::PlanError (plan_uuid.c_str (), "failed to open installation repository");
        }
    }

  }
