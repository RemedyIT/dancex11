/**
 * @file    dependency_sorter.cpp
 * @author Johnny Willemsen, Martin Corino
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */
#include "dependency_sorter.h"

#include <algorithm>
#include <vector>

namespace DAnCEX11
{
  /// Add a handler which has no dependencies
  void
  Dependency_Sorter::add_nondependent (const std::string &subject)
  {
    this->dep_map_[subject] = IH_DEPS ();
  }

  /// Add a dependency to the map.
  void
  Dependency_Sorter::add_dependency (const std::string &subject,
    const IH_DEPS &deps)
  {
    this->dep_map_[subject] = deps;
  }


  void
  Dependency_Sorter::add_dependency (const std::string &subject,
    const std::string &install_after)
  {
    DEP_MAP::iterator after = this->dep_map_.find (subject);

    if (after == this->dep_map_.end ())
      {
        IH_DEPS tmp;
        tmp.insert (install_after);
        this->dep_map_[subject] = tmp;
      }
    else
      {
        after->second.insert (install_after);
      }
  }

  void
  Dependency_Sorter::calculate_order (Dependency_Sorter::INSTALL_ORDER &retval)
  {
    // Deps which have been added to the install order
    IH_DEPS selected;

    DEP_MAP tmp_dep = this->dep_map_;

    while (retval.size () != this->dep_map_.size ())
      {
        bool updated (false);

        for (const std::pair<const std::string, IH_DEPS>& dependency : tmp_dep)
          {
            if (dependency.second.empty ()) // i.e., no dependencies
              {
                retval.push_back (dependency.first);
                selected.insert (dependency.first);
                updated = true;
                tmp_dep.erase (dependency.first);
                break;
              }

            if (selected.size () >= dependency.second.size ())
              {
                std::vector< std::string > tmp (dependency.second.size ());

                std::set_intersection (dependency.second.begin (),
                                       dependency.second.end (),
                                       selected.begin (),
                                       selected.end (),
                                       tmp.begin ());

                if (tmp.size () == dependency.second.size ()) // i.e., all deps satisfied
                  {
                    retval.push_back (dependency.first);
                    selected.insert (dependency.first);
                    updated = true;
                    tmp_dep.erase (dependency.first);
                    break;
                  }
              }
          }

        if (!updated) // i.e., we weren't able to add a -new- dep
          {
            throw Invalid_Install_Order ();
          }
      }
  }
}
