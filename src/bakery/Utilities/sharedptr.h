/*
 * Copyright 2004 Murray Cumming
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

#ifndef BAKERY_UTILITIES_SHAREDPTR_H
#define BAKERY_UTILITIES_SHAREDPTR_H

#include <iostream> //Just for debugging.

namespace Bakery
{

/**A shared reference-counting smart-pointer.
 *
 */
template< typename T_obj >
class sharedptr
{
public:
  typedef size_t size_type;

  ///Allocate a new instance.
  sharedptr();

  ///Take ownership
  explicit sharedptr(T_obj* pobj);

  ///Share ownership
  sharedptr(const sharedptr& src);

  ///Share ownership
  sharedptr& operator=(const sharedptr& src);

  virtual ~sharedptr();

  /** Dereferencing.
   */
  inline T_obj& operator*();

  /** Dereferencing.
   */
  inline const T_obj& operator*() const;

  /** Dereferencing.
   *
   * Use the methods of the underlying instance like so:
   * @code
   * refptr->memberfun()
   * @endcode
   */
  inline T_obj* operator->() const;

  /** Test whether the RefPtr<> points to any underlying instance.
   *
   * Mimics usage of ordinary pointers:
   * @code
   *   if (ptr)
   *     do_something();
   * @endcode
   */
  inline operator bool() const;

  ///Get the underlying instance:
  inline T_obj* obj();

  ///Get the underlying instance:
  inline const T_obj* obj() const;


protected:
  inline void ref();
  inline void unref();

  size_type* m_pRefCount; //Shared between instances, by copying.
  T_obj* m_pobj; //The underlying instance.
};

template< typename T_obj>
sharedptr<T_obj>::sharedptr()
: m_pRefCount(0), m_pobj(0)
{

}

template< typename T_obj>
sharedptr<T_obj>::sharedptr(T_obj* pobj)
: m_pRefCount(0), m_pobj(pobj)
{
    //Start refcounting:
    ref();
}

template< typename T_obj>
sharedptr<T_obj>::sharedptr(const sharedptr<T_obj>& src)
: m_pRefCount(src.m_pRefCount), m_pobj(src.m_pobj)
{
    ref();
}

template< typename T_obj>
sharedptr<T_obj>& sharedptr<T_obj>::operator=(const sharedptr<T_obj>& src)
{
  //std::cout << "sharedptr& operator=(const sharedptr& src)" << std::endl;
  if(&src != this)
  {
    //Unref any existing stuff.
    //operator= can never run before a constructor, so these values will be initialized already.
    if(m_pobj) //The if() might not be required.
    {
      unref(); //Could cause a deallocation.
    }

    //Copy:
    m_pobj = src.m_pobj;

    m_pRefCount = src.m_pRefCount;
    ref();
  }

  return *this;
}

template< typename T_obj>
sharedptr<T_obj>::~sharedptr()
{
   unref();
}

/*
template< typename T_obj>
void sharedptr<T_obj>::clear_without_deallocating()
{
  m_pobj = 0;
}
*/

template< typename T_obj>
inline
T_obj* sharedptr<T_obj>::obj()
{
  return m_pobj;
}

template< typename T_obj>
inline
const T_obj* sharedptr<T_obj>::obj() const
{
  return m_pobj;
}

template< typename T_obj>
inline
T_obj& sharedptr<T_obj>::operator*()
{
  return *m_pobj;
}

template< typename T_obj>
inline
const T_obj& sharedptr<T_obj>::operator*() const
{
  return *m_pobj;
}

template< typename T_obj>
inline
T_obj* sharedptr<T_obj>::operator->() const
{
  return m_pobj;
}

template <class T_obj>
inline
sharedptr<T_obj>::operator bool() const
{
  return (m_pobj != 0);
}


template <class T_obj>
inline
void sharedptr<T_obj>::ref()
{
  if(m_pobj) //Don't waste time on invalid instances. These would be very rare anyway, and intentionally created with (0,0) construction.
  {
    if(m_pRefCount == 0)
    {
      //std::cout << "sharedptr::ref(): first ref" << std::endl;
      //First ref, so allocate the shared count:
      m_pRefCount = new size_type();
      *m_pRefCount = 1;
    }
    else
    {
      //std::cout << "sharedptr::ref(): starting at" << *m_pRefCount << std::endl;
      (*m_pRefCount)++;
    }
  }
}

template <class T_obj>
inline
void sharedptr<T_obj>::unref()
{
  if(m_pRefCount)
  {
    //std::cout << "sharedptr::unref(): starting at " << *m_pRefCount << std::endl;

    if( (*m_pRefCount) > 0 )
       (*m_pRefCount)--;

    //Unalloc if this is the last user of the obj:
    if(*m_pRefCount == 0)
    {
      if(m_pobj)
       {
         //try
         //{
           //std::cout << "sharedptr::unref(): deallocating " << *m_pRefCount << std::endl;
           delete m_pobj;
           m_pobj = 0;
         //}
         /*
         catch(ex_base&)
         {
          //std::cout << "sharedptr::unref(): exception thrown during deallocation." << std::endl;
           //Ignore it. Can't throw an expection up to the destructor.
         }
         */

         m_pobj = 0;
      }

       //Clear ref count:
       delete m_pRefCount;
       m_pRefCount = 0;
    }
  }
  else
  {
    //std::cout << "sharedptr::unref(): ref not setup." << std::endl;
  }

}


} //namespace

#endif //BAKERY_UTILITIES_SHAREDPTR_H

