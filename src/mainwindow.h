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

#ifndef PREFIXSUFFIX_MAINWINDOW_H
#define PREFIXSUFFIX_MAINWINDOW_H

#include <gtkmm.h>
#include <bakery/Configuration/Client.h>
#include "renamer.h"

namespace PrefixSuffix
{

class MainWindow : public Gtk::Window
{
public:
  MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  virtual ~MainWindow();

private:

  // Signal Handlers:
  void on_radio_prefix_clicked();
  void on_radio_suffix_clicked();

  void on_button_process();
  void on_button_close();
  void on_button_stop();

  void do_rename();

  void set_ui_locked(bool locked = true);
  void show_error(const Glib::ustring& message);
  void stop_process(const Glib::ustring& message = Glib::ustring());
  void clear_lists();

  Glib::RefPtr<Gdk::Cursor> m_old_cursor;

protected:
  virtual void on_hide(); //override.

  void on_renamer_progress(const double fraction);
  void on_renamer_stopped(const Glib::ustring& error_message);

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

  //GSettings:
  Bakery::Conf::Client m_conf_client;

  Renamer* m_renamer;
};

} //namespace PrefixSuffix

#endif /* PREFIXSUFFIX_MAINWINDOW_H */
