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
#include <list>

class MainWindow : public Gtk::Window
{
public:
  MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  virtual ~MainWindow();

private:

  virtual bool build_list_of_files();
  virtual bool build_list_of_files(const Glib::ustring& directorypath_uri); //Called recursively.
  virtual Glib::ustring get_new_basename(const Glib::ustring& filepath);
  static bool file_is_hidden(const Glib::ustring& filepath);
  static void canonical_folder_path(Glib::ustring& folderpath);
  static void get_folder_and_file(const Glib::ustring& filepath, Glib::ustring& folderpath, Glib::ustring& filename);

  // Signal Handlers:
  virtual void on_radio_prefix_clicked();
  virtual void on_radio_suffix_clicked();

  virtual void on_button_process();
  virtual void on_button_close();

  void on_directory_enumerate_children(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gio::File>& directory);

  void do_rename();
  void set_ui_locked(bool locked = true);
  void show_error(const Glib::ustring& message);

  Glib::RefPtr<Gdk::Cursor> m_old_cursor;

protected:
  virtual void on_hide(); //override.

  // Widgets

  //From Glade file:
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
  Gtk::CheckButton* m_check_hidden;
  Gtk::CheckButton* m_check_folders;
  Gtk::CheckButton* m_check_recurse;
  Gtk::ProgressBar* m_progress_bar;

  //Not from Glade file:
  //Gnome::UI::AppBar m_Status;

  //GSettings:
  Bakery::Conf::Client m_conf_client;

  //List of files to rename:
  typedef std::list<Glib::ustring> type_list_strings;
  type_list_strings m_list_files, m_list_files_new, m_list_folders, m_list_folders_new;

  type_list_strings::size_type m_progress_max, m_progress_count;
};

