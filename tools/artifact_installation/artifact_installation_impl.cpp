// -*- C++ -*-
/**
 * @file artifact_installation_impl.cpp
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "artifact_installation_impl.h"
#include "installation_repository_manager.h"
#include "dancex11/logger/log.h"
#include "dancex11/core/dancex11_utility.h"

namespace DAnCEX11
  {
    /*
     *  ArtifactRegistry
     */

    ArtifactRegistry::ArtifactRegistry (TCONDITION& condition, bool locked)
      : condition_ (condition),
        locked_ (locked),
        install_count_ (0)
    {
      DANCEX11_LOG_TRACE ("ArtifactRegistry::constructor");
    }

    ArtifactRegistry::~ArtifactRegistry ()
    {
      DANCEX11_LOG_TRACE ("ArtifactRegistry::destructor");
    }

    /*
     *  ArtifactInstallation_Impl
     */

    ArtifactInstallation_Impl::THandlerMap ArtifactInstallation_Impl::handlers_;
    ArtifactInstallation_Impl::TLOCK ArtifactInstallation_Impl::handler_lock_;
    IDL::traits<::DAnCEX11::ArtifactInstallation>::ref_type ArtifactInstallation_Impl::instance_;

    IDL::traits<::DAnCEX11::ArtifactInstallation>::ref_type
    ArtifactInstallation_Impl::instance ()
    {
      if (!instance_)
      {
        instance_ = CORBA::make_reference<DAnCEX11::ArtifactInstallation_Impl> ();
      }
      return instance_;
    }

    ArtifactInstallation_Impl::ArtifactInstallation_Impl ()
      : IDL::traits<::DAnCEX11::ArtifactInstallation>::base_type (),
        artifacts_condition_ (artifacts_lock_)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::constructor");
    }

    ArtifactInstallation_Impl::~ArtifactInstallation_Impl ()
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::destructor");
    }

    void ArtifactInstallation_Impl::initialize ()
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::initialize");

      ACE_GUARD_REACTION (TLOCK,
                          handler_guard_,
                          handler_lock_,
                          throw Deployment::PlanError (
                            "ArtifactInstallation_Impl::initialize",
                            "ArtifactInstallation handler lock failed"));

      THandlerMap::iterator it = handlers_.end ();
      for (it = handlers_.begin ();
           it != handlers_.end ();
           ++it)
        {
          (*it).second->initialize ();
        }
    }

    void ArtifactInstallation_Impl::clear ()
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::clear");

      ACE_GUARD_REACTION (TLOCK,
                          handler_guard_,
                          handler_lock_,
                          throw Deployment::PlanError (
                            "ArtifactInstallation_Impl::clear",
                            "ArtifactInstallation handler lock failed"));

      THandlerMap::iterator it = handlers_.end ();
      for (it = handlers_.begin ();
           it != handlers_.end ();
           ++it)
        {
          (*it).second->clear ();
        }
    }

    void ArtifactInstallation_Impl::prepare (
        const ::Deployment::DeploymentPlan& plan)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::prepare");

      ACE_GUARD_REACTION (TLOCK,
                          handler_guard_,
                          handler_lock_,
                          throw Deployment::PlanError (
                            "ArtifactInstallation_Impl::prepare",
                            "ArtifactInstallation handler lock failed"));

      std::string plan_repopath;
      if (Utility::get_property_value ("nl.remedy.it.DnCX11.ArtifactRepo.Path",
                                       plan.infoProperty (), plan_repopath))
      {
        InstallationRepositoryManager *repoman =
            InstallationRepositoryManager::instance ();
        if (!repoman)
        {
          throw Deployment::PlanError (
              "ArtifactInstallation_Impl::prepare",
              "No InstallationRepositoryManager implementation installed.");
        }

        DANCEX11_LOG_DEBUG ("ArtifactInstallation_Impl::prepare - " <<
                            "mapping repository path for " << plan.UUID () <<
                            " to [" << plan_repopath << "]");

        repoman->map_repository (plan.UUID (), plan_repopath);
      }
    }

    void ArtifactInstallation_Impl::install (
      const std::string& plan_uuid,
      const ::Deployment::ArtifactDeploymentDescription & artifact)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::install");

      std::string const name = artifact.name ();

      // allocate (and lock) the artifact registry for the given plan
      ArtifactRegistry::Guard ar_guard (this->allocate_artifact_registry (plan_uuid, name));

      DANCEX11_LOG_DEBUG ("ArtifactInstallation_Impl::install - " <<
                          "installation request for plan " <<
                          plan_uuid <<
                          ", artifact " <<
                          name);

      // check to see if artifact has already been previously installed
      if (ar_guard->install_count () > 0)
        {
          DANCEX11_LOG_DEBUG ("ArtifactInstallation_Impl::install - " <<
                              "detected previously installed plan " <<
                              plan_uuid <<
                              ", artifact " <<
                              name <<
                              " @ location " <<
                              ar_guard->location ());

          // just increment install count and return
          ar_guard->increment_install_count ();
          return;
        }

      // extract artifact properties
      ArtifactInstallationHandler::TPropertyMap properties;
      for (uint32_t i=0; i < artifact.execParameter ().size () ;++i)
        {
          Deployment::Property property = artifact.execParameter ()[i];
          std::string prop_val;
          if (property.value () >>= prop_val)
            {
              properties[property.name ()] = prop_val;
            }
          else
            {
              // @will    this appears to be my first non fatal error
              DANCEX11_LOG_ERROR  ("ArtifactInstallation_Impl::install - " <<
                                   "failed to extract property " << property.name () <<
                                   " for plan " << plan_uuid <<
                                   ", artifact " << name <<
                                   "; property will be ignored");
            }
        }

      // attempt to install the artifact going through the alternative locations
      uint32_t loc_n = 0;
      while (loc_n < artifact.location ().size ())
        {
          try
            {
              std::string location = artifact.location ()[loc_n];

              DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::install -" <<
                                  " installation attempt for plan " << plan_uuid <<
                                  ", artifact " << name <<
                                  " @ location[" << loc_n << "] " <<
                                  location);

              // parse protocol stack & path; URI = <protocol>:[<protocol>:[...]]//<path>
              TProtocolStack prot_stack;
              this->parse_uri (plan_uuid, location, prot_stack);

              // if no protocols specified no further action necessary
              if (!prot_stack.empty ())
                {
                  // process protocol handlers from last to first
                  while (!prot_stack.empty ())
                    {
                      std::string protocol = prot_stack.top ();

                      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::install -" <<
                                          " handling protocol " << protocol <<
                                          " for artifact " << name <<
                                          " @ location " << location <<
                                          " from plan " << plan_uuid);

                      // find protocol handler and install
                      if (!this->install_i (plan_uuid, protocol, location, properties))
                        {
                          // we are about to throw an exception,
                          // so this is a terminal error message
                          DANCEX11_LOG_ERROR ("ArtifactInstallation_Impl::install -" <<
                                              " unknown protocol " << protocol <<
                                              " for artifact " << name <<
                                              " from plan " << plan_uuid);

                          std::string err ("unknown installation protocol ");
                          err += protocol;
                          throw Deployment::PlanError (plan_uuid,
                                                       err.c_str ());
                        }

                      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::install -" <<
                                          " protocol " << protocol <<
                                          " installed artifact " << name <<
                                          " @ location " << location <<
                                          " from plan " << plan_uuid <<
                                          " to location " << location);

                      // register installed version for later removal
                      ar_guard->versions ().push_back (
                          ArtifactRegistry::Version(protocol, location));

                      // remove handled protocol
                      prot_stack.pop ();
                    }

                  // check property settings and clean installation intermediates if required
                  ArtifactInstallationProperties property_reader (properties);
                  const std::string& propval = property_reader.get_property (
                    ArtifactInstallationProperties::PROP_KEEP_INTERMEDIATES);
                  if (propval != "yes" && propval != "true")
                    {
                      this->remove_intermediates (plan_uuid, name, *ar_guard);
                    }
                }
              else
                {
                  // just register the artifact without protocol
                  ar_guard->versions ().push_back (
                      ArtifactRegistry::Version(std::string(),location));
                }

              // we successfully installed the artifact so increment the install count
              ar_guard->increment_install_count ();

              DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::install - " <<
                                  "installed plan " << plan_uuid <<
                                  ", artifact " << name <<
                                  " @ location " << location);

              return; // artifact successfully installed
            }
          catch (const Deployment::PlanError& ex)
            {
              // since we do not yet know if this is fatal or not, I'm indicating this
              // is non fatal
              DANCEX11_LOG_ERROR ("ArtifactInstallation_Impl::install -" <<
                                  " installation failed for plan " << plan_uuid <<
                                  ", artifact " << name <<
                                  "@ location[" << loc_n << "] " <<
                                  artifact.location ()[loc_n] <<
                                  " : " << ex.reason ());

              // artifact installation failed for this location; clean up anything left behind
              this->remove_i (plan_uuid, name.c_str (), &ar_guard, false);

              // if we have alternates we continue otherwise we fail finally
              if (++loc_n >= artifact.location ().size ())
                {
                  throw; // rethrow exception
                }

              // try alternate
            }
        }
    }

    void ArtifactInstallation_Impl::parse_uri (const std::string& plan_uuid,
        std::string& location,
        TProtocolStack& protstack)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::parse_uri");

      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::parse_uri -" <<
                          " parsing location " << location <<
                          " for plan " << plan_uuid);

      std::string loctmp = location;
      std::string::size_type p;
      while ((p = loctmp.find (':')) != std::string::npos)
        {
          if (p == 0)
            {
              std::string err ("malformed location ");
              err += location;
              throw Deployment::PlanError (plan_uuid,
                                           err.c_str ());
            }

          std::string const prot = loctmp.substr (0, p);
          protstack.push (prot);

          DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::parse_uri -" <<
                              " parsed protocol " << prot <<
                              " from location " << location <<
                              " for plan " << plan_uuid);

          loctmp = loctmp.substr (p+1);
        }

      // did we find any protocol specifier?
      if (!protstack.empty ())
        {
          // check for path separator
          p = loctmp.find ("//");
          if (p != 0)
            {
              std::string err ("malformed location ");
              err += location;
              throw Deployment::PlanError (plan_uuid,
                                           err.c_str ());
            }

          // remove path separator
          location = loctmp.substr (2);
        }
    }

    bool ArtifactInstallation_Impl::install_i (const std::string& plan_uuid,
        const std::string& protocol,
        std::string& location,
        const TPropertyMap& properties)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::install_i");

      ACE_GUARD_REACTION (TLOCK,
                          handler_guard_,
                          handler_lock_,
                          throw Deployment::PlanError (
                            plan_uuid.c_str (),
                            "ArtifactInstallation handler lock failed"));

      // find protocol handler
      THandlerMap::iterator ith = handlers_.find (protocol);
      if (ith != handlers_.end ())
        {
          // activate protocol installation handler
          // after successful installation the protocol handler will update
          // the location to the installed version
          ith->second->install (plan_uuid, location, properties);
          return true;
        }
      return false;
    }

    ArtifactRegistry*
    ArtifactInstallation_Impl::allocate_artifact_registry (const std::string& plan_uuid,
                                                           const std::string& name)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::allocate_artifact_registry");

      ACE_GUARD_REACTION (TLOCK,
                          artifacts_guard_,
                          artifacts_lock_,
                          throw Deployment::PlanError (
                            plan_uuid.c_str (),
                            "artifacts lock failed"));

      while (true)
        {
          // get existing or create one
          ArtifactRegistry* ar {};
          TArtifactsMap& plan_map = this->artifacts_[plan_uuid];
          TArtifactsMap::iterator it_art = plan_map.find (name);
          if (it_art == plan_map.end ())
            {
              ACE_NEW_NORETURN (ar, ArtifactRegistry (this->artifacts_condition_,
                                                      true));
              if (ar == 0)
                {
                  throw Deployment::PlanError (
                    plan_uuid.c_str (),
                    "out of memory");
                }
              plan_map[name] = ar;
              return ar; // we created it locked so we're ready
            }
          else
            {
              ar = it_art->second;
            }

          // lock
          if (ar->is_locked ())
            {
              // registry is locked by another thread so wait
              if (this->artifacts_condition_.wait () != 0)
                {
                  throw Deployment::PlanError (
                    plan_uuid.c_str (),
                    "artifact registry lock failed");
                }
            }
          else
            {
              // we're free to set the lock
              ar->set_locked ();
              return ar;
            }
        }
      return 0;
    }

    ArtifactRegistry*
    ArtifactInstallation_Impl::lock_artifact_registry (const std::string& plan_uuid,
                                                       const std::string& name)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::lock_artifact_registry");

      ACE_GUARD_REACTION (TLOCK,
                          artifacts_guard_,
                          artifacts_lock_,
                          throw Deployment::PlanError (
                            plan_uuid.c_str (),
                            "artifacts lock failed"));

      while (true)
        {
          // get existing
          TArtifactsMap& plan_map = this->artifacts_[plan_uuid];
          TArtifactsMap::iterator it_art = plan_map.find (name);
          if (it_art == plan_map.end ())
            {
              std::string err ("unknown artifact ");
              err += name;
              throw Deployment::PlanError (
                plan_uuid.c_str (),
                err.c_str ());
            }
          ArtifactRegistry* ar = it_art->second;

          // lock
          if (ar->is_locked ())
            {
              // registry is locked by another thread so wait
              if (this->artifacts_condition_.wait () != 0)
                {
                  throw Deployment::PlanError (
                    plan_uuid.c_str (),
                    "artifact registry lock failed");
                }
            }
          else
            {
              // we're free to set the lock
              ar->set_locked ();
              return ar;
            }
        }
      return 0;
    }

    void ArtifactInstallation_Impl::remove (
      const std::string& plan_uuid,
      const std::string& artifact_name)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove");

      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove -"
                          " uninstall request for artifact " << artifact_name <<
                          " from plan " << plan_uuid);

      ArtifactRegistry* ar =
          this->remove_artifact_registry (plan_uuid, artifact_name);

      if (!ar)
        return; // nothing to do yet

      this->remove_i (plan_uuid, artifact_name, ar);
    }

    void ArtifactInstallation_Impl::remove_all (const std::string& plan_uuid)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_all");

      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_all -" <<
                          " removing versions for plan " << plan_uuid);

      // this method removes all installed artifacts irrespective of
      // install count!

      TArtifactsMap artifacts_map;
      this->remove_artifacts_map (plan_uuid, artifacts_map);

      for (TArtifactsMap::iterator ita = artifacts_map.begin ();
           ita != artifacts_map.end ();
           ++ita)
        {
          this->remove_i (plan_uuid, ita->first.c_str (), ita->second);
        }
    }

    void ArtifactInstallation_Impl::remove_intermediates (
        const std::string& plan_uuid,
        const std::string& artifact_name,
        ArtifactRegistry& artifact_reg)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_intermediates");

      ACE_GUARD_REACTION (TLOCK,
                          handler_guard_,
                          handler_lock_,
                          throw Deployment::PlanError (
                            plan_uuid.c_str (),
                            "ArtifactInstallation handler lock failed"));

      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_intermediates -" <<
                          " removing intermediate versions for artifact " << artifact_name <<
                          " from plan " << plan_uuid);

      ArtifactRegistry::TVersions& versions = artifact_reg.versions ();
      while (versions.size () > 1)
        {
          // find protocol handler
          THandlerMap::iterator ith = handlers_.find (versions.front ().protocol_);
          if (ith == handlers_.end ())
            {
              DANCEX11_LOG_ERROR ("ArtifactInstallation_Impl::remove_intermediates -" <<
                                  " cannot find protocol " << versions.front ().protocol_ <<
                                  " to remove version @ location " << versions.front ().location_ <<
                                  " for artifact " << artifact_name <<
                                  " from plan " << plan_uuid);
            }
          else
            {
              DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_intermediates -" <<
                                  " removing version of protocol " << versions.front ().protocol_ <<
                                  "@ " << versions.front ().location_ <<
                                  " for artifact " << artifact_name <<
                                  " from plan " << plan_uuid);

              ith->second->remove (plan_uuid, versions.front ().location_);
            }
          versions.erase (versions.begin ());
        }
    }

    void ArtifactInstallation_Impl::remove_i (const std::string& plan_uuid,
                                              const std::string& artifact_name,
                                              ArtifactRegistry* artifact_reg,
                                              bool do_delete)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_i");

      ACE_GUARD_REACTION (TLOCK,
                          handler_guard_,
                          handler_lock_,
                          throw Deployment::PlanError (
                            plan_uuid,
                            "ArtifactInstallation handler lock failed"));

      std::unique_ptr<ArtifactRegistry> safe_artifact_reg (do_delete ? artifact_reg : 0);

      for (ArtifactRegistry::TVersions::iterator iti =
             artifact_reg->versions ().begin ();
           iti != artifact_reg->versions ().end ();
           ++iti)
        {
          // in case no protocol registered there is nothing to remove
          if (!iti->protocol_.empty ())
            {
              // find protocol handler
              THandlerMap::iterator ith = handlers_.find (iti->protocol_);
              if (ith == handlers_.end ())
                {
                  DANCEX11_LOG_ERROR ("ArtifactInstallation_Impl::remove_i - " <<
                                      "cannot find protocol " << iti->protocol_ <<
                                      " to remove version @ location " << iti->location_ <<
                                      " for artifact " << artifact_name <<
                                      " from plan " << plan_uuid);
                }
              else
                {
                  DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_i - " <<
                                      "removing version of protocol " << iti->protocol_ <<
                                      " @ " << iti->location_ <<
                                      " for artifact " << artifact_name <<
                                      " from plan " << plan_uuid);

                  ith->second->remove (plan_uuid, iti->location_);
                }
            }
        }
    }

    ArtifactRegistry* ArtifactInstallation_Impl::remove_artifact_registry (
      const std::string& plan_uuid,
      const std::string& name)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_artifact_registry");

      ArtifactRegistry* ar {};

      // lock the artifact registry for the given plan
      ArtifactRegistry::Guard ar_guard (this->lock_artifact_registry (plan_uuid, name));

      // check to see if artifact has already been previously installed
      if (ar_guard->install_count () > 0)
        {
          // decrement install count
          ar_guard->decrement_install_count ();
          // only if we reach 0, we really remove artifacts
          if (ar_guard->install_count () == 0)
            {
              ACE_GUARD_REACTION (TLOCK,
                                  artifacts_guard_,
                                  artifacts_lock_,
                                  throw Deployment::PlanError (
                                    plan_uuid.c_str (),
                                    "artifacts lock failed"));

              DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove -" <<
                                  " removing versions for artifact " << name <<
                                  " from plan " << plan_uuid);

              // erase the artifact registry
              this->artifacts_[plan_uuid].erase (name);

              ar = &ar_guard;
            }
        }
      return ar;
    }

    void ArtifactInstallation_Impl::remove_artifacts_map (
      const std::string& plan_uuid,
      TArtifactsMap& artifacts_map)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::remove_artifacts_map");

      ACE_GUARD_REACTION (TLOCK,
                          artifacts_guard_,
                          artifacts_lock_,
                          throw Deployment::PlanError (
                            plan_uuid.c_str (),
                            "artifacts lock failed"));

      TArtifactsRegistry::iterator it_reg = this->artifacts_.find (plan_uuid);
      if (it_reg != this->artifacts_.end ())
        {
          artifacts_map = it_reg->second;
          this->artifacts_.erase (it_reg);
        }
    }

    std::string ArtifactInstallation_Impl::get_artifact_location (
      const std::string& plan_uuid,
      const std::string& artifact_name)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::get_artifact_location");

      ACE_GUARD_REACTION (TLOCK,
                          artifacts_guard_,
                          artifacts_lock_,
                          throw Deployment::PlanError (
                            plan_uuid,
                            "artifacts lock failed"));

      TArtifactsRegistry::iterator it_reg = this->artifacts_.find (plan_uuid);
      if (it_reg != this->artifacts_.end ())
        {
          TArtifactsMap::iterator it_art = it_reg->second.find (artifact_name);
          if (it_art != it_reg->second.end ())
            {
              return it_art->second->location ();
            }
        }
      return 0;
    }

    int ArtifactInstallation_Impl::register_handler (ArtifactInstallationHandler* aih)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::register_handler");

      if (aih != 0)
        {
          ACE_GUARD_RETURN (TLOCK, guard_, handler_lock_, -1);

          DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::register_handler -" <<
                               " " << aih->protocol_prefix () << ":// handler");
          ArtifactInstallation_Impl::handlers_[aih->protocol_prefix ()] = aih;
        }

      return 0;
    }

    int ArtifactInstallation_Impl::deregister_handler (ArtifactInstallationHandler* aih)
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::deregister_handler");

      if (aih != 0)
        {
          ACE_GUARD_RETURN (TLOCK, guard_, handler_lock_, -1);

          THandlerMap::iterator it_aih =
            ArtifactInstallation_Impl::handlers_.find (aih->protocol_prefix ());
          if (it_aih->second == aih)
            {
              DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::deregister_handler -" <<
                                  " " << aih->protocol_prefix () << ":// handler");
              ArtifactInstallation_Impl::handlers_.erase (it_aih);
            }
        }

      return 0;
    }


    bool ArtifactInstallation_Impl::handlers_available ()
    {
      DANCEX11_LOG_TRACE ("ArtifactInstallation_Impl::handlers_available");

      ACE_GUARD_RETURN (TLOCK, guard_, handler_lock_, false);

      bool available = !ArtifactInstallation_Impl::handlers_.empty ();

      DANCEX11_LOG_DEBUG ("ArtifactInstallation_Impl::handlers_available : " << (available ? "true" : "false"));

      return available;
    }

  }
