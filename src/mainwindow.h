/* app.h
 *
 * Copyright (C) 2002-2015 The PrefixSuffix Development Team
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

#include <gtkmm.h>
#include <bakery/Configuration/Client.h>
#include <queue>
#include <set> //TODO: Use unordered_set with C++11.

class MainWindow : public Gtk::Window
{
public:
  MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  virtual ~MainWindow();

private:

  void build_list_of_files();
  void build_list_of_files(const Glib::ustring& directorypath_uri); //Called recursively.
  Glib::ustring get_new_basename(const Glib::ustring& filepath);
  static bool file_is_hidden(const Glib::ustring& filepath);
  static void canonical_folder_path(Glib::ustring& folderpath);
  static void get_folder_and_file(const Glib::ustring& filepath, Glib::ustring& folderpath, Glib::ustring& filename);

  // Signal Handlers:
  void on_radio_prefix_clicked();
  void on_radio_suffix_clicked();

  void on_button_process();
  void on_button_close();
  void on_button_stop();

  void on_directory_enumerate_children(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gio::File>& directory);
  void request_next_files(const Glib::RefPtr<Gio::File>& directory, const Glib::RefPtr<Gio::FileEnumerator>& enumerator);
  void on_directory_next_files(const Glib::RefPtr<Gio::AsyncResult>& result,
    const Glib::RefPtr<Gio::File>& directory, const Glib::RefPtr<Gio::FileEnumerator>& enumerator);

  void rename_next_file();
  void on_set_display_name(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gio::File>& file);

  void do_rename();
  void do_rename_files();
  void set_ui_locked(bool locked = true);
  void show_error(const Glib::ustring& message);
  void stop_process(const Glib::ustring& message = Glib::ustring());
  void clear_lists();

  Glib::RefPtr<Gdk::Cursor> m_old_cursor;

protected:
  virtual void on_hide(); //override.

  // Widgets

  //From Glade file:
  Gtk::Widget* m_grid_inputs;
  Gtk::RadioButton* m_radio_prefix;
  Gtk::RadioButton* m_radio_suffix;
  Gtk::Entry* m_entry_prefix_replace;
  Gtk::Entry* m_entry_prefix_with;
  Gtk::Entry* m_entry_suffix_replace;
  Gtk::Entry* m_entry_suffix_with;
  Gtk::FileChooserButton* m_entry_path;
  Gtk::Widget* m_container_prefix;
  Gtk::Widget* m_container_suffix;
  Gtk::Button* m_button_process;
  Gtk::Button* m_button_close;
  Gtk::Button* m_button_stop;
  Gtk::CheckButton* m_check_hidden;
  Gtk::CheckButton* m_check_folders;
  Gtk::CheckButton* m_check_recurse;
  Gtk::ProgressBar* m_progress_bar;

  Glib::RefPtr<Gio::Cancellable> m_cancellable;

  //GSettings:
  Bakery::Conf::Client m_conf_client;

  //List of files to rename:
  typedef std::queue<Glib::ustring> type_queue_strings;
  type_queue_strings m_files, m_files_new, m_folders, m_folders_new;

  typedef std::set< Glib::RefPtr<Gio::File> > type_set_files;
  type_set_files m_directory_enumerations_in_progress;

  type_queue_strings::size_type m_progress_max, m_progress_count;
};

