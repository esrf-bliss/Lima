//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2009  The Tango Community
//
// Part of the code comes from the ACE Framework
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// Ramon Sunes (ALBA) 
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
 * \authors N.Leclercq, J.Malik - Synchrotron SOLEIL
 */

#ifndef _WIN_NT_THREADING_IMPL_
#define _WIN_NT_THREADING_IMPL_

// ----------------------------------------------------------------------------
// SOME PLATEFORM SPECIFIC CONSTs
// ----------------------------------------------------------------------------
#define WIN_NT_NULL NULL

// ----------------------------------------------------------------------------
// YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT MUTEX - YAT
// ----------------------------------------------------------------------------
#define YAT_MUTEX_IMPLEMENTATION \
  HANDLE m_nt_mux; \
  friend class Condition;

// ----------------------------------------------------------------------------
// YAT SEMAPHORE - YAT SEMAPHORE - YAT SEMAPHORE - YAT SEMAPHORE - YAT SEMAPHO
// ----------------------------------------------------------------------------
#define YAT_SEMAPHORE_IMPLEMENTATION \
  HANDLE m_nt_sem;

// ----------------------------------------------------------------------------
// YAT CONDITION - YAT CONDITION - YAT CONDITION - YAT CONDITION - YAT CONDITI
// ----------------------------------------------------------------------------
#define YAT_CONDITION_IMPLEMENTATION \
  int m_waiters_count; \
  CRITICAL_SECTION m_waiters_count_lock; \
  HANDLE m_nt_sem; \
  HANDLE m_waiters_done; \
  bool m_was_broadcast;

// ----------------------------------------------------------------------------
// YAT THREAD - YAT THREAD - YAT THREAD - YAT THREAD - YAT THREAD - YAT THREAD
// ----------------------------------------------------------------------------
//- YAT common thread entry point (non-OO OS interface to YAT interface)
#define YAT_THREAD_COMMON_ENTRY_POINT \
  unsigned __stdcall yat_thread_common_entry_point (void *)

extern "C" YAT_THREAD_COMMON_ENTRY_POINT;

#define YAT_THREAD_IMPLEMENTATION \
  HANDLE m_nt_thread_handle; \
  void spawn (void); \
  static int yat_to_nt_priority (Priority); \
  friend YAT_THREAD_COMMON_ENTRY_POINT;

#endif  // _WIN_NT_THREADING_IMPL_
