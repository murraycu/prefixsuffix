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

#include "renamer.h"
#include <iostream>
#include <glibmm/i18n.h>


Renamer::Renamer(const Glib::ustring& directory_path,
  const Glib::ustring& prefix_replace, const Glib::ustring prefix_with,
  const Glib::ustring& suffix_replace, const Glib::ustring suffix_with,
  bool recurse_into_folders, bool operate_on_folders,
  bool operate_on_hidden)
  : m_directory_path(directory_path),
    m_prefix_replace(prefix_replace),
    m_prefix_with(prefix_with),
    m_suffix_replace(suffix_replace),
    m_suffix_with(suffix_with),
    m_recurse_into_folders(recurse_into_folders),
    m_operate_on_folders(operate_on_folders),
    m_operate_on_hidden(operate_on_hidden),
    m_prefix(!prefix_replace.empty() && !prefix_with.empty()),
    m_suffix(!suffix_replace.empty() && !suffix_with.empty()),
    m_progress_max(0),
    m_progress_count(0)
{
}

Renamer::~Renamer()
{
}

void Renamer::start()
{
  m_signal_progress.emit(0.0f);

  //Check that there is enough information:
  if(m_directory_path.empty())
  {
    std::cerr << G_STRFUNC << ": m_directory_path is empty." << std::endl;
    return;
  }

  if( m_prefix_replace.empty() && m_prefix_with.empty() && m_suffix_replace.empty() && m_suffix_with.empty() )
  {
    std::cerr << G_STRFUNC << ": prefix and suffix strings are empty." << std::endl;
    return;
  }

  build_list_of_files();
}

void Renamer::on_set_display_name(const Glib::RefPtr<Gio::AsyncResult>& result,
  const Glib::RefPtr<Gio::File>& file)
{
  try
  {
    Glib::RefPtr<Gio::File> file_renamed = file->set_display_name_finish(result);
    if(!file_renamed)
    {
      std::cerr << G_STRFUNC << ": null result from Gio::File::set_display_name_finish()." << std::endl;
      stop_process(_("PrefixSuffix failed while renaming the files."));
      return;
    }
  }
  catch(const Glib::Error& ex)
  {
    std::cerr << G_STRFUNC << ": Exception from Gio::File::set_display_name_finish(): " << ex.what() << std::endl;
    stop_process(_("PrefixSuffix failed while renaming the files."));
    return;
  }

  rename_next_file();
}

void Renamer::rename_next_file()
{
  if(m_files.empty())
  {
    //We have finished, so stop:
    m_signal_progress.emit(1.0f);
    stop_process();
    return;
  }

  //Tell the UI about our progress:
  //TODO: Only signal for every n files?
  const double progress_fraction = m_count / (m_count - m_files.size());
  m_signal_progress.emit(progress_fraction);

  //Rename the next file:
  const Glib::ustring uri = m_files.front();
  m_files.pop();
  const Glib::ustring uriNew = m_files_new.front();
  m_files_new.pop();
  const Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(uri);

  //std::cout << G_STRFUNC << ": debug: uri: " << uri << std::endl;
  //std::cout << G_STRFUNC << ": debug: uriNew: " << uriNew << std::endl;
  file->set_display_name_async(uriNew,
    sigc::bind(
      sigc::mem_fun(*this, &Renamer::on_set_display_name),
      file),
    m_cancellable);
}

void Renamer::do_rename_files()
{
  if(m_files.empty())
  {
    stop_process(_("No files have this prefix or suffix, so no files will be renamed."));
    return;
  }

  //Rename the files:
  //TODO: Can we start a batch of these instead of doing them one by one?
  m_count = m_files.size();
  rename_next_file();
}

void Renamer::clear_lists()
{
  m_files = type_queue_strings(); //There is no std::queue<>::clear() methods.
  m_files_new = type_queue_strings();
  m_folders = type_queue_strings();
  m_folders_new = type_queue_strings();
}

void Renamer::build_list_of_files()
{
  //This is the first run, rather than a recursion.
  clear_lists();
  m_cancellable = Gio::Cancellable::create();

  //recurse:
  build_list_of_files(m_directory_path);  
}

void Renamer::request_next_files(const Glib::RefPtr<Gio::File>& directory, const Glib::RefPtr<Gio::FileEnumerator>& enumerator)
{
  enumerator->next_files_async(
    sigc::bind(
      sigc::mem_fun(*this, &Renamer::on_directory_next_files),
      directory, enumerator),
    m_cancellable,
    5 /* number to request at once */);
}

void Renamer::on_directory_next_files(const Glib::RefPtr<Gio::AsyncResult>& result,
  const Glib::RefPtr<Gio::File>& directory, const Glib::RefPtr<Gio::FileEnumerator>& enumerator)
{
  typedef std::list<Glib::ustring> type_list_strings;
  type_list_strings list_folders;

  try
  {
    typedef std::list< Glib::RefPtr<Gio::FileInfo> > type_list_file_info;
    type_list_file_info list_info = enumerator->next_files_finish(result);

    //If we have finished enumerating this directory:
    if(list_info.empty())
    {
      type_set_files::iterator iter = m_directory_enumerations_in_progress.find(directory);
      if(iter != m_directory_enumerations_in_progress.end())
        m_directory_enumerations_in_progress.erase(iter);

      if(m_directory_enumerations_in_progress.empty())
      {
        //We have finished listing all the files and directories:
        do_rename_files();
      }

      return;
    }

    for(type_list_file_info::const_iterator iter = list_info.begin(); iter != list_info.end(); ++iter)
    {
      Glib::RefPtr<Gio::FileInfo> info = *iter;
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
        if(!m_operate_on_hidden && file_is_hidden(basename))
          bUse = false;
      }

      if(bUse)
      {
        Glib::ustring uri = child->get_uri();

        const Gio::FileType file_type = info->get_file_type();
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
            m_files.push(uri);
            m_files_new.push(basename_new);
          }
        }
      }
    }
  }
  catch(const Glib::Error& ex)
  {
    stop_process(_("PrefixSuffix failed while obtaining the list of files."));
    std::cerr << G_STRFUNC << ": Exception with directory uri=" << directory->get_uri() << ": " << ex.what() << std::endl;

    return; //Stop trying.
  }

  request_next_files(directory, enumerator);

  // Examine the sub-directories:
  for(type_list_strings::const_iterator iter = list_folders.begin(); iter != list_folders.end(); ++iter)
  {
    //Recurse to get files in this folder.
    const Glib::ustring child_dir = *iter;

    if(m_recurse_into_folders)
    {
      build_list_of_files(child_dir);
    }

    if(m_operate_on_folders)
    {
      const Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(child_dir);
      const std::string basename = file->get_basename();
      const Glib::ustring& filepath_new = get_new_basename(basename);
      if(child_dir != filepath_new) //Ignore it if the prefix/suffix change had no effect
      {
        m_folders.push(child_dir);
        m_folders_new.push(basename);
      }
    }
  }
}

void Renamer::on_directory_enumerate_children(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gio::File>& directory)
{
  try
  {
    Glib::RefPtr<Gio::FileEnumerator> enumerator = directory->enumerate_children_finish(result);
    request_next_files(directory, enumerator);
  }
  catch(const Glib::Error& ex)
  {
    stop_process(_("PrefixSuffix failed while obtaining the list of files."));
    std::cerr << G_STRFUNC << ": Exception with directory uri=" << directory->get_uri() << ": " << ex.what() << std::endl;

    return; //Stop trying.
  }
}

void Renamer::build_list_of_files(const Glib::ustring& directorypath_uri_in)
{
  //This is a recursion.

  Glib::ustring directorypath_uri = directorypath_uri_in;
  canonical_folder_path(directorypath_uri);

  //Get the filenames in the directory:
  Glib::RefPtr<Gio::File> directory = Gio::File::create_for_uri(directorypath_uri);
  m_directory_enumerations_in_progress.insert(
    m_directory_enumerations_in_progress.end(),
    directory);
  directory->enumerate_children_async(
    sigc::bind(
      sigc::mem_fun(*this, &Renamer::on_directory_enumerate_children),
      directory),
    m_cancellable,
    G_FILE_ATTRIBUTE_STANDARD_NAME "," G_FILE_ATTRIBUTE_STANDARD_TYPE "," G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN);
}

Glib::ustring Renamer::get_new_basename(const Glib::ustring& basename)
{
  Glib::ustring filename_new;

  //Prefix:
  if(m_prefix)
  {
    if(!m_prefix_replace.empty()) //If an old prefix was specified
    {
      //If the old prefix is there:
      Glib::ustring::size_type posPrefix = basename.find(m_prefix_replace);
      if(posPrefix != Glib::ustring::npos)
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
      Glib::ustring::size_type posSuffix = basename.rfind(m_suffix_replace);
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

bool Renamer::file_is_hidden(const Glib::ustring& filename) //static
{
  //See whether it has "." at the start:
  Glib::ustring::size_type size = filename.size();
  if(size > 0)
    return (filename[0] == '.');
  else
    return false;
}

void Renamer::canonical_folder_path(Glib::ustring& folderpath) //static
{
  //Make sure that it has "/" at the end:
  Glib::ustring::size_type size = folderpath.size();
  if(size > 0)
  {
    if(folderpath[size - 1] != '/') //TODO: Cross platform - mac uses ':'.
      folderpath += "/";
  }
}

void Renamer::get_folder_and_file(const Glib::ustring& filepath, Glib::ustring& folderpath, Glib::ustring& filename)
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

void Renamer::show_error(const Glib::ustring& message)
{
  //TODO: Gtk::MessageDialog dialog(*this, message);
  //dialog.run();
}

void Renamer::stop()
{
  stop_process();
}

void Renamer::stop_process(const Glib::ustring& error_message)
{
  if(m_cancellable)
  {
    m_cancellable->cancel();
    m_cancellable.reset();
  }

  clear_lists();

  m_signal_stopped.emit(error_message);
}

Renamer::type_signal_stopped Renamer::signal_stopped()
{
  return m_signal_stopped;
}

Renamer::type_signal_progress Renamer::signal_progress()
{
  return m_signal_progress;
}




