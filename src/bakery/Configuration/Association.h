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
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  virtual void load_widget();
  virtual void save_widget();
#else
  virtual void load_widget(std::auto_ptr<Glib::Error>& error);
  virtual void save_widget(std::auto_ptr<Glib::Error>& error);
#endif
  
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

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void load_widget()
#else
  void load_widget(std::auto_ptr<Glib::Error>& error)
#endif // GLIBMM_EXCEPTIONS_ENABLED
  {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    double val = get_conf_client()->get_float(get_key());
#else
    double val = get_conf_client()->get_float(get_key(), error);
    if (error.get() != NULL)
#endif
      if (m_widget.get_value() != val)
        m_widget.set_value(val);
  }

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void save_widget()
#else
  void save_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
    double val = m_widget.get_value();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    double existing_val = get_conf_client()->get_float(get_key());
#else
    double existing_val = get_conf_client()->get_float(get_key(), error);
    if (error.get() != NULL)
#endif
      if (existing_val != val)
#ifdef GLIBMM_EXCEPTIONS_ENABLED
        get_conf_client()->set(get_key(), val);
#else
        get_conf_client()->set(get_key(), val, error);
#endif
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

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void load_widget()
#else
  void load_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
    // Only set it if it has changed (avoids excess notifications).
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    Glib::ustring val = get_conf_client()->get_string(get_key());
#else
    Glib::ustring val = get_conf_client()->get_string(get_key(), error);
    if (error.get() != NULL)
#endif
      if (m_widget.get_text() != val)
        m_widget.set_text(val);
  }

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void save_widget()
#else
  void save_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
    // Only set it if it has changed (avoids excess notifications).
    Glib::ustring val = m_widget.get_text();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    Glib::ustring existing_val = get_conf_client()->get_string(get_key());
#else
    Glib::ustring existing_val = get_conf_client()->get_string(get_key(), error);
    if (error.get() != NULL)
#endif
    {
      if (existing_val != val)
#ifdef GLIBMM_EXCEPTIONS_ENABLED
        get_conf_client()->set(get_key(), val);
#else
        get_conf_client()->set(get_key(), val, error);
#endif
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

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void load_widget()
#else
  void load_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
    // Only set it if it has changed (avoids excess notifications).
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    bool val = get_conf_client()->get_bool(get_key());
#else
    bool val = get_conf_client()->get_bool(get_key(), error);
    if (error.get() != NULL)
#endif
      if (m_widget.get_active() != val)
        m_widget.set_active(val);
  }

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void save_widget()
#else
  void save_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
    // Only set it if it has changed (avoids excess notifications).
    bool val = m_widget.get_active();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    bool existing_val = get_conf_client()->get_bool(get_key());
#else
    bool existing_val = get_conf_client()->get_bool(get_key(), error);
    if (error.get() != NULL)
#endif
      if (existing_val != val)
#ifdef GLIBMM_EXCEPTIONS_ENABLED
      get_conf_client()->set(get_key(), val);
#else
      get_conf_client()->set(get_key(), val, error);
#endif
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

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void load_widget()
#else
  void load_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    double val = get_conf_client()->get_float(get_key());
#else
    double val = get_conf_client()->get_float(get_key(), error);
    if (error.get() != NULL)
#endif
      if (m_widget.get_value() != val)
        m_widget.set_value(val);
  }

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  void save_widget()
#else
  void save_widget(std::auto_ptr<Glib::Error>& error)
#endif
  {
    double val = m_widget.get_value();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    double existing_val = get_conf_client()->get_float(get_key());
#else
    double existing_val = get_conf_client()->get_float(get_key(), error);
    if (error.get() != NULL)
#endif
      if (existing_val != val)
#ifdef GLIBMM_EXCEPTIONS_ENABLED
        get_conf_client()->set(get_key(), val);
#else
        get_conf_client()->set(get_key(), val, error);
#endif
  }
  

  Association(const Glib::ustring& full_key, type_widget& widget, bool instant)
  : AssociationCreation<type_widget>(full_key, widget, instant)
  {};
};



} //namespace Conf

} //namespace Bakery

#endif //BAKERY_CONFIGURATION_ASSOCIATION_H
