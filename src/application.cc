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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace PrefixSuffix
{

Application::Application()
: Gtk::Application("org.prefixsuffix.application",
    Gio::APPLICATION_HANDLES_COMMAND_LINE),
  m_window(0),
  m_stop_without_window(0)
{
  Glib::set_application_name("PrefixSuffix");

  signal_handle_local_options().connect(
    sigc::mem_fun(*this, &Application::on_handle_local_options), false);

  add_main_option_entry(Gio::Application::OPTION_TYPE_BOOL, "version", 'v', _("Show the application's version"));
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

  m_window->show();

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
  //std::cout << G_STRFUNC << ": debug" << std::endl;
  // The application has been started, so let's show a window:

  if(m_stop_without_window)
    return;

  if(!m_window)
    create_window();
}

void Application::on_startup()
{
  //std::cout << G_STRFUNC << ": debug" << std::endl;
  Gtk::Application::on_startup();
}

// We have to override on_command_line() because we have set
// Gio::APPLICATION_HANDLES_COMMAND_LINE, which we have to set when
// handling the handle_local_options signal.
// The default on_command_line() (used when 
// Gio::APPLICATION_HANDLES_COMMAND_LINE is not set) calls activate(),
// so, likewise, we now need to do that here.
int Application::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
  //std::cout << G_STRFUNC << ": debug" << std::endl;

  activate();

  //The local instance will eventually exit with this status code:
  return EXIT_SUCCESS;
}

int Application::on_handle_local_options(const Glib::RefPtr<Glib::VariantDict>& options)
{
  //std::cout << G_STRFUNC << ": debug" << std::endl;

  if(!options)
    std::cerr << G_STRFUNC << ": options is null." << std::endl;

  bool version = false;
  options->lookup_value("version", version);
  if (version)
  {
    const Glib::ustring msg = Glib::ustring::compose(_("%1 - Version %2"), 
      Glib::get_application_name(), PACKAGE_VERSION);
    std::cout << msg << std::endl;

    m_stop_without_window = true;

    //A non-negative result means stop the program:
    return EXIT_SUCCESS;
  }

  //Remove some options to show that we have handled them in the local instance,
  //so they won't be passed to the primary (remote) instance:
  options->remove("version");

  //A negative result means continue.
  return -1;
}

} //namespace PrefixSuffix
