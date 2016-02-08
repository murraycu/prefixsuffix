/*
 * Copyright 2002 The Bakery team
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

namespace Bakery {

namespace Conf {

AssociationBase::AssociationBase(const Glib::ustring& key, bool instant)
  : m_key(key)
  , m_instant(instant)
{
}

AssociationBase::~AssociationBase()
{
}

void
AssociationBase::add(const Glib::RefPtr<Gio::Settings>& conf_client)
{
  // Connect signals so that widget changes are saved instantly,
  // and so that widgets are updated immediately if someone else changes
  // the configuration data.

  m_conf_client = conf_client;
  if (is_instant()) {
    connect_widget(sigc::mem_fun(*this, &AssociationBase::on_widget_changed));

    // TODO: Use this when we can use glibmm 2.46,
    // so we can respond to changes from outside of our UI.
    // conf_client->connect_changed(get_key(), sigc::mem_fun(*this,
    // &AssociationBase::on_conf_changed));
  }
}

void
AssociationBase::load()
{
  if (get_conf_client())
    load_widget();
}

void
AssociationBase::save()
{
  if (get_conf_client())
    save_widget();
}

void
AssociationBase::on_widget_changed()
{
  // TODO: Should this be protected by a mutex to avoid overlapping callbacks?
  save();
}

void
AssociationBase::on_conf_changed()
{
  // TODO: Should this be protected by a mutex to avoid overlapping callbacks?
  load();
}

bool
AssociationBase::is_instant() const
{
  return m_instant;
}

Glib::ustring
AssociationBase::get_key() const
{
  return m_key;
}

// TODO:
// See Scott Meyers "Effective C++" item 21 for reasons why returned
// objects should be const:
// http://www.awprofessional.com/content/images/0201924889/items/item21.html

Glib::RefPtr<const Gio::Settings>
AssociationBase::get_conf_client() const
{
  return m_conf_client;
}

Glib::RefPtr<Gio::Settings>
AssociationBase::get_conf_client()
{
  return m_conf_client;
}

} // namespace Conf

} // namespace Bakery
