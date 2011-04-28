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

#ifndef _YAT_SHARED_OBJECT_H_
#define _YAT_SHARED_OBJECT_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#include <yat/threading/Mutex.h>

namespace yat
{

// ============================================================================
//! A reference counted object abstraction.
// ============================================================================
//!
//! Base class for any reference counted object (i.e. shared) object.
//!
// ============================================================================
class YAT_DECL SharedObject
{
public:

  SharedObject ();
  // Constructor.

  virtual ~SharedObject ();
  // Destructor.

  SharedObject * duplicate ();
  // Return a "shallow" copy. Increment the reference count by 1
  // to avoid deep copies.

  int release (bool commit_suicide = true);
  // Decrease the shared reference count by 1.  If the reference count
  // equals 0 and <commit_suicide> is true, then delete <this> and return 0.
  // Return 1 otherwise. Behavior is undefinedif reference count < 0.

  int reference_count () const;
  // Returns the current reference count.

  void lock ();
  // Locks the underlying Mutex

  void unlock ();
  // Unlocks the underlying Mutex

  Mutex & mutex ();
  // Returns the underlying synch. object

protected:
  // a mutex to protect the data against race conditions
  Mutex lock_;

private:
  // internal release implementation
  SharedObject * release_i ();

  //- reference count for (used to avoid deep copies)
  int reference_count_;

  // = Disallow these operations.
  //--------------------------------------------
  SharedObject & operator= (const SharedObject &);
  SharedObject (const SharedObject &);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/SharedObject.i>
#endif  // YAT_INLINE_IMPL

#endif  // _SHARED_OBJECT_H_
