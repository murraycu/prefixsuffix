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
    m_renamer(0)
{
  builder->get_widget("grid_inputs", m_grid_inputs);

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
  builder->get_widget("button_stop", m_button_stop);
  m_button_close->signal_clicked().connect( sigc::mem_fun(*this, &MainWindow::on_button_stop) );

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
  //The UI will be unlocked in stop_process().
  set_ui_locked();

  do_rename();
}

void MainWindow::set_ui_locked(bool locked)
{
  //Prevent/allowchanges to any of the criteria:
  m_grid_inputs->set_sensitive(!locked);

  //Prevent/allow a new start or a close:
  m_button_process->set_sensitive(!locked);
  m_button_close->set_sensitive(!locked);

  //Prevent/allow a stop.
  m_button_stop->set_sensitive(locked);

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
  if(m_renamer)
  {
    stop_process(_("A rename already seems to be in process."));
    return;
  }

  //Check that there is enough information:
  const Glib::ustring uri = m_entry_path->get_uri();
  if(uri.empty())
  {
    stop_process(_("Please choose a directory."));
    return;
  }
  /*
  else if ( !(Glib::file_test( m_entry_path->get_text(), Glib::FILE_TEST_IS_DIR)) )
  {

  }
  */
  if( m_radio_prefix->get_active() && (m_entry_prefix_replace->get_text().empty()) &&  (m_entry_prefix_with->get_text().empty()) )
  {
    stop_process(_("Please enter values in the prefix fields."));
    return;
  }

  if( m_radio_prefix->get_active() && (m_entry_prefix_replace->get_text() == m_entry_prefix_with->get_text()) )
  {
    stop_process(_("The Replace and With values are identical."));
    return;
  }

  if( m_radio_suffix->get_active() && (m_entry_suffix_replace->get_text().empty()) &&  (m_entry_suffix_with->get_text().empty()) )
  {
    stop_process(_("Please enter values in the suffix fields."));
    return;
  }

  if( m_radio_suffix->get_active() && (m_entry_suffix_replace->get_text() == m_entry_suffix_with->get_text()) )
  {
    stop_process(_("The Replace and With values are identical."));
    return;
  }

  m_renamer = new Renamer(uri,
   m_entry_prefix_replace->get_text(), m_entry_prefix_with->get_text(),
   m_entry_prefix_replace->get_text(), m_entry_suffix_with->get_text(),
   m_check_recurse->get_active(), m_check_folders->get_active(),
   m_check_hidden->get_active());

  //We have enough to start processing:
  //Bakery::BusyCursor busyCursor(*this);

  m_renamer->signal_stopped().connect(
    sigc::mem_fun(*this, &MainWindow::on_renamer_stopped));
  m_renamer->signal_progress().connect(
    sigc::mem_fun(*this, &MainWindow::on_renamer_progress));
  m_renamer->start();
}

void MainWindow::on_button_close()
{
  //Just in case:
  stop_process();

  hide();
}

void MainWindow::on_button_stop()
{
  //Just in case:
  stop_process();
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

void MainWindow::on_renamer_progress(const double fraction)
{
   m_progress_bar->set_fraction(fraction);
}

void MainWindow::on_renamer_stopped(const Glib::ustring& error_message)
{
  delete m_renamer;
  m_renamer = 0;

  stop_process(error_message);
}

void MainWindow::stop_process(const Glib::ustring& error_message)
{
  if(m_renamer) {
    m_renamer->stop();

    delete m_renamer;
    m_renamer = 0;
  }

  if(!error_message.empty())
    show_error(error_message);

  set_ui_locked(false);
}






