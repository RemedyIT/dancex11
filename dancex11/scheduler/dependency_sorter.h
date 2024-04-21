/**
 * @file    dependency_sorter.h
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#ifndef DAnCEX11_DEP_SORTER_H
#define DAnCEX11_DEP_SORTER_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "tao/x11/base/stddef.h"

namespace DAnCEX11
{
  class Dependency_Sorter final
  {
  public:
    using INSTALL_ORDER = std::vector <std::string>;
    using IH_DEPS = std::set <std::string>;

    class Invalid_Install_Order
      : std::exception {};

    /// Add a handler which has no dependencies
    void
    add_nondependent (const std::string &subject);

    /// Add a dependency to the map.
    void
    add_dependency (const std::string &subject,
      const IH_DEPS &deps);

    /// Add a dependency to the map.
    void
    add_dependency (const std::string &subject,
      const std::string &install_after);

    void
    calculate_order (INSTALL_ORDER &);

  private:
    using DEP_MAP = std::map<std::string, IH_DEPS>;

    DEP_MAP dep_map_;
  };
}

#endif /* DAnCEX11_DEP_SORTER_H */
