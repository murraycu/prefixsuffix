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


namespace Bakery
{

namespace Conf
{

Client::Client(const Glib::ustring& configuration_directory)
: m_directory(configuration_directory)
{
  m_refClient = Gio::Settings::create(m_directory);
}

Client::~Client()
{
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

void Client::load()
{
  for (type_vecWidgets::iterator iter = m_vecWidgets.begin(); iter != m_vecWidgets.end(); ++iter)
    (*iter)->load();
}

void Client::save()
{
  for (type_vecWidgets::iterator iter = m_vecWidgets.begin(); iter != m_vecWidgets.end(); ++iter)
    (*iter)->save();
}


} //namespace Conf

} //namespace Bakery

