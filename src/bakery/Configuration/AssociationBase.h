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

#include <gconfmm.h>
#include <gtkmm/widget.h>
#include <bakery/Utilities/sharedptr.h>

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
    
  void add(const Glib::RefPtr<Gnome::Conf::Client>& conf_client);

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void load();
  void save();
#else
  // TODO: Ignore errors in these functions?
  void load(std::auto_ptr<Glib::Error>& error);
  void save(std::auto_ptr<Glib::Error>& error);
#endif

  virtual ~AssociationBase();
    
protected:
  AssociationBase(const Glib::ustring& full_key, bool instant);
  AssociationBase(const AssociationBase& other); // Not implemented


  bool is_instant() const;
  Glib::ustring get_key() const;
  Glib::RefPtr<const Gnome::Conf::Client> get_conf_client() const;
  Glib::RefPtr<Gnome::Conf::Client> get_conf_client();
    
  typedef sigc::slot<void> Callback;
  virtual void connect_widget(Callback on_widget_changed) = 0;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  virtual void load_widget() = 0;
  virtual void save_widget() = 0;
#else
  virtual void load_widget(std::auto_ptr<Glib::Error>& error) = 0;
  virtual void save_widget(std::auto_ptr<Glib::Error>& error) = 0;
#endif

  void on_widget_changed();
  void on_conf_changed(guint cnxn_id, Gnome::Conf::Entry entry);

  Glib::ustring m_key;
  bool m_instant;
  Glib::RefPtr<Gnome::Conf::Client> m_conf_client;
};

} //namespace Conf

} //namespace Bakery

#endif //BAKERY_CONFIGURATION_ASSOCIATIONBASE_H
