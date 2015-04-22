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

#include "application.h"
#include "mainwindow.h"
#include <gtkmm/builder.h>
#include <glibmm/optioncontext.h>
#include <iostream>

#include <glibmm/i18n.h>

namespace PrefixSuffix
{

Application::Application()
: Gtk::Application("org.prefixsuffix.application"),
  m_window(0)
{
  Glib::set_application_name("PrefixSuffix");
}

Glib::RefPtr<Application> Application::create()
{
  return Glib::RefPtr<Application>( new Application() );
}

void Application::create_window()
{
  if(m_window)
  {
    std::cerr << G_STRFUNC << ": m_window already exists." << std::endl;
    return;
  }

  //Glade:
  Glib::RefPtr<Gtk::Builder> builder;

  //Look for the installed .glade file:
  try
  {
    builder = Gtk::Builder::create_from_file(PREFIXSUFFIX_GLADEDIR "prefixsuffix.glade");
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

  if(!builder)
  {
    //Try the local directory. Maybe somebody is just running this without installing it:
    g_warning("Prefix: Failed to find installed glade file. Looking for it in the current directory.\n");
    builder = Gtk::Builder::create_from_file("prefixsuffix.glade");
  }

  //If it still can't be found:
  if(!builder)
    g_warning("PrefixSuffix: Could not find prefixsuffix.glade.\n");

  builder->get_widget_derived("window", m_window);

  //Make sure that the application runs for as long this window is still open:
  add_window(*m_window);

  //Delete the window when it is hidden:
  m_window->signal_hide().connect(
    sigc::mem_fun(*this, &Application::on_window_hide));
}

void Application::on_window_hide()
{
  delete m_window;
  m_window = 0;
}

void Application::on_activate()
{
  // The application has been started, so let's show a window:
  m_window->show();
}

void Application::on_startup()
{
  Gtk::Application::on_startup();
  create_window();
}

} //namespace PrefixSuffix
