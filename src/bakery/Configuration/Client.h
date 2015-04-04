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

#ifndef BAKERY_CONFIGURATION_CONFCLIENT_H
#define BAKERY_CONFIGURATION_CONFCLIENT_H

#include "bakery/Configuration/Association.h"

namespace Bakery
{

namespace Conf
{

/** Configuration Client
 * Allows you to associate widget "values" with configuration keys,
 * and then load() and save() them all at once.
 * The "value" depends on the widget:
 *    Gtk::Entry - text (gconf string).
 *    Gtk::CheckButton, Gtk::RadioButton - active (gconf bool).
 *    Gtk::Range (e.g. scales and scrollbars) - position (gconf float).
 *    Gtk::SpinButton - value (gconf float).
 *    Gtk::Combo - text (gconf string).
 *    Gtk::OptionMenu - item number (gconf int); ideally we would
 *      prefer to use a string representation, but that's not realistic.
 *
 * Advantages compared to Gnome::Conf::Client:
 * - Avoids repetition of key names.
 * - Avoids repetition of the directory name.
 * - Avoids manual code to get and set widget values.
 * - Avoids 2 sets of very similar code, for load and save.
 * - No need for Glib::RefPtr<>
 * - Can do instant apply and instant update with add_instant()..
 */
class Client : public sigc::trackable
{
public:
  Client(const Glib::ustring& configuration_directory);
  virtual ~Client();

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  virtual void load();
  virtual void save();
#else
  virtual void load(std::auto_ptr<Glib::Error>& error);
  virtual void save(std::auto_ptr<Glib::Error>& error);
#endif

  /// e.g. conf_client.add("user_name", m_EntryUserName);
  virtual void add(const Glib::ustring& key, Gtk::Widget& widget);
  virtual void add_instant(const Glib::ustring& key, Gtk::Widget& widget);

protected:

  /** Override this method to add recognition of additional
   * widget types to a derived class of Client. You must also
   * provide an implementation for the specialization of Association<T>
   * for the widget type(s) you are adding support for.
   */
  virtual void add_implementation(const Glib::ustring& key, Gtk::Widget& widget, bool instant);

  typedef Bakery::Conf::AssociationBase::AssociationPtr AssociationPtr;

public:

protected:
  template< class T_Widget >
  void add_association(const Glib::ustring& key, T_Widget& widget, bool instant)
  {
    Glib::ustring full_key = m_directory + "/" + key;
    AssociationPtr assoc = Association<T_Widget>::create(full_key, widget, instant);
    m_vecWidgets.push_back(assoc);
    assoc->add(m_refClient);
  }

private:
  Glib::RefPtr<Gnome::Conf::Client> m_refClient;
  Glib::ustring m_directory;

  typedef std::vector<AssociationPtr> type_vecWidgets;
  type_vecWidgets m_vecWidgets;
};

} //namespace Conf

} //namespace Bakery


#endif //BAKERY_CONFIGURATION_CONFCLIENT_H
