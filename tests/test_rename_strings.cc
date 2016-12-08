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
#include <gtest/gtest.h>

TEST(TestRenameStrings, Prefix) {
  const PrefixSuffix::StringRenamer renamer("oldprefix", "newprefix", "", "");
  Glib::ustring renamed = renamer.get_new_basename("oldprefixabc");
  EXPECT_EQ("newprefixabc", renamed);

  renamed = renamer.get_new_basename("aoldprefixabc");
  EXPECT_EQ("aoldprefixabc", renamed);
}

TEST(TestRenameStrings, PrefixRemove) {
  const PrefixSuffix::StringRenamer renamer("oldprefix", "", "", "");
  Glib::ustring renamed = renamer.get_new_basename("oldprefixabc");
  EXPECT_EQ("abc", renamed);

  renamed = renamer.get_new_basename("aoldprefixabc");
  EXPECT_EQ("aoldprefixabc", renamed);
}

TEST(TestRenameStrings, Suffix) {
  const PrefixSuffix::StringRenamer renamer("", "", "oldsuffix", "newsuffix");
  Glib::ustring renamed = renamer.get_new_basename("abcoldsuffix");
  EXPECT_EQ("abcnewsuffix", renamed);

  renamed = renamer.get_new_basename("abcoldsuffixa");
  EXPECT_EQ("abcoldsuffixa", renamed);
}

TEST(TestRenameStrings, ExtensionSuffix) {
  // Check that it doesn't change when it shouldn't:
  const PrefixSuffix::StringRenamer renamer("", "", ".jpg", ".png");
  Glib::ustring renamed = renamer.get_new_basename("something");
  EXPECT_EQ("something", renamed);

  // Check that it doesn't change when it shouldn't, again:
  renamed = renamer.get_new_basename("something.foo");
  EXPECT_EQ("something.foo", renamed);

  // Check that it does change when it should:
  renamed = renamer.get_new_basename("something.jpg");
  EXPECT_EQ("something.png", renamed);
}

TEST(TestRenameStrings, SuffixRemove) {
  const PrefixSuffix::StringRenamer renamer("", "", "suffixtoremove", "");
  Glib::ustring renamed = renamer.get_new_basename("abcsuffixtoremove");
  EXPECT_EQ("abc", renamed);

  renamed = renamer.get_new_basename("abcsuffixtoremovea");
  EXPECT_EQ("abcsuffixtoremovea", renamed);
}
