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
//#include <libgnome/gnome-i18n.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
//#include <libgnomeuimm/about.h>
#include <gtkmm/messagedialog.h>
#include <bakery/Utilities/BusyCursor.h>
#include <iostream>
#include <glibmm/i18n.h>


MainWindow::MainWindow()
  : m_ConfClient("/apps/gnome/prefixsuffix"),
    m_progress_max(0),
    m_progress_count(0)
{
  //set_border_width(12);

  //Glade:
  //Look for the installed .glade file:
  try
  {
    m_refGlade = Gtk::Builder::create_from_file(PREFIXSUFFIX_GLADEDIR "prefixsuffix.glade");
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << G_STRFUNC << ": BuilderError Exception: " << ex.what() << std::endl;
  }
  catch(const Glib::MarkupError& ex)
  {
    std::cerr << G_STRFUNC << ": MarkupError exception:" << ex.what() << std::endl;
  }
  catch(const Glib::FileError& ex)
  {
    std::cerr << G_STRFUNC << ": FileError: exception" << ex.what() << std::endl;
  }
  catch(const Glib::Error& ex)
  {
    std::cerr << G_STRFUNC << ": Exception of unexpected type: " << ex.what() << std::endl;
  }

  //This doesn't actually work, because an invalid GladeXML object is created. I filed a bug:
  if(!m_refGlade)
  {
    //Try the local directory. Maybe somebody is just running this without installing it:
    g_warning("Prefix: Failed to find installed glade file. Looking for it in the current directory.\n");
    m_refGlade = Gtk::Builder::create_from_file("prefixsuffix.glade");
  }

  //If it still can't be found:
  if(!m_refGlade)
    g_warning("PrefixSuffix: Could not find prefixsuffix.glade.\n");

  //Get the top-level window in the glade file:
  Gtk::Window* pWindow = 0;
  m_refGlade->get_widget("window", pWindow);
  if(pWindow)
  {
    //Get the top-level container widget:
    m_refGlade->get_widget("toplevel", m_pWidgetTopLevel);
    if(m_pWidgetTopLevel)
    {
      //Remove it from the Window and add it to this MainWindow:
      m_pWidgetTopLevel->reference(); //ref it while it is unparented.
      pWindow->remove();

      add(*m_pWidgetTopLevel);
      m_pWidgetTopLevel->unreference();
    }

    //Radiobuttons:
    m_refGlade->get_widget("radio_prefix", m_pRadioPrefix);
    m_pRadioPrefix->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_radio_prefix_clicked) );
    m_refGlade->get_widget("radio_suffix",  m_pRadioSuffix);
    m_pRadioSuffix->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_radio_suffix_clicked) );

    //Entrys
    m_refGlade->get_widget("entry_prefix_replace", m_pEntryPrefixReplace);
    m_refGlade->get_widget("entry_prefix_with", m_pEntryPrefixWith);
    m_refGlade->get_widget("entry_suffix_replace", m_pEntrySuffixReplace);
    m_refGlade->get_widget("entry_suffix_with", m_pEntrySuffixWith);
    m_refGlade->get_widget("entry_path", m_pEntryPath);

    //Containers:
    m_refGlade->get_widget("table_prefix", m_pContainerPrefix);
    m_refGlade->get_widget("table_suffix", m_pContainerSuffix);

    //Buttons:
    m_refGlade->get_widget("button_process", m_pButtonProcess);
    m_pButtonProcess->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_button_process) );
    m_refGlade->get_widget("button_close", m_pButtonClose);
    m_pButtonClose->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_button_close) );

    //Check Buttons:
    m_refGlade->get_widget("check_operate_on_hidden", m_pCheckHidden);
    m_refGlade->get_widget("check_operate_on_folders", m_pCheckFolders);
    m_refGlade->get_widget("check_recurse", m_pCheckRecurse);

    //Progress bar:
    m_refGlade->get_widget("progressbar", m_pProgressBar);
  }


  //GConf: associate configuration keys with widgets. We use load() and save() later:
  m_ConfClient.add("prefix_replace",  *m_pEntryPrefixReplace);
  m_ConfClient.add("prefix_with",  *m_pEntryPrefixWith);
  m_ConfClient.add("suffix_replace",  *m_pEntrySuffixReplace);
  m_ConfClient.add("suffix_with",  *m_pEntrySuffixWith);
  m_ConfClient.add("path",  *m_pEntryPath);
  m_ConfClient.add("prefix",  *m_pRadioPrefix);
  m_ConfClient.add("suffix",  *m_pRadioSuffix);
  m_ConfClient.add("check_operate_on_hidden",  *m_pCheckHidden);
  m_ConfClient.add("check_operate_on_folders",  *m_pCheckFolders);
  m_ConfClient.add("check_recurse",  *m_pCheckRecurse);

  m_ConfClient.load(); //Fill the widgets from the stored preferences.


  //set_statusbar(m_Status);

  show_all_children();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_radio_prefix_clicked()
{
  bool enable = m_pRadioPrefix->get_active();
  m_pContainerPrefix->set_sensitive(enable);
}

void MainWindow::on_radio_suffix_clicked()
{
  bool enable = m_pRadioSuffix->get_active();
  m_pContainerSuffix->set_sensitive(enable);
}

void MainWindow::on_button_process()
{
  //Check that there is enough information:
  const Glib::ustring uri = m_pEntryPath->get_uri();
  if(uri.empty())
  {
    Gtk::MessageDialog dialog(*this, "Please choose a directory.");
    dialog.run();
    return;
  }
  /*
  else if ( !(Glib::file_test( m_pEntryPath->get_text(), Glib::FILE_TEST_IS_DIR)) )
  {

  }
  */
  if( m_pRadioPrefix->get_active() && (m_pEntryPrefixReplace->get_text().size() == 0) &&  (m_pEntryPrefixWith->get_text().size() == 0) )
  {
    Gtk::MessageDialog dialog(*this, "Please enter values in the prefix fields.");
    dialog.run();
    return;
  }

  if( m_pRadioPrefix->get_active() && (m_pEntryPrefixReplace->get_text() == m_pEntryPrefixWith->get_text()) )
  {
    Gtk::MessageDialog dialog(*this, "The Replace and With values are identical.");
    dialog.run();
    return;
  }

  if( m_pRadioSuffix->get_active() && (m_pEntrySuffixReplace->get_text().size() == 0) &&  (m_pEntrySuffixWith->get_text().size() == 0) )
  {
    Gtk::MessageDialog dialog(*this, "Please enter values in the suffix fields.");
    dialog.run();
    return;
  }

  if( m_pRadioSuffix->get_active() && (m_pEntrySuffixReplace->get_text() == m_pEntrySuffixWith->get_text()) )
  {
    Gtk::MessageDialog dialog(*this, "The Replace and With values are identical.");
    dialog.run();
    return;
  }

  //We have enough to start processing:
  //Bakery::BusyCursor busyCursor(*this);

  //m_Status.push(_("Generating list of files."));
  //m_Status.refresh();
  if(!build_list_of_files())
    return;

  if(m_listFiles.empty())
  {
    Gtk::MessageDialog dialog(*this, "No files have this prefix or suffix, so no files will be renamed.");
    dialog.run();
    return;
  }

  //Rename the files:
  type_listStrings::const_iterator iterNew = m_listFilesNew.begin();
  for(type_listStrings::const_iterator iter = m_listFiles.begin(); iter != m_listFiles.end(); ++iter)
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
      Gtk::MessageDialog dialog(*this, "PrefixSuffix failed while renaming the files.");
      dialog.run();
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
  m_listFiles.clear();
  m_listFilesNew.clear();
  m_listFolders.clear();
  m_listFoldersNew.clear();
  const Glib::ustring uri = m_pEntryPath->get_uri();

  //recurse:
  return build_list_of_files(uri);  
}
  
bool MainWindow::build_list_of_files(const Glib::ustring& directorypath_uri_in)
{
  //This is a recursion.

  Glib::ustring directorypath_uri = directorypath_uri_in;
  canonical_folder_path(directorypath_uri);

  bool bUseHidden = m_pCheckHidden->get_active();
  const bool operate_on_folders = m_pCheckFolders->get_active();
  const bool recurse_into_folders = m_pCheckRecurse->get_active();

  //Get the filenames in the directory:
  type_listStrings listFolders;
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
          listFolders.push_back(uri);
        } else {
          const Glib::ustring& basename_new = get_new_basename(basename);
          if(basename_new != basename) //Ignore it if the prefix/suffix change had no effect
          {
            m_listFiles.push_back(uri);
            m_listFilesNew.push_back(basename_new);
          }
        }
      }

      info = enumerator->next_file();
    }
  }
  catch(const Glib::Error& ex)
  {
    Gtk::MessageDialog dialog(*this, "PrefixSuffix failed while obtaining the list of files.");
    dialog.run();

    std::cerr << G_STRFUNC << ": Exception with directorypath_uri=" << directorypath_uri << ": " << ex.what() << std::endl;

    return false; //Stop trying.
  }

  // Examine the sub-directories:
  for(type_listStrings::const_iterator iter = listFolders.begin(); iter != listFolders.end(); ++iter)
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
        m_listFolders.push_back(child_dir);
        m_listFoldersNew.push_back(basename);
      }
    }
  }

  return true; //Success.
}

Glib::ustring MainWindow::get_new_basename(const Glib::ustring& basename)
{
  Glib::ustring filename_new;

  //Prefix:
  if(m_pRadioPrefix->get_active())
  {
    Glib::ustring strPrefixReplace = m_pEntryPrefixReplace->get_text();
    Glib::ustring strPrefixWith = m_pEntryPrefixWith->get_text();

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
  if(m_pRadioSuffix->get_active())
  {
    Glib::ustring strSuffixReplace = m_pEntrySuffixReplace->get_text();
    Glib::ustring strSuffixWith = m_pEntrySuffixWith->get_text();

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
  m_ConfClient.save();
}






