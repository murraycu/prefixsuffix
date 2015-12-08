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

#ifndef BAKERY_CONFIGURATION_ASSOCIATIONBASE_H
#define BAKERY_CONFIGURATION_ASSOCIATIONBASE_H

#include <gtkmm/widget.h>
#include <giomm/settings.h>
#include "bakery/Utilities/sharedptr.h"

namespace Bakery
{

namespace Conf
{

/** Provides behaviors that are common to all widget/key associations.
 */
class AssociationBase : public sigc::trackable
{
public:
  typedef sharedptr<AssociationBase> AssociationPtr;
  typedef sharedptr<const AssociationBase> AssociationConstPtr;
    
  void add(const Glib::RefPtr<Gio::Settings>& conf_client);

  void load();
  void save();

  virtual ~AssociationBase();
    
protected:
  AssociationBase(const Glib::ustring& full_key, bool instant);
  AssociationBase(const AssociationBase& other); // Not implemented


  bool is_instant() const;
  Glib::ustring get_key() const;
  Glib::RefPtr<const Gio::Settings> get_conf_client() const;
  Glib::RefPtr<Gio::Settings> get_conf_client();
    
  typedef sigc::slot<void> Callback;
  virtual void connect_widget(Callback on_widget_changed) = 0;

private:
  virtual void load_widget() = 0;
  virtual void save_widget() = 0;

  void on_widget_changed();
  void on_conf_changed();

  Glib::ustring m_key;
  bool m_instant;
  Glib::RefPtr<Gio::Settings> m_conf_client;
};

} //namespace Conf

} //namespace Bakery

#endif //BAKERY_CONFIGURATION_ASSOCIATIONBASE_H
