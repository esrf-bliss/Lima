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
 * \author N.Leclercq, J.Malik - (boost inspired impl) - Synchrotron SOLEIL
 */

#ifndef _YAT_SHARED_PTR_H_
#define _YAT_SHARED_PTR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>

namespace yat
{

// ============================================================================
//! The SharedPtr class 
// ============================================================================
//! Template arg class <T> must inherit from yat::SharedObject
// ============================================================================
template <typename T> class SharedPtr
{
public:

  /**
   * Default constructor. 
   */
  SharedPtr() : so_(0)
  {
    //- noop
  }

  /**
   * Constructor with initialization. 
   * Takes ownership of the specified yat::SharedObject. 
   * <T> must inherit from yat::SharedObject. 
   */
  template <typename Y> explicit SharedPtr( Y * so ) : so_(so)
  {
    //- noop
  }
  
  /**
   * Destructor.
   */
  ~SharedPtr()
  {
    this->reset();
  }

  /**
   * Assignment operator.
   * Releases currently pointed SharedObject, then points to the specified one.
   */
  SharedPtr& operator= (SharedPtr& r) 
  {
    T * new_so = r ? r.so_->duplicate() : 0;
    if (so_)
      so_->release();
    so_ = new_so;
    return *this;
  }

  /**
   * Releases the currently pointed yat::SharedObject then takes ownership of the specified one
   */
  void reset ( T* so = 0 )
  {
    if (so_)
      so_->release();
    so_ = so;
  }

  /**
   * Underlying yat::SharedObject accessor
   */
  T* get()
  {
    return so_;
  }
  
  /**
   * Underlying yat::SharedObject accessor
   */
  T* operator-> ()
  {
    return so_;
  }
  
  /**
   * Underlying yat::SharedObject accessor
   */
  T& operator* ()
  {
    return *so_;
  }

  /**
   * Implicit conversion to bool: typedef
   */
  typedef T* SharedPtr::*unspecified_bool_type;
  
  /**
   * Implicit conversion to bool: operator
   */
  operator unspecified_bool_type() const
  {
    return so_ ? &SharedPtr::so_ : 0;
  }

private:
  T* so_;
};

} //- namespace

#endif //- _YAT_SHARED_PTR_H_
