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

#ifndef _POSIX_THREADING_IMPL_
#define _POSIX_THREADING_IMPL_

// ----------------------------------------------------------------------------
// YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT
// ----------------------------------------------------------------------------
#define YAT_MUTEX_IMPLEMENTATION \
  pthread_mutex_t m_posix_mux; \
  friend class Condition;

// ----------------------------------------------------------------------------
// YAT CONDITION - YAT CONDITION - YAT CONDITION - YAT CONDITION - YAT CONDITI
// ----------------------------------------------------------------------------
#define YAT_CONDITION_IMPLEMENTATION \
  pthread_cond_t m_posix_cond;

// ----------------------------------------------------------------------------
// YAT SEMAPHORE - YAT SEMAPHORE - YAT SEMAPHORE - YAT SEMAPHORE - YAT SEMAPHO
// ----------------------------------------------------------------------------
#define YAT_SEMAPHORE_IMPLEMENTATION \
  Mutex m_mux; \
  Condition m_cond; \
  int m_value;

// ----------------------------------------------------------------------------
// YAT THREAD - YAT THREAD - YAT THREAD - YAT THREAD - YAT THREAD - YAT THREAD
// ----------------------------------------------------------------------------
//- YAT common thread entry point (non-OO OS interface to YAT interface)
#define YAT_THREAD_COMMON_ENTRY_POINT \
  void * yat_thread_common_entry_point (void *)

extern "C" YAT_THREAD_COMMON_ENTRY_POINT;

#define YAT_THREAD_IMPLEMENTATION \
  pthread_t m_posix_thread; \
  void spawn (); \
  static int yat_to_posix_priority (Priority); \
  friend YAT_THREAD_COMMON_ENTRY_POINT;

#endif //- _POSIX_THREADING_IMPL_
