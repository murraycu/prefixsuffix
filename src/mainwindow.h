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
  MainWindow();
  virtual ~MainWindow();

protected:

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

  virtual void on_hide(); //override.

  Glib::RefPtr<Gtk::Builder> m_refGlade;

  // Widgets

  //From Glade file:
  Gtk::Widget* m_pWidgetTopLevel;
  Gtk::RadioButton* m_pRadioPrefix;
  Gtk::RadioButton* m_pRadioSuffix;
  Gtk::Entry* m_pEntryPrefixReplace;
  Gtk::Entry* m_pEntryPrefixWith;
  Gtk::Entry* m_pEntrySuffixReplace;
  Gtk::Entry* m_pEntrySuffixWith;
  Gtk::FileChooserButton* m_pEntryPath;
  Gtk::Widget* m_pContainerPrefix;
  Gtk::Widget* m_pContainerSuffix;
  Gtk::Button* m_pButtonProcess;
  Gtk::Button* m_pButtonClose;
  Gtk::CheckButton* m_pCheckHidden;
  Gtk::CheckButton* m_pCheckFolders;
  Gtk::CheckButton* m_pCheckRecurse;
  Gtk::ProgressBar* m_pProgressBar;

  //Not from Glade file:
  //Gnome::UI::AppBar m_Status;

  //GSettings:
  Bakery::Conf::Client m_ConfClient;

  //List of files to rename:
  typedef std::list<Glib::ustring> type_listStrings;
  type_listStrings m_listFiles, m_listFilesNew, m_listFolders, m_listFoldersNew;

  type_listStrings::size_type m_progress_max, m_progress_count;
};

