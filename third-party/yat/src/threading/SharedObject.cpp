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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/SharedObject.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/SharedObject.i>
#endif

namespace yat
{

// ============================================================================
// SharedObject::SharedObject
// ============================================================================
SharedObject::SharedObject () 
 : reference_count_ (1)
{
  YAT_TRACE("SharedObject::SharedObject");
}

// ============================================================================
// SharedObject::~SharedObject
// ============================================================================
SharedObject::~SharedObject ()
{
  YAT_TRACE("SharedObject::~SharedObject");

  DEBUG_ASSERT(this->reference_count_ == 0);
}

// ============================================================================
// SharedObject::duplicate
// ============================================================================
SharedObject * SharedObject::duplicate ()
{
  YAT_TRACE("SharedObject::duplicate");

  MutexLock guard(this->lock_);

  this->reference_count_++;

  return this;
}

// ============================================================================
// SharedObject::release
// ============================================================================
int SharedObject::release (bool _commit_suicide)
{
  YAT_TRACE("SharedObject::release");

  SharedObject * more_ref = this->release_i ();

  if (! more_ref && _commit_suicide)
    delete this;

  return more_ref ? 1 : 0;
}

// ============================================================================
// SharedObject::release_i
// ============================================================================
SharedObject *SharedObject::release_i ()
{
  MutexLock guard (this->lock_);

  DEBUG_ASSERT(this->reference_count_ > 0);

  this->reference_count_--;

  return (this->reference_count_ == 0) ? 0 : this;
}

} //- namespace
