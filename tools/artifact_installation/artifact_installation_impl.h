// -*- C++ -*-
/**
 * @file artifact_installation_impl.h
 * @author Martin Corino <mcorino@remedy.nl>
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#ifndef DANCEX11_ARTIFACT_INSTALLATION_IMPL_H
#define DANCEX11_ARTIFACT_INSTALLATION_IMPL_H

#include /**/ "ace/config-all.h"

#include "ace/Service_Object.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "dancex11/core/dancex11_artifactinstallationC.h"
#include "dance_artifact_installation_export.h"
#include "dance_artifact_installation_handler.h"
#include <map>
#include <vector>
#include <stack>

namespace DAnCEX11
{
  class ArtifactRegistry
  {
    public:
      typedef ACE_MT_SYNCH::CONDITION TCONDITION;
      struct Version
      {
        std::string protocol_;
        std::string location_;
        Version ();
        Version (const std::string& protocol,
                 const std::string& location);
        Version (const Version& version);
        Version& operator =(const Version& version);
      };
      typedef std::vector<Version> TVersions;

      ArtifactRegistry (TCONDITION& condition, bool locked=false);
      ~ArtifactRegistry ();

      const std::string& location () const;

      TVersions& versions ();
      const TVersions& versions () const;

      uint32_t install_count () const;
      void increment_install_count ();
      void decrement_install_count ();

      void set_locked ();
      void set_unlocked ();
      bool is_locked () const;

      class Guard
        {
          public:
            Guard (ArtifactRegistry* ar);
            ~Guard ();

            ArtifactRegistry* operator ->();

            ArtifactRegistry& operator *();

            ArtifactRegistry* operator &();

          private:
            ArtifactRegistry* arp_;
        };

    private:
      TCONDITION& condition_;
      bool locked_;
      uint32_t install_count_;
      TVersions  versions_;
  };

  class DAnCE_Artifact_Installation_Export ArtifactInstallation_Impl
    : public IDL::traits< ::DAnCEX11::ArtifactInstallation>::base_type
  {
    public:
      typedef ACE_MT_SYNCH::MUTEX TLOCK;
      typedef ACE_MT_SYNCH::CONDITION TCONDITION;
      typedef ArtifactInstallationHandler::TPropertyMap TPropertyMap;
      virtual ~ArtifactInstallation_Impl ();

      virtual void initialize ();

      virtual void clear ();

      virtual void prepare (
          const ::Deployment::DeploymentPlan& plan);

      virtual void install (const std::string& plan_uuid,
            const ::Deployment::ArtifactDeploymentDescription & artifact);

      virtual void remove (const std::string& plan_uuid,
                           const std::string& artifact_name);

      virtual void remove_all (const std::string& plan_uuid);

      virtual std::string get_artifact_location (const std::string& plan_uuid,
                                                 const std::string& artifact_name);

    private:
      ArtifactInstallation_Impl ();

      template <typename _Tp1, typename, typename ...Args>
      friend CORBA::object_reference<_Tp1> CORBA::make_reference(Args&& ...args);

      typedef std::stack<std::string> TProtocolStack;

      // key is artifact name
      typedef std::map<std::string,
                       ArtifactRegistry*> TArtifactsMap;
      // key is plan uuid
      typedef std::map<std::string,
                       TArtifactsMap> TArtifactsRegistry;

      ArtifactRegistry* allocate_artifact_registry (const std::string& plan_uuid,
                                                    const std::string& name);

      ArtifactRegistry* lock_artifact_registry (const std::string& plan_uuid,
                                                const std::string& name);

      void parse_uri (const std::string& plan_uuid,
                      std::string& location,
                      TProtocolStack& protstack);

      bool install_i (const std::string& plan_uuid,
                      const std::string& protocol,
                      std::string& location,
                      const TPropertyMap& properties);

      void remove_i (const std::string& plan_uuid,
                     const std::string& artifact_name,
                     ArtifactRegistry* artifact_reg,
                     bool do_delete = true);

      void remove_intermediates (const std::string& plan_uuid,
                                 const std::string& artifact_name,
                                 ArtifactRegistry& artifact_reg);

      ArtifactRegistry* remove_artifact_registry (const std::string& plan_uuid,
                                                  const std::string& name);

      void remove_artifacts_map (const std::string& plan_uuid,
                                 TArtifactsMap& artifacts_map);

      TArtifactsRegistry artifacts_;
      TLOCK artifacts_lock_;
      TCONDITION artifacts_condition_;

    public:

      static IDL::traits< ::DAnCEX11::ArtifactInstallation>::ref_type instance ();

      static int register_handler (ArtifactInstallationHandler* aih);

      static int deregister_handler (ArtifactInstallationHandler* aih);

      static bool handlers_available ();

      typedef std::map<std::string,
                       ArtifactInstallationHandler*> THandlerMap;

    private:
      static IDL::traits< ::DAnCEX11::ArtifactInstallation>::ref_type instance_;
      static THandlerMap handlers_;
      static TLOCK handler_lock_;
  };

  inline
  ArtifactRegistry::Version::Version () {}

  inline
  ArtifactRegistry::Version::Version (
      const std::string& protocol,
      const std::string& location)
    : protocol_ (protocol),
      location_ (location)
    {
    }

  inline
  ArtifactRegistry::Version::Version (const ArtifactRegistry::Version& version)
  {
    *this = version;
  }

  inline
  ArtifactRegistry::Version& ArtifactRegistry::Version::operator =(const ArtifactRegistry::Version& version)
  {
    this->protocol_ = version.protocol_;
    this->location_ = version.location_;
    return *this;
  }

  inline
  const std::string& ArtifactRegistry::location () const
  {
    return this->versions_.back ().location_;
  }

  inline
  ArtifactRegistry::TVersions& ArtifactRegistry::versions ()
  {
    return this->versions_;
  }

  inline
  const ArtifactRegistry::TVersions& ArtifactRegistry::versions () const
  {
    return this->versions_;
  }

  inline
  uint32_t ArtifactRegistry::install_count () const
  {
    return this->install_count_;
  }

  inline
  void ArtifactRegistry::increment_install_count ()
  {
    ++this->install_count_;
  }

  inline
  void ArtifactRegistry::decrement_install_count ()
  {
    --this->install_count_;
  }

  inline
  void ArtifactRegistry::set_locked ()
  {
    this->locked_ = true;
  }

  inline
  void ArtifactRegistry::set_unlocked ()
  {
    if (this->locked_)
      {
        this->locked_ = false;
        this->condition_.broadcast ();
      }
  }

  inline
  bool ArtifactRegistry::is_locked () const
  {
    return this->locked_;
  }

  inline
  ArtifactRegistry::Guard::Guard (ArtifactRegistry* ar)
    : arp_ (ar)
  {
  }

  inline
  ArtifactRegistry::Guard::~Guard ()
  {
    if (this->arp_)
      {
        this->arp_->set_unlocked ();
      }
    this->arp_ = 0;
  }

  inline
  ArtifactRegistry* ArtifactRegistry::Guard::operator ->()
  {
    return this->arp_;
  }

  inline
  ArtifactRegistry* ArtifactRegistry::Guard::operator &()
  {
    return this->arp_;
  }

  inline
  ArtifactRegistry& ArtifactRegistry::Guard::operator *()
  {
    return *this->arp_;
  }
}

#endif /* DANCEX11_ARTIFACT_INSTALLATION_IMPL_H */
