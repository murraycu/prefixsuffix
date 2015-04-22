/* PrefixSuffix
 *
 * Copyright (C) 2015 Murray Cumming
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#ifndef PREFIXSUFFIX_APPLICATION_H
#define PREFIXSUFFIX_APPLICATION_H

#include <gtkmm/application.h>

namespace PrefixSuffix
{

class MainWindow;

class Application: public Gtk::Application
{
protected:
  Application();

public:
  static Glib::RefPtr<Application> create();
    
protected:
  //Overrides of default signal handlers:
  virtual void on_startup();
  virtual void on_activate();

private:
  void create_window();

  void on_window_hide();

  //We have just one window in this single-instance application,
  //because we silently store the settings and don't want to have UI
  //to deal with multiple settings in multiple active instances.
  MainWindow* m_window;
};

} //namespace PrefixSuffix

#endif /* GTKMM_APPLICATION_H */
