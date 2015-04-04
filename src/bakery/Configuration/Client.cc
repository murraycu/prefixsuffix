/*
 * Copyright 2002 Murray Cumming
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

#include "bakery/Configuration/Client.h"
#include <gtkmm/togglebutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/range.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/combo.h>
#include <gtkmm/optionmenu.h>

namespace Bakery
{

namespace Conf
{

Client::Client(const Glib::ustring& configuration_directory)
: m_directory(configuration_directory)
{
  m_refClient = Gnome::Conf::Client::get_default_client();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  m_refClient->add_dir(m_directory);
#else
  std::auto_ptr<Glib::Error> error;
  m_refClient->add_dir(m_directory, Gnome::Conf::CLIENT_PRELOAD_NONE, error);
  // Ignore error, we cannot do anything useful in the constructor
#endif
}

Client::~Client()
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  m_refClient->remove_dir(m_directory);
#else
  std::auto_ptr<Glib::Error> error;
  m_refClient->remove_dir(m_directory, error);
#endif
}

void Client::add(const Glib::ustring& key, Gtk::Widget& widget)
{
  add_implementation(key, widget, false);
}

void Client::add_instant(const Glib::ustring& key, Gtk::Widget& widget)
{
  add_implementation(key, widget, true);
}

void Client::add_implementation(const Glib::ustring& key, Gtk::Widget& widget, bool instant = false)
{
  if (dynamic_cast<Gtk::SpinButton*>(&widget))
    add_association(key, static_cast<Gtk::SpinButton&>(widget), instant);
  else if (dynamic_cast<Gtk::Entry*>(&widget))
    add_association(key, static_cast<Gtk::Entry&>(widget), instant);
  else if (dynamic_cast<Gtk::ToggleButton*>(&widget))
    add_association(key, static_cast<Gtk::ToggleButton&>(widget), instant);
  else if (dynamic_cast<Gtk::Range*>(&widget))
    add_association(key, static_cast<Gtk::Range&>(widget), instant);
  // TODO: Support for connecting a Gtk::Curve widget to an array of floating point values
}

#ifdef GLIBMM_EXCEPTIONS_ENABLED
void Client::load()
#else
void Client::load(std::auto_ptr<Glib::Error>& error)
#endif
{
  for (type_vecWidgets::iterator iter = m_vecWidgets.begin(); iter != m_vecWidgets.end(); ++iter)
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    (*iter)->load();
#else
    (*iter)->load(error);
#endif
}

#ifdef GLIBMM_EXCEPTIONS_ENABLED
void Client::save()
#else
void Client::save(std::auto_ptr<Glib::Error>& error)
#endif
{
  for (type_vecWidgets::iterator iter = m_vecWidgets.begin(); iter != m_vecWidgets.end(); ++iter)
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    (*iter)->save();
#else
    (*iter)->save(error);
#endif
}


} //namespace Conf

} //namespace Bakery

