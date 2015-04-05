/* app.cc
 *
 * Copyright (C) 2002 The PrefixSuffix Development Team
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

#include "mainwindow.h"
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/messagedialog.h>
#include <iostream>
#include <glibmm/i18n.h>


MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
  : Gtk::Window(cobject),
    m_conf_client("org.gnome.prefixsuffix"),
    m_progress_max(0),
    m_progress_count(0)
{
  //set_border_width(12);

  //Radiobuttons:
  builder->get_widget("radio_prefix", m_radio_prefix);
  m_radio_prefix->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_radio_prefix_clicked) );
  builder->get_widget("radio_suffix",  m_radio_suffix);
  m_radio_suffix->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_radio_suffix_clicked) );

  //Entrys
  builder->get_widget("entry_prefix_replace", m_entry_prefix_replace);
  builder->get_widget("entry_prefix_with", m_entry_prefix_with);
  builder->get_widget("entry_suffix_replace", m_entry_suffix_replace);
  builder->get_widget("entry_suffix_with", m_entry_suffix_with);
  builder->get_widget("entry_path", m_entry_path);

  //Containers:
  builder->get_widget("table_prefix", m_container_prefix);
  builder->get_widget("table_suffix", m_container_suffix);

  //Buttons:
  builder->get_widget("button_process", m_button_process);
  m_button_process->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_button_process) );
  builder->get_widget("button_close", m_button_close);
  m_button_close->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_button_close) );

  //Check Buttons:
  builder->get_widget("check_operate_on_hidden", m_check_hidden);
  builder->get_widget("check_operate_on_folders", m_check_folders);
  builder->get_widget("check_recurse", m_check_recurse);

  //Progress bar:
  builder->get_widget("progressbar", m_progress_bar);


  //GConf: associate configuration keys with widgets. We use load() and save() later:
  m_conf_client.add("prefix-replace",  *m_entry_prefix_replace);
  m_conf_client.add("prefix-with",  *m_entry_prefix_with);
  m_conf_client.add("suffix-replace",  *m_entry_suffix_replace);
  m_conf_client.add("suffix-with",  *m_entry_suffix_with);
  m_conf_client.add("prefix",  *m_radio_prefix);
  m_conf_client.add("suffix",  *m_radio_suffix);
  m_conf_client.add("check-operate-on-hidden",  *m_check_hidden);
  m_conf_client.add("check-operate-on-folders",  *m_check_folders);
  m_conf_client.add("check-recurse",  *m_check_recurse);

  m_conf_client.load(); //Fill the widgets from the stored preferences.


  //set_statusbar(m_Status);

  show_all_children();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_radio_prefix_clicked()
{
  bool enable = m_radio_prefix->get_active();
  m_container_prefix->set_sensitive(enable);
}

void MainWindow::on_radio_suffix_clicked()
{
  bool enable = m_radio_suffix->get_active();
  m_container_suffix->set_sensitive(enable);
}

void MainWindow::on_button_process()
{
  set_ui_locked();
  do_rename();
  set_ui_locked(false);
}

void MainWindow::set_ui_locked(bool locked)
{
  set_sensitive(!locked);

  Glib::RefPtr<Gdk::Window> window = get_window();
  if(!window)
    return;

  Glib::RefPtr<Gdk::Cursor> cursor;
  if(locked)
  {
    m_old_cursor = window->get_cursor();
    cursor = Gdk::Cursor::create(get_display(), Gdk::WATCH);
  }
  else
  {
    cursor = m_old_cursor;
    m_old_cursor.reset();
  }

  window->set_cursor(cursor);

  //Force the GUI to update:
  //TODO: Make sure that gtkmm has some non-Gtk::Main API for this:
  while(gtk_events_pending())
    gtk_main_iteration_do(true);
}

void MainWindow::do_rename()
{
  //Check that there is enough information:
  const Glib::ustring uri = m_entry_path->get_uri();
  if(uri.empty())
  {
    show_error("Please choose a directory.");
    return;
  }
  /*
  else if ( !(Glib::file_test( m_entry_path->get_text(), Glib::FILE_TEST_IS_DIR)) )
  {

  }
  */
  if( m_radio_prefix->get_active() && (m_entry_prefix_replace->get_text().size() == 0) &&  (m_entry_prefix_with->get_text().size() == 0) )
  {
    show_error("Please enter values in the prefix fields.");
    return;
  }

  if( m_radio_prefix->get_active() && (m_entry_prefix_replace->get_text() == m_entry_prefix_with->get_text()) )
  {
    show_error("The Replace and With values are identical.");
    return;
  }

  if( m_radio_suffix->get_active() && (m_entry_suffix_replace->get_text().size() == 0) &&  (m_entry_suffix_with->get_text().size() == 0) )
  {
    show_error("Please enter values in the suffix fields.");
    return;
  }

  if( m_radio_suffix->get_active() && (m_entry_suffix_replace->get_text() == m_entry_suffix_with->get_text()) )
  {
    show_error("The Replace and With values are identical.");
    return;
  }

  //We have enough to start processing:
  //Bakery::BusyCursor busyCursor(*this);

  //m_Status.push(_("Generating list of files."));
  //m_Status.refresh();
  if(!build_list_of_files())
    return;

  if(m_list_files.empty())
  {
    show_error("No files have this prefix or suffix, so no files will be renamed.");
    return;
  }

  //Rename the files:
  type_list_strings::const_iterator iterNew = m_list_files_new.begin();
  for(type_list_strings::const_iterator iter = m_list_files.begin(); iter != m_list_files.end(); ++iter)
  {
    const Glib::ustring uri = *iter;
    const Glib::ustring uriNew = *iterNew;
    const Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(uri);

    std::cout << G_STRFUNC << ": debug: uri: " << uri << std::endl;
    std::cout << G_STRFUNC << ": debug: uriNew: " << uriNew << std::endl;
    try
    {
      file->set_display_name(uriNew);
    }
    catch(const Glib::Error& ex)
    {
      std::cerr << G_STRFUNC << ": Exception from Gio::File::set_display_name(): " << ex.what() << std::endl;
      show_error("PrefixSuffix failed while renaming the files.");
      return;
    }

    ++iterNew;
  }

  //Rename the folders:
  //TODO
}

void MainWindow::on_button_close()
{
  hide();
}

bool MainWindow::build_list_of_files()
{
  //This is the first run, rather than a recursion.
  m_list_files.clear();
  m_list_files_new.clear();
  m_list_folders.clear();
  m_list_folders_new.clear();
  const Glib::ustring uri = m_entry_path->get_uri();

  //recurse:
  return build_list_of_files(uri);  
}
  
bool MainWindow::build_list_of_files(const Glib::ustring& directorypath_uri_in)
{
  //This is a recursion.

  Glib::ustring directorypath_uri = directorypath_uri_in;
  canonical_folder_path(directorypath_uri);

  bool bUseHidden = m_check_hidden->get_active();
  const bool operate_on_folders = m_check_folders->get_active();
  const bool recurse_into_folders = m_check_recurse->get_active();

  //Get the filenames in the directory:
  type_list_strings list_folders;
  try
  {
    Glib::RefPtr<Gio::File> directory = Gio::File::create_for_uri(directorypath_uri);
    Glib::RefPtr<Gio::FileEnumerator> enumerator = directory->enumerate_children();
    Glib::RefPtr<Gio::FileInfo> info = enumerator->next_file();
    while(info)
    {
      const Glib::RefPtr<const Gio::File> child = directory->get_child(info->get_name());

      bool bUse = true;

      const std::string basename = child->get_basename();
      //Ignore any non-file filenames:
      if( (basename == "..") || (basename == ".") || basename.empty() )
      {
        bUse = false;
      }
      else
      {
        //Check whether we should use hidden files:
        if(!bUseHidden && file_is_hidden(basename))
          bUse = false;
      }

      if(bUse)
      {
        Glib::ustring uri = child->get_uri();

        const Gio::FileType file_type = child->query_file_type();
        if(file_type == Gio::FILE_TYPE_DIRECTORY)
        {
          //It's a folder:
          canonical_folder_path(uri);

          //Add to list of folders, so we can recurse into them,
          //and maybe rename them:
          list_folders.push_back(uri);
        } else {
          const Glib::ustring& basename_new = get_new_basename(basename);
          if(basename_new != basename) //Ignore it if the prefix/suffix change had no effect
          {
            m_list_files.push_back(uri);
            m_list_files_new.push_back(basename_new);
          }
        }
      }

      info = enumerator->next_file();
    }
  }
  catch(const Glib::Error& ex)
  {
    show_error("PrefixSuffix failed while obtaining the list of files.");

    std::cerr << G_STRFUNC << ": Exception with directorypath_uri=" << directorypath_uri << ": " << ex.what() << std::endl;

    return false; //Stop trying.
  }

  // Examine the sub-directories:
  for(type_list_strings::const_iterator iter = list_folders.begin(); iter != list_folders.end(); ++iter)
  {
    //Recurse to get files in this folder.
    const Glib::ustring child_dir = *iter;

    if(recurse_into_folders)
    {
      if(!build_list_of_files(child_dir))
        return false;
    }

    if(operate_on_folders)
    {
      const Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(child_dir);
      const std::string basename = file->get_basename();
      const Glib::ustring& filepath_new = get_new_basename(basename);
      if(child_dir != filepath_new) //Ignore it if the prefix/suffix change had no effect
      {
        m_list_folders.push_back(child_dir);
        m_list_folders_new.push_back(basename);
      }
    }
  }

  return true; //Success.
}

Glib::ustring MainWindow::get_new_basename(const Glib::ustring& basename)
{
  Glib::ustring filename_new;

  //Prefix:
  if(m_radio_prefix->get_active())
  {
    Glib::ustring strPrefixReplace = m_entry_prefix_replace->get_text();
    Glib::ustring strPrefixWith = m_entry_prefix_with->get_text();

    if(strPrefixReplace.size()) //If an old prefix was specified
    {
      //If the old prefix is there:
      Glib::ustring::size_type posPrefix = basename.find(strPrefixReplace);
      if(posPrefix != Glib::ustring::npos)
      {
        //Remove old prefix:
        filename_new = basename.substr(strPrefixReplace.size());

        //Add new prefix:
        filename_new = strPrefixWith + filename_new;
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
      filename_new = strPrefixWith + basename;
    }
  }


  //Suffix:
  if(m_radio_suffix->get_active())
  {
    Glib::ustring strSuffixReplace = m_entry_suffix_replace->get_text();
    Glib::ustring strSuffixWith = m_entry_suffix_with->get_text();

    //If the old suffix is there:
    if(strSuffixReplace.size()) //if an old suffix was specified
    {
      Glib::ustring::size_type posSuffix = basename.rfind(strSuffixReplace);
      if(posSuffix != Glib::ustring::npos && ((basename.size() - posSuffix) == strSuffixReplace.size())) //if it was found, and if these were the last characters in the string.
      {
        //Remove old suffix:
        filename_new = basename.substr(0, posSuffix);

        //Add new suffix:
        filename_new += strSuffixWith;
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
      filename_new += strSuffixWith;
    }
  }

  return filename_new;
}

bool MainWindow::file_is_hidden(const Glib::ustring& filename) //static
{
  //See whether it has "." at the start:
  Glib::ustring::size_type size = filename.size();
  if(size > 0)
    return (filename[0] == '.');
  else
    return false;
}

void MainWindow::canonical_folder_path(Glib::ustring& folderpath) //static
{
  //Make sure that it has "/" at the end:
  Glib::ustring::size_type size = folderpath.size();
  if(size > 0)
  {
    if(folderpath[size - 1] != '/') //TODO: Cross platform - mac uses ':'.
      folderpath += "/";
  }
}

void MainWindow::get_folder_and_file(const Glib::ustring& filepath, Glib::ustring& folderpath, Glib::ustring& filename)
{
  //Initialize output parameters:
  folderpath.erase();
  filename.erase();

  //TODO: Use Uri::extract_short_name()?

  //Find right-most "/" separator:
  Glib::ustring::size_type posSeparator = filepath.rfind("/");
  if(posSeparator != Glib::ustring::npos) //If it was found.
  {
    folderpath = filepath.substr(0, posSeparator + 1); //include the "/".

    if( (posSeparator + 1) < filepath.size() )
      filename = filepath.substr(posSeparator + 1);

    //g_warning("get_folder_and_file: %s, %s\n",  folderpath.c_str(), filename.c_str());
  }
}

void MainWindow::on_hide()
{
  //Store the widgets' contents in GConf, for use when the app is restarted.
  m_conf_client.save();
}

void MainWindow::show_error(const Glib::ustring& message)
{
  Gtk::MessageDialog dialog(*this, message);
  dialog.run();
}






