// -*- C++ -*-

/* main.cc
 *
 * Copyright (C) 2000 Murray Cumming
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtkmm.h>
#include "mainwindow.h"

int
main(int argc, char* argv[])
{
  Gtk::Main main(argc, argv);

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

  MainWindow* window = 0;
  builder->get_widget_derived("window", window);

  main.run(*window);

  delete window;

  return 0;
}
