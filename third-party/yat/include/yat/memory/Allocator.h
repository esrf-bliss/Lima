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

#ifndef _YAT_ALLOCATOR_H_
#define _YAT_ALLOCATOR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <list>
#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
//! The NewAllocator class 
// ============================================================================
//!  
// ============================================================================
template <typename T> class NewAllocator
{
public:
  //- Ctor
  NewAllocator ();

  //- Dtor
  virtual ~NewAllocator ();
  
  //- memory allocation - can't allocate more than sizeof(T)
  virtual T * malloc ();

  //- memory release - <p> must have beeb allocated by <this> CachedAllocator
  virtual void free (T * p);
};

/*
// ============================================================================
//! The CachedAllocator class  
// ============================================================================
//! Implements an unbounded memory pool of T with selectable locking strategy 
//! ok but for "cachable" classes only!
// ============================================================================
#define CACHABLE_CLASS_DECL(CLASS_NAME, LOCKING_STRATEGY)
  typedef CachedAllocator<CLASS_NAME, LOCKING_STRATEGY> CLASS_NAME#Cache; \
  static CLASS_NAME#::Cache m_cache; \
  void * operator new (size_t); \
  void operator delete (void *); \
  static void pre_alloc (size_t _nobjs); \
  static void release_pre_alloc ();
#endif
  //-TODO: not usable for the moment - to be finished...
*/

// ============================================================================
//! The CachedAllocator class  
// ============================================================================
//! Implements an unbounded memory pool of T with selectable locking strategy 
//! ok... for "cachable" classes only!
// ============================================================================
template <typename T, typename L = yat::NullMutex> 
class CachedAllocator : public NewAllocator<T>
{
  //- memory pool (or cache) implementation
  typedef std::list<T*> Cache; 

public: 
  //- Ctor - preallocates <nb_preallocated_objs> 
  CachedAllocator (size_t nb_preallocated_objs = 0);

  //- Dtor
  virtual ~CachedAllocator();
  
  //- memory allocation - can't allocate more than sizeof(T)
  virtual T * malloc ();

  //- memory release - <p> must have beeb allocated by <this> CachedAllocator
  virtual void free (T * p);

protected:
  //- locking (i.e. thread safety) strategy
  L m_lock;

  //- the memory cache (i.e. memory pool) 
  Cache m_cache;
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/memory/Allocator.i>
#endif // YAT_INLINE_IMPL

#include <yat/memory/Allocator.tpp>

#endif // _YAT_ALLOCATOR_H_


