/** Copyright (C) 2002-2015 The PrefixSuffix Development Team
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef PREFIXSUFFIX_STRING_RENAMER_H
#define PREFIXSUFFIX_STRING_RENAMER_H

#include <glibmm/ustring.h>

namespace PrefixSuffix
{

class StringRenamer
{
public:
  StringRenamer(const Glib::ustring& prefix_replace, const Glib::ustring prefix_with,
    const Glib::ustring& suffix_replace, const Glib::ustring suffix_with);

  ~StringRenamer();

  Glib::ustring get_new_basename(const Glib::ustring& filepath) const;

  void debug_cout() const;

  const Glib::ustring m_prefix_replace, m_prefix_with;
  const Glib::ustring m_suffix_replace, m_suffix_with;

  //These just avoid repeated checks for empty strings:
  const bool m_prefix, m_suffix;
};

} //namespace PrefixSuffix

#endif /* PREFIXSUFFIX_STRING_RENAMER_H */
