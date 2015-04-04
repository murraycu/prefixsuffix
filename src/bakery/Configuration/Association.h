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

#ifndef BAKERY_CONFIGURATION_ASSOCIATION_H
#define BAKERY_CONFIGURATION_ASSOCIATION_H

#include "bakery/Configuration/AssociationBase.h"
#include <gtkmm/togglebutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/range.h>
#include <gtkmm/spinbutton.h>

namespace Bakery
{
namespace Conf
{
  
template< class T_Widget >
class Association;

template< class T_Widget >
class AssociationCreation : public AssociationBase
{
public:
  static const AssociationPtr create(const Glib::ustring& full_key, T_Widget& widget, bool instant)
  {
    return AssociationPtr( new Association<T_Widget>(full_key, widget, instant) );
  }

  virtual ~AssociationCreation()
  {
  }

  
protected:
  AssociationCreation(const Glib::ustring& full_key, T_Widget& widget, bool instant)
  : AssociationBase(full_key,instant),
    m_widget(widget)
  {
  }

  AssociationCreation(const AssociationCreation& other); // Not implemented

  T_Widget& m_widget;
};

template< class T_Widget >
class Association : public AssociationCreation<T_Widget>
{
public:

  //For some reason, the compiler can not use this if it is in the base class:
  //typedef AssociationCreation<T_Widget>::Callback Callback;
  typedef sigc::slot<void> Callback;
  
  /** These methods must be implemented explicitly for each
   * specialization of Association<T> to provide appropriate
   * behaviors based on the widget type.
   */
  virtual void connect_widget(Callback on_widget_changed);
  virtual void load_widget();
  virtual void save_widget();
  
protected:
  Association(const Glib::ustring& full_key, T_Widget& widget, bool instant)
  : AssociationCreation<T_Widget>(full_key, widget, instant)
  {};
};

//----------------------------------------------------------------------
// Association<T> specializations. These provide widget/key
// association behaviors that are specific to individual widget types.
//----------------------------------------------------------------------

//SpinButton specializatipn:

template<>
class Association<Gtk::SpinButton>  : public AssociationCreation<Gtk::SpinButton>
{
public:
  typedef Gtk::SpinButton type_widget;
  
  void connect_widget(Callback widget_changed)
  {
    m_widget.signal_value_changed().connect(widget_changed);
  }

  void load_widget()
  {
    double val = get_conf_client()->get_float(get_key());
      if (m_widget.get_value() != val)
        m_widget.set_value(val);
  }

  void save_widget()
  {
    double val = m_widget.get_value();
    double existing_val = get_conf_client()->get_float(get_key());
      if (existing_val != val)
        get_conf_client()->set(get_key(), val);
  }
  
 
  Association(const Glib::ustring& full_key, type_widget& widget, bool instant)
  : AssociationCreation<type_widget>(full_key, widget, instant)
  {};
};


//Gtk::Entry specializatipn:

template<>
class Association<Gtk::Entry>  : public AssociationCreation<Gtk::Entry>
{
public:
  typedef Gtk::Entry type_widget;
  
  void connect_widget(Callback widget_changed)
  {
    m_widget.signal_changed().connect(widget_changed);
  }

  void load_widget()
  {
    // Only set it if it has changed (avoids excess notifications).
    Glib::ustring val = get_conf_client()->get_string(get_key());
    if (m_widget.get_text() != val)
      m_widget.set_text(val);
  }

  void save_widget()
  {
    // Only set it if it has changed (avoids excess notifications).
    Glib::ustring val = m_widget.get_text();
    Glib::ustring existing_val = get_conf_client()->get_string(get_key());
    {
      if (existing_val != val)
        get_conf_client()->set(get_key(), val);
    }
  }
  
  Association(const Glib::ustring& full_key, type_widget& widget, bool instant)
  : AssociationCreation<type_widget>(full_key, widget, instant)
  {};
};


//Gtk::ToggleButton specializatipn:

template<>
class Association<Gtk::ToggleButton>  : public AssociationCreation<Gtk::ToggleButton>
{
public:
  typedef Gtk::ToggleButton type_widget;
  
  void connect_widget(Callback widget_changed)
  {
    m_widget.signal_toggled().connect(widget_changed);
  }

  void load_widget()
  {
    // Only set it if it has changed (avoids excess notifications).
    bool val = get_conf_client()->get_bool(get_key());
    if (m_widget.get_active() != val)
      m_widget.set_active(val);
  }

  void save_widget()
  {
    // Only set it if it has changed (avoids excess notifications).
    bool val = m_widget.get_active();
    bool existing_val = get_conf_client()->get_bool(get_key());
    if (existing_val != val)
      get_conf_client()->set(get_key(), val);
  }
  
  Association(const Glib::ustring& full_key, type_widget& widget, bool instant)
  : AssociationCreation<type_widget>(full_key, widget, instant)
  {};
};


//Gtk::Range specializatipn:

template<>
class Association<Gtk::Range>  : public AssociationCreation<Gtk::Range>
{
public:
  typedef Gtk::Range type_widget;
  
  void connect_widget(Callback widget_changed)
  {
    m_widget.signal_value_changed().connect(widget_changed);
  }

  void load_widget()
  {
    double val = get_conf_client()->get_float(get_key());
    if (m_widget.get_value() != val)
      m_widget.set_value(val);
  }

  void save_widget()
  {
    double val = m_widget.get_value();
    double existing_val = get_conf_client()->get_float(get_key());
    if (existing_val != val)
      get_conf_client()->set(get_key(), val);
  }

  Association(const Glib::ustring& full_key, type_widget& widget, bool instant)
  : AssociationCreation<type_widget>(full_key, widget, instant)
  {};
};



} //namespace Conf

} //namespace Bakery

#endif //BAKERY_CONFIGURATION_ASSOCIATION_H
