/** Copyright (C) 2015 The PrefixSuffix Development Team
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
#include <cstdlib>

bool check_prefix()
{
  const PrefixSuffix::StringRenamer renamer("oldprefix", "newprefix", "", "");
  Glib::ustring renamed = renamer.get_new_basename("oldprefixabc");
  if(renamed != "newprefixabc")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  renamed = renamer.get_new_basename("aoldprefixabc");
  if(renamed != "aoldprefixabc")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  return true;
}

bool check_suffix()
{
  const PrefixSuffix::StringRenamer renamer("", "", "oldsuffix", "newsuffix");
  Glib::ustring renamed = renamer.get_new_basename("abcoldsuffix");
  if(renamed != "abcnewsuffix")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  renamed = renamer.get_new_basename("abcoldsuffixa");
  if(renamed != "abcoldsuffixa")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  return true;
}

bool check_extension_suffix()
{
  //Check that it doesn't change when it shouldn't:
  const PrefixSuffix::StringRenamer renamer("", "", ".jpg", ".png");
  Glib::ustring renamed = renamer.get_new_basename("something");
  if(renamed != "something")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  //Check that it doesn't change when it shouldn't, again:
  renamed = renamer.get_new_basename("something.foo");
  if(renamed != "something.foo")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  //Check that it does change when it should:
  renamed = renamer.get_new_basename("something.jpg");
  if(renamed != "something.png")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  return true;
}

bool check_suffix_remove()
{
  const PrefixSuffix::StringRenamer renamer("", "", "suffixtoremove", "");
  Glib::ustring renamed = renamer.get_new_basename("abcsuffixtoremove");
  if(renamed != "abc")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  renamed = renamer.get_new_basename("abcsuffixtoremovea");
  if(renamed != "abcsuffixtoremovea")
  {
    std::cerr << G_STRFUNC << ": Unexpected result: " << renamed << std::endl;
    return false;
  }

  return true;
}

int main()
{
  if(!check_prefix())
    return EXIT_FAILURE;

  if(!check_suffix())
    return EXIT_FAILURE;

  if(!check_suffix_remove())
    return EXIT_FAILURE;

  if(!check_extension_suffix())
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
