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

#include "string_renamer.h"
#include <iostream>
#include <glibmm/i18n.h>

namespace PrefixSuffix
{

StringRenamer::StringRenamer(
  const Glib::ustring& prefix_replace, const Glib::ustring prefix_with,
  const Glib::ustring& suffix_replace, const Glib::ustring suffix_with)
  : m_prefix_replace(prefix_replace),
    m_prefix_with(prefix_with),
    m_suffix_replace(suffix_replace),
    m_suffix_with(suffix_with),
    m_prefix(!prefix_replace.empty() || !prefix_with.empty()),
    m_suffix(!suffix_replace.empty() || !suffix_with.empty())
{
}

StringRenamer::~StringRenamer()
{
}

Glib::ustring StringRenamer::get_new_basename(const Glib::ustring& basename) const
{
  Glib::ustring filename_new;

  //Prefix:
  if(m_prefix)
  {
    if(!m_prefix_replace.empty()) //If an old prefix was specified
    {
      //If the old prefix is there:
      Glib::ustring::size_type posPrefix = basename.find(m_prefix_replace);
      if(posPrefix == 0)
      {
        //Remove old prefix:
        filename_new = basename.substr(m_prefix_replace.size());

        //Add new prefix:
        filename_new = m_prefix_with + filename_new;
      }
      else
      {
        //No change:
        filename_new = basename;
      }
    }
   else
    {
      //There's no old prefix to find, so just add the new prefix:
      filename_new = m_prefix_with + basename;
    }
  }


  //Suffix:
  if(m_suffix)
  {
    //If the old suffix is there:
    if(!m_suffix_replace.empty()) //if an old suffix was specified
    {
      const Glib::ustring::size_type posSuffix = basename.rfind(m_suffix_replace);
      if(posSuffix != Glib::ustring::npos && ((basename.size() - posSuffix) == m_suffix_replace.size())) //if it was found, and if these were the last characters in the string.
      {
        //Remove old suffix:
        filename_new = basename.substr(0, posSuffix);

        //Add new suffix:
        filename_new += m_suffix_with;
      }
     else
      {
        //No change:
        filename_new = basename;
      }
    }
    else
    {
      //There's no old suffix to find, so just add the new suffix:
      filename_new += m_suffix_with;
    }
  }

  return filename_new;
}

} //namespace PrefixSuffix



