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
  }
  /*
  else if ( !(Glib::file_test( m_pEntryPath->get_text(), Glib::FILE_TEST_IS_DIR)) )
  {

  }
  */
  else if( m_pRadioPrefix->get_active() && (m_pEntryPrefixReplace->get_text().size() == 0) &&  (m_pEntryPrefixWith->get_text().size() == 0) )
  {
    Gtk::MessageDialog dialog(*this, "Please enter values in the prefix fields.");
    dialog.run();
  }
  else if( m_pRadioPrefix->get_active() && (m_pEntryPrefixReplace->get_text() == m_pEntryPrefixWith->get_text()) )
  {
    Gtk::MessageDialog dialog(*this, "The Replace and With values are identical.");
    dialog.run();
  }
  else if( m_pRadioSuffix->get_active() && (m_pEntrySuffixReplace->get_text().size() == 0) &&  (m_pEntrySuffixWith->get_text().size() == 0) )
  {
    Gtk::MessageDialog dialog(*this, "Please enter values in the suffix fields.");
  }
  else if( m_pRadioSuffix->get_active() && (m_pEntrySuffixReplace->get_text() == m_pEntrySuffixWith->get_text()) )
  {
    Gtk::MessageDialog dialog(*this, "The Replace and With values are identical.");
    dialog.run();
  }
  else
  {
    //We have enough to start processing:
    Bakery::BusyCursor busyCursor(*this);

    //m_Status.push(_("Generating list of files."));
    //m_Status.refresh();
    bool test = build_list_of_files();
    if(test)
    {
      //m_Status.pop();

      //bool bOperateOnHidden = m_pCheckHidden->get_active();

      //m_Status.push(_("Renaming files."));
      //m_Status.refresh();

      //Build the list of new filenames:
      type_listStrings listFiles_old_names, listFiles_new_names;
      for(type_listStrings::iterator iter = m_listFiles.begin(); iter != m_listFiles.end(); ++iter)
      {
        const Glib::ustring& filepath = *iter;
        const Glib::ustring& filepath_new = get_new_filepath(filepath);
        if(filepath != filepath_new) //Ignore it if the prefix/suffix change had no effect
        {
          listFiles_old_names.push_back( filepath );
          listFiles_new_names.push_back( filepath_new );
          //std::cout << "adding: old=" << filepath << ", new=" << filepath_new << std::endl;
        }
      }

      //Rename the files
      if(listFiles_old_names.empty())
      {
        Gtk::MessageDialog dialog(*this, "No files have this prefix or suffix, so no files will be renamed.");
        dialog.run();
      }
      else
      {
        try
        {
          Gnome::Vfs::Transfer::transfer_list(listFiles_old_names, listFiles_new_names,
                                         Gnome::Vfs::XFER_REMOVESOURCE, //move instead of copying
                                         Gnome::Vfs::XFER_ERROR_MODE_QUERY,
                                         Gnome::Vfs::XFER_OVERWRITE_MODE_QUERY,
                                         sigc::mem_fun(*this, &MainWindow::on_transfer_progress) );

          Gtk::MessageDialog dialog(*this, "Renaming has finished successfully.");
          dialog.run();
        }
        catch(const Gnome::Vfs::exception& ex)
        {
          Glib::ustring message = "PrefixSuffix failed while renaming the files. Error:/n" + ex.what();
          Gtk::MessageDialog dialog(*this, message);
          dialog.run();
        }
      }

      //m_Status.pop();
      //m_Status.refresh();
    }
  }
}

void MainWindow::on_button_close()
{
  hide();
}

bool MainWindow::build_list_of_files()
{
  //This is the first run, rather than a recursion.
  m_listFiles.clear();
  m_listFolders.clear();
  Glib::ustring uri = m_pEntryPath->get_uri();

  //recurse:
  return build_list_of_files(uri);  
}
  
bool MainWindow::build_list_of_files(Glib::ustring& directorypath_uri)
{
  //This is a recursion.

  canonical_folder_path(directorypath_uri);

  bool bUseHidden = m_pCheckHidden->get_active();

  //Get the filenames in the directory:
  type_listStrings listFilenames;
  try
  {
    Gnome::Vfs::DirectoryHandle handle;
    handle.open(directorypath_uri, Gnome::Vfs::FILE_INFO_DEFAULT |
                                   Gnome::Vfs::FILE_INFO_GET_MIME_TYPE |
                                   Gnome::Vfs::FILE_INFO_FORCE_SLOW_MIME_TYPE);

    bool file_exists = true;
    while(file_exists) //read_next() returns false when there are no more files to read.
    {
      Glib::RefPtr<Gnome::Vfs::FileInfo> refFileInfo = handle.read_next(file_exists);
      Glib::ustring filename = refFileInfo->get_name();

      //Ignore any non-file filenames:
      if( (filename != "..") && (filename != ".") && !filename.empty() )
        listFilenames.push_back(refFileInfo->get_name());
    }
  }
  catch(const Gnome::Vfs::exception& ex)
  {
    Gtk::MessageDialog dialog(*this, "PrefixSuffix failed while obtaining the list of files.");
    dialog.run();
    return false; //Stop trying.
  }

  try
  {
    // Examine the filenames:
    for(type_listStrings::iterator iter = listFilenames.begin(); iter != listFilenames.end(); ++iter)
    {
      const Glib::ustring filename = *iter;

      //Check whether we should use hidden files:
      bool bUse = true;
      if(!bUseHidden && file_is_hidden(filename))
        bUse = false;

      if(bUse)
      {
        //Concatenate the URIs:
        Glib::RefPtr<Gnome::Vfs::Uri> uri = Gnome::Vfs::Uri::create(directorypath_uri);
        Glib::RefPtr<Gnome::Vfs::Uri> uri_appended = uri->append_file_name(filename);
        Glib::ustring filepath = uri_appended->to_string();
        
        //TODO Test whether it is a folder or a file:
        if(false) //if(Glib::file_test(filepath, Glib::FILE_TEST_IS_DIR))
        {
          //It's a folder:
          canonical_folder_path(filepath);

          //Add to list of folders:
          m_listFolders.push_back(filepath); //Might not be used - we check later.

          //Recurse to get files in this folder.
          build_list_of_files(filepath);
        }
        else
        {
          //It's a file:
          m_listFiles.push_back(filepath);
        }
      }
    }
  }
  catch(const Glib::Exception& ex)
  {
    Gtk::MessageDialog dialog(*this, "PrefixSuffix failed while building the list of files.");
    dialog.run();
    return false; //Stop trying.
  }

  return true; //Success.
}

Glib::ustring MainWindow::get_new_filepath(const Glib::ustring& filepath)
{
  //Separate the file path:
  Glib::ustring directory, filename_old;
  get_folder_and_file(filepath, directory, filename_old);

  Glib::ustring filename_new;

  //Prefix:
  if(m_pRadioPrefix->get_active())
  {
    Glib::ustring strPrefixReplace = m_pEntryPrefixReplace->get_text();
    Glib::ustring strPrefixWith = m_pEntryPrefixWith->get_text();

    if(strPrefixReplace.size()) //If an old prefix was specified
    {
      //If the old prefix is there:
      Glib::ustring::size_type posPrefix = filename_old.find(strPrefixReplace);
      if(posPrefix != Glib::ustring::npos)
      {
        //Remove old prefix:
        filename_new = filename_old.substr(strPrefixReplace.size());

        //Add new prefix:
        filename_new = strPrefixWith + filename_new;
      }
      else
      {
        //No change:
        filename_new = filename_old;
      }
    }
   else
    {
      //There's no old prefix to find, so just add the new prefix:
      filename_new = strPrefixWith + filename_old;
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
      Glib::ustring::size_type posSuffix = filename_old.rfind(strSuffixReplace);
      if(posSuffix != Glib::ustring::npos && ((filename_old.size() - posSuffix) == strSuffixReplace.size())) //if it was found, and if these were the last characters in the string.
      {
        //Remove old suffix:
        filename_new = filename_old.substr(0, posSuffix);

        //Add new suffix:
        filename_new += strSuffixWith;
      }
     else
      {
        //No change:
        filename_new = filename_old;
      }
    }
    else
    {
      //There's no old suffix to find, so just add the new suffix:
      filename_new += strSuffixWith;
    }
  }

  //Rejoin the directory and filename parts:
  return directory + filename_new;
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

bool MainWindow::on_transfer_progress(const Gnome::Vfs::Transfer::ProgressInfo& info)
{
  bool returnState = true;

  if(info)
  {
    switch (info.get_status())
    {
      case Gnome::Vfs::XFER_PROGRESS_STATUS_VFSERROR:
        std::cout << "VFS Error: " << gnome_vfs_result_to_string(static_cast<GnomeVFSResult>(info.get_vfs_status())) << std::endl;
        exit(1);

      case Gnome::Vfs::XFER_PROGRESS_STATUS_OVERWRITE:
        //std::cout << "Overwriting " << info.get_target_name() << " with " << info.get_source_name() << std::endl;
        exit(1);

      case Gnome::Vfs::XFER_PROGRESS_STATUS_OK:
      {
        //std::cout << "Status: OK" << std::endl;

        switch (info.get_phase())
        {
          case Gnome::Vfs::XFER_PHASE_INITIAL:
          {
            //Set the Progress Bar to 0%.
            m_progress_count = 0;
            m_pProgressBar->set_fraction(0);            
            
            //std::cout << "  Initial phase." << std::endl;
            break;
          }
          case Gnome::Vfs::XFER_CHECKING_DESTINATION:
            //std::cout << "  Checking destination." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_COLLECTING:
            //std::cout << "  Collecting file list." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_READYTOGO:
            //std::cout << "  Ready to go!" << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_OPENSOURCE:
            //std::cout << "  Opening source." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_OPENTARGET:
            //std::cout << "  Opening target." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_COPYING:
          /*
            std::cout << "  Copying '" << info.get_source_name() << "' to '" << info.get_target_name() <<
              "' (file " << info.get_file_index() << "/" << info.get_total_files() << ", byte " <<
              info.get_bytes_copied() << "/" << info.get_file_size() << " in file, " <<
              info.get_total_bytes_copied() << "/" << info.get_total_bytes() << std::endl;
          */
            break;
          case Gnome::Vfs::XFER_PHASE_MOVING:
          {
            //Set the progress bar to a suitable % to show how many files have been moved:
            m_progress_count++; //1 file has been moved.
            double fraction = m_progress_count / m_listFiles.size();
            m_pProgressBar->set_fraction(fraction);
            
            /*
            std::cout << "  Moving '" << info.get_source_name() << "' to '" << info.get_target_name() <<
              "' (file " << info.get_file_index() << "/" << info.get_total_files() << ", byte " <<
              info.get_bytes_copied() << "/" << info.get_file_size() << " in file, " <<
              info.get_total_bytes_copied() << "/" << info.get_total_bytes() << std::endl;
            */
            break;
          }
          case Gnome::Vfs::XFER_PHASE_READSOURCE:
            //std::cout << "  Reading source." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_CLOSESOURCE:
            //std::cout << "  Closing source." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_CLOSETARGET:
            //std::cout << "  Closing target." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_DELETESOURCE:
            //std::cout << "  Deleting source." << std::endl;
            break;
          case Gnome::Vfs::XFER_PHASE_FILECOMPLETED:
            /*
            std::cout << "  Done with '" << info.get_source_name() << "' -> '" << info.get_target_name() <<
              "', going next." << std::endl;
            */
            break;
          case Gnome::Vfs::XFER_PHASE_COMPLETED:
          {
            //std::cout << "  Completed." << std::endl;
            break;
          }
          default:
            std::cout << "prefixsuffix:  Unexpected phase " << info.get_phase() << std::endl;
        }
      }
      case Gnome::Vfs::XFER_PROGRESS_STATUS_DUPLICATE:
        break;
    }
  }

  return returnState;
}





