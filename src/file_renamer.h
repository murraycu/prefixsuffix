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

#ifndef PREFIXSUFFIX_FILE_RENAMER_H
#define PREFIXSUFFIX_FILE_RENAMER_H

#include "string_renamer.h"
#include <giomm/file.h>
#include <queue>
#include <set> //TODO: Use unordered_set with C++11.

namespace PrefixSuffix
{

class FileRenamer
{
public:
  FileRenamer(const Glib::ustring& directory_path,
    const Glib::ustring& prefix_replace, const Glib::ustring prefix_with,
    const Glib::ustring& suffix_replace, const Glib::ustring suffix_with,
    bool recurse_into_folders = true, bool operate_on_folders = false,
    bool operate_on_hidden = false);

  ~FileRenamer();

  void start();
  void stop();

  typedef sigc::signal<void, Glib::ustring> type_signal_stopped;

  /** Emitted during renaming.
   */
  type_signal_stopped signal_stopped();


  typedef sigc::signal<void, double> type_signal_progress;

  /** Emitted after successful renaming (with no error_message string),
   * or when renaming stops due to an error (with an error message string).
   */
  type_signal_progress signal_progress();

private:

  void build_list_of_files();
  void build_list_of_files(const Glib::ustring& directorypath_uri); //Called recursively.

  static bool file_is_hidden(const Glib::ustring& filepath);
  static void canonical_folder_path(Glib::ustring& folderpath);
  static void get_folder_and_file(const Glib::ustring& filepath, Glib::ustring& folderpath, Glib::ustring& filename);

  void on_directory_enumerate_children(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gio::File>& directory);
  void request_next_files(const Glib::RefPtr<Gio::File>& directory, const Glib::RefPtr<Gio::FileEnumerator>& enumerator);
  void on_directory_next_files(const Glib::RefPtr<Gio::AsyncResult>& result,
    const Glib::RefPtr<Gio::File>& directory, const Glib::RefPtr<Gio::FileEnumerator>& enumerator);

  void rename_next_file();
  void on_set_display_name(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gio::File>& file);


  void do_rename_files();
  void stop_process(const Glib::ustring& message = Glib::ustring());
  void clear_lists();

  Glib::RefPtr<Gio::Cancellable> m_cancellable;

  //List of files to rename:
  typedef std::queue<Glib::ustring> type_queue_strings;
  type_queue_strings m_files, m_files_new, m_folders, m_folders_new;

  typedef std::set< Glib::RefPtr<Gio::File> > type_set_files;
  type_set_files m_directory_enumerations_in_progress;

  const Glib::ustring m_directory_path;
  const StringRenamer m_string_renamer;
  const bool m_recurse_into_folders, m_operate_on_folders, m_operate_on_hidden;


  //The original number of files in m_files;
  double m_count;

  type_queue_strings::size_type m_progress_max, m_progress_count;

  type_signal_stopped m_signal_stopped;
  type_signal_progress m_signal_progress;
};

} //namespace PrefixSuffix

#endif /* PREFIXSUFFIX_FILE_RENAMER_H */
