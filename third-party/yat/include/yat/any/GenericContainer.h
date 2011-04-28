//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2010  The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sune (ALBA) for the yat::Signal class 
//
// The YAT library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details 
//
// Contact:
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author N.Leclercq, J.Malik - Synchrotron SOLEIL
 */

#ifndef _YAT_GENERIC_CONTAINER_H_
#define _YAT_GENERIC_CONTAINER_H_

#include <typeinfo>
#include <yat/CommonHeader.h> 

namespace yat 
{

// ============================================================================
//  class: Container
// ============================================================================
class YAT_DECL Container
{
public:
  Container ()
  {
    //- noop
  };

  virtual ~Container () 
  {
    //- noop
  };
};

// ============================================================================
//  template class: GenericContainer - class T must have a copy ctor
// ============================================================================
template <typename T> 
class GenericContainer : public Container
{
public:

  //- default ctor
  GenericContainer () 
    : ptr_(0), own_(false), allocated_memory_(false)
  {
    //- noop
  }

  //- ctor - no copy
  //- just point to <_data> - <_ownership> will tell us...
  GenericContainer (T* _data, bool _ownership = true) 
    : ptr_(0), own_(_ownership), allocated_memory_(false)
  {
    *this = _data;
  }

  //- ctor - copy - ownership set to true
  GenericContainer (const T& _data) 
    : ptr_(0), own_(false), allocated_memory_(false)
  {
    *this = _data;
  }

  //- copy ctor  - copy - ownership set to true
  GenericContainer (const GenericContainer<T>& _src) 
    : ptr_(0), own_(false), allocated_memory_(false)
  {
    *this = _src;
  }

  //- dtor - delete dat according to ownership flag
  virtual ~GenericContainer ()
  {
    if (own_ && allocated_memory_)
      delete ptr_;
  }

  //- changes content - makes a copy and sets ownership to true
  const GenericContainer& operator= (const GenericContainer<T>& _src)
  {  
    if (&_src == this)
      return *this;
    if (! ptr_ || ! own_ || ! allocated_memory_)
    {
      try
      {
        ptr_ = new (std::nothrow) T(_src.content());
        if (! ptr_)
          throw std::bad_alloc();
        allocated_memory_ = true;
      }
      catch (const std::bad_alloc&)
      {
        ptr_ = 0;
        own_ = false;
        THROW_YAT_ERROR("OUT_OF_MEMORY",
                        "memory allocation failed",
                        "GenericContainer:operator=");
      }
    }
    else 
    {
      *ptr_ = _src.content();
    }
    own_ = true;
    return *this;
  }

  //- changes content - makes a copy and sets ownership to true
  const GenericContainer& operator= (T& _src)
  {
    return this->operator=(static_cast<const T&>(_src));
  }
    
  //- changes content - makes a copy and sets ownership to true
  const GenericContainer& operator= (const T& _src)
  {
    if (! ptr_ || ! own_ || ! allocated_memory_)
    {
      try
      {
        ptr_ = new (std::nothrow) T(_src);
        if (! ptr_)
          throw std::bad_alloc();
        allocated_memory_ = true;
      }
      catch (const std::bad_alloc&)
      {
        ptr_ = 0;
        own_ = false;
        THROW_YAT_ERROR("OUT_OF_MEMORY",
                        "memory allocation failed",
                        "GenericContainer:operator=");
      }
    }
    else 
    {
      *ptr_ = _src;
    }
    own_ = true;
    return *this;
  }

  //- changes content but does not change ownership
  const GenericContainer& operator= (T* _data)
  { 
    if (_data == ptr_)
      return *this;
    if (own_)
      delete ptr_;
    ptr_ = _data;
    allocated_memory_ = false;
    return *this;
  }

  //- changes content and set ownership
  void set_content (T* _data, bool _ownership = true)
  {
    *this = _data;
    own_ = _ownership;
  }

  //- changes content (makes a copy)
  void set_content (const T& _data)
  {
    *this = _data;
    own_ = true;
  }

  //- changes content (makes a copy)
  void set_content (T& _data)
  {
    *this = _data;
    own_ = true;
  }
  
  //- returns content 
  //- does NOT transfer data ownership to the caller
  T * operator-> ()
  {
    return ptr_;
  }
	
  //- returns content 
  //- optionally transfers data ownership to the caller
  T * get_content (bool transfer_ownership)
  {
    if (transfer_ownership)
    {
      own_ = false;
      allocated_memory_ = false;
    }
    return ptr_;
  }

  //- returns content
  T & get_content ()
  {
    DEBUG_ASSERT(ptr_ != 0);
    return *ptr_;
  }

  //- does this container have ownership of the underlying data?
  bool get_ownership () const
  {
    return own_;
  }
  
  //- changes data ownership (dangerous isn't)
  void set_ownership (bool own)
  {
    own_ = own;
  }

private:
  //- actual container content
  T * ptr_;
  //- do we have data ownership?
  bool own_;
  //- did we allocated memory pointed by <ptr_>?
  bool allocated_memory_;
};

} // namespace yat

#endif



