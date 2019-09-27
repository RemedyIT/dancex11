/**
 * @file    dancex11_dynamic_loader.cpp
 * @author Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 * Chamber of commerce Rotterdam nr.276339, The Netherlands
 */

#include "dancex11_dynamic_loader.h"
#include <stdexcept>

namespace DAnCEX11
{
  void
  DL_MethodBase::is_valid ()
  {
    if (!this->entry_point ())
    {
      throw std::runtime_error ("DAnCEX11::DL_Method::call on nil entrypoint");
    }
  }

  bool
  DL_Module::open_i (
      std::string& status)
  {
    if (this->dl_artifact_.empty ())
    {
      status += "DL_Module: Cannot dynamically load module for empty artifact name.";
    }
    else
    {
      if (this->dl_module_)
      {
        if (this->dl_module_->get_handle () != ACE_SHLIB_INVALID_HANDLE)
          return true;
      }

      this->dl_module_ = std::make_shared<ACE_DLL> ();

      DANCEX11_LOG_DEBUG ("DL_Module::open_i - " <<
                          "loading artifact <" << this->dl_artifact_ << ">");

      if (this->dl_module_->open(this->dl_artifact_.c_str (), this->dl_openmode_, false) == 0)
      {
        DANCEX11_LOG_DEBUG ("DL_Module::open_i - " <<
                            "artifact successfully loaded");

        return true;
      }
      else
      {

        status += "DL_Module: Dynamic load of artifact <";
        status += this->dl_artifact_;
        status += "> failed : ";
        status += this->dl_module_->error ();
      }
    }
    return false;
  }

  ptrdiff_t
  DL_Module::resolve_i (
      const std::string& entrypoint,
      std::string& status)
  {

    DANCEX11_LOG_DEBUG ("DL_Module::resolve_i - " <<
                        "resolving entrypoint <" << entrypoint <<
                        "> in artifact <" << this->dl_artifact_ << ">");

    void *void_ptr = this->dl_module_->symbol (entrypoint.c_str ());
    ptrdiff_t tmp_ptr = reinterpret_cast <ptrdiff_t> (void_ptr);

    if (tmp_ptr == 0)
    {
      status += "DL_Module: Failed to resolve entrypoint <";
      status += entrypoint;
      status += "> in artifact <";
      status += this->dl_artifact_;
      status += "> : ";
      status += this->dl_module_->error ();
    }
    else
    {
      DANCEX11_LOG_DEBUG ("DL_Module::resolve_i - " <<
                          "successfully resolved entrypoint");
    }

    return tmp_ptr;
  }

}
