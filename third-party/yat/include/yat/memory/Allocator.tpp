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

#ifndef _ALLOCATOR_TPP_
#define _ALLOCATOR_TPP_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include  <yat/memory/Allocator.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/memory/Allocator.i>
#endif // YAT_INLINE_IMPL

namespace yat 
{

// ============================================================================
// NewAllocator::NewAllocator
// ============================================================================
template <typename T>
NewAllocator<T>::NewAllocator ()
{
  //- noop
}

// ============================================================================
// NewAllocator::~NewAllocator
// ============================================================================
template <typename T>
NewAllocator<T>::~NewAllocator ()
{
  //- noop
}

#define USE_NEW_OPERATOR_IN_ALLOCATOR

#if ! defined(USE_NEW_OPERATOR_IN_ALLOCATOR)
// ============================================================================
// VERY NICE TRICK STOLEN FROM THE ACE LIB FOR OBJS MEMORY SPACE ALLOCATION 
// TAKING INTO ACCOUNT THAT MEMORY IS ALLOCATED USING <char> WHILE OBJ STORAGE
// NEEDS ALIGNMENT...
// ============================================================================
union yat_max_align_info
{
  int (*i)();
  void* p;
  long l;
  double d;
};
// ============================================================================
// PART OF THE ACE LIB MEMORY ALIGNMENT TRICK
// ============================================================================
#define ROUNDUP(X, Y) ((X) + ((Y) - 1) & ~((Y) - 1))

// ============================================================================
// NewAllocator::malloc
// ============================================================================
template <typename T>
T * NewAllocator<T>::malloc ()
{
  YAT_TRACE("NewAllocator<T>::malloc");

  //- use the generic byte type (i.e. char) to allocate space
  //- in this case we have to take care of alignment. the space
  //- required is >= sizeof (T). using the ACE lib trick, it gives...
  
  size_t chunk_size = sizeof (T);
  chunk_size = ROUNDUP(chunk_size, sizeof(yat_max_align_info));
  return (T *) new char[chunk_size];
}

#else  //- ! defined(USE_NEW_OPERATOR_IN_ALLOCATOR)

// ============================================================================
// NewAllocator::malloc
// ============================================================================
template <typename T>
T * NewAllocator<T>::malloc ()
{
  YAT_TRACE("NewAllocator<T>::malloc");

  //- use default new "array" operator - infinite loop otherwise!
  //- that's the C++ trick os this class guys! ACE impl. made me understand!
  //- the compiler will take care of alignment for us.
   
  return new T[1];
}

#endif  //- ! defined(USE_NEW_OPERATOR_IN_ALLOCATOR)

// ============================================================================
// NewAllocator::free
// ============================================================================
template <typename T>
void NewAllocator<T>::free (T * p)
{
  YAT_TRACE("NewAllocator<T>::free");

  //- used the generic byte type (i.e. char) to allocate space so...
  delete[] (char *)p;

}

// ============================================================================
// CachedAllocator::CachedAllocator
// ============================================================================
template <typename T, typename L> 
CachedAllocator<T,L>::CachedAllocator (size_t nb_preallocated_objs)
{
#if defined (YAT_DEBUG)
  YAT_LOG("CachedAllocator<T,L>::ctor::preallocating " 
          << nb_preallocated_objs
          << " objs");
#endif

  //- prealloc objects
  this->m_cache.clear();
  
  //- prealloc objects
  for (size_t i = 0; i < nb_preallocated_objs; i++)
  {
    T * t = NewAllocator::malloc(); 
    m_cache.push_back(t);
  }
}

// ============================================================================
// CachedAllocator::~CachedAllocator
// ============================================================================
template <typename T, typename L> 
CachedAllocator<T,L>::~CachedAllocator () 
{
#if defined (YAT_DEBUG)
  YAT_LOG("CachedAllocator<T,L>::dtor::"
          << this->m_cache.size() 
          << " objs in cache");
#endif
    
  //- release objects
  Cache::iterator it = this->m_cache.begin();
  for (; it != this->m_cache.end(); ++it)
    NewAllocator::free(*it);
}
  
// ============================================================================
// CachedAllocator::malloc
// ============================================================================
template <typename T, typename L> 
T * CachedAllocator<T,L>::malloc ()
{ 
  YAT_TRACE("CachedAllocator<T,L>::malloc");
  
  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);
  
  //- do we have something in the cache?
  if (! m_cache.empty())
  {
#if defined (YAT_DEBUG)
    YAT_LOG("CachedAllocator<T,L>::malloc::returning chunk from cache [" 
            << this->m_cache.size() 
            << "]");
#endif
    //- get ref. to the first available chunk of memory
    T * p = m_cache.front();
    //- remove chunk for list of available 
    m_cache.pop_front();
    //- return storage to caller
    return p;
  }
  
#if defined (YAT_DEBUG)
  YAT_LOG("CachedAllocator<T,L>::malloc::returning chunk from heap");
#endif

  //- allocate a chunk of memory
  return NewAllocator<T>::malloc();
}

// ============================================================================
// CachedAllocator::free
// ============================================================================
template <typename T, typename L> 
void CachedAllocator<T,L>::free (T * p)
{   
  YAT_TRACE("CachedAllocator<T,L>::free");
  
  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);
  
  //- push chunk back into the cache
  this->m_cache.push_back(p);
  
#if defined (YAT_DEBUG)
  YAT_LOG("CachedAllocator<T,L>::malloc::pushed chunk back into the cache [" 
          << this->m_cache.size() 
          << "]");
#endif
}
  
} // namespace 

#endif // _ALLOCATOR_TPP_

