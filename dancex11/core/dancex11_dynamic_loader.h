/**
 * @file    dancex11_dynamic_loader.h
 * @author Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_DYNAMIC_LOADER_H
#define DAnCEX11_DYNAMIC_LOADER_H

#include "dancex11/core/dancex11_stub_export.h"
#include "ace/DLL.h"

#include "dancex11/logger/log.h"

#include <string>
#include <memory>

namespace DAnCEX11
{

  class DL_Module;

  class DANCEX11_STUB_Export DL_MethodBase
  {
  protected:
    DL_MethodBase () = default;
    DL_MethodBase (const DL_Module& dlmod,
                   ptrdiff_t entrypoint);
    DL_MethodBase (const DL_MethodBase&) = default;
    DL_MethodBase (DL_MethodBase&&) = default;
    virtual ~DL_MethodBase () = default;

    void is_valid ();

    ptrdiff_t
    entry_point () const
    { return this->dl_entrypt_; }

  private:
    std::shared_ptr<ACE_DLL> dl_module_ {};
    ptrdiff_t dl_entrypt_ {};
  };

  template <typename T, typename ...Args>
  class DL_Method : public DL_MethodBase
  {
  public:
    DL_Method () = default;
    DL_Method (const DL_Module& dlmod,
               ptrdiff_t entrypoint)
     : DL_MethodBase (dlmod, entrypoint)
    {}
    DL_Method (const DL_Method&) = default;
    DL_Method (DL_Method&&) = default;

    explicit operator bool() const // never throws
    { return this->entry_point () == 0 ? false : true; }
    bool operator ==(std::nullptr_t) const // never throws
    { return this->entry_point () == 0 ? true : false; }
    bool operator !=(std::nullptr_t) const // never throws
    { return this->entry_point () == 0 ? false : true; }

    T
    operator () (
        Args ...args)
    {
      this->is_valid ();

      typedef T (*method_type) (Args ...);

      method_type method_ptr = reinterpret_cast<method_type> (this->entry_point ());

      return method_ptr (std::forward<Args> (args)...);
    }
  };

  class DANCEX11_STUB_Export DL_Module final
  {
  public:
    DL_Module (std::string artifact,
               uint32_t openmode = ACE_DEFAULT_SHLIB_MODE)
      : dl_artifact_ (artifact),
        dl_openmode_ (openmode)
    {}
    ~DL_Module () = default;

    template <typename T, typename ...Args>
    DL_Method<T, Args...>
    resolve (
        std::string& status,
        const std::string& entrypoint)
    {
      if (this->open_i (status))
      {
        ptrdiff_t mptr = this->resolve_i (entrypoint, status);
        if (mptr)
        {
          return DL_Method<T, Args...> (*this, mptr);
        }
      }
      return {};
    }

  protected:
    bool
    open_i (
        std::string& status);

    ptrdiff_t
    resolve_i (
        const std::string& entrypoint,
        std::string& status);

  private:
    friend class DL_MethodBase;

    std::string dl_artifact_ {};
    uint32_t dl_openmode_ {};
    std::shared_ptr<ACE_DLL> dl_module_ {};
  };


  inline DL_MethodBase::DL_MethodBase (
      const DL_Module& dlmod,
      ptrdiff_t entrypoint)
    : dl_module_ (dlmod.dl_module_),
      dl_entrypt_ (entrypoint)
  {}

}

#endif /* DAnCEX11_DYNAMIC_LOADER_H */
