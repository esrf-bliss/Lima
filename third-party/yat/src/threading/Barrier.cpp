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
#include <yat/threading/Barrier.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/Barrier.i>
#endif // YAT_INLINE_IMPL


namespace yat {

// ----------------------------------------------------------------------------
// Barrier::Barrier
// ----------------------------------------------------------------------------
Barrier::Barrier (size_t _count)
 : m_thread_count (_count),
   m_condition (m_mutex),
   m_waiters_count (0)
{
  YAT_TRACE("Barrier::Barrier");

  YAT_LOG("Barrier::Barrier:: " << m_thread_count << " threads involved");
}

// ----------------------------------------------------------------------------
// Barrier::~Barrier
// ----------------------------------------------------------------------------
Barrier::~Barrier ()
{
  YAT_TRACE("Barrier::Barrier");
}

// ----------------------------------------------------------------------------
// Barrier::wait
// ----------------------------------------------------------------------------
void Barrier::wait ()
{
  //- enter critical section
  MutexLock guard(this->m_mutex);

  //- increment waiters count
  this->m_waiters_count++;

  YAT_LOG("Barrier::wait::thread " << DUMP_THREAD_UID << "::about to wait on Barrier");

  //- are all expected threads waiting on the barrier?
  if (this->m_waiters_count == m_thread_count)
  {
    YAT_LOG("Barrier::wait::all expected waiters present. Reset/notify Barrier...");
    //- reset the barrier
    this->m_waiters_count = 0;
    //- notify all waiters
    this->m_condition.broadcast();
    //- done: return 
    return;
  }

  //- make the calling thread wait
  this->m_condition.wait();

  YAT_LOG("Barrier::wait::thread " << DUMP_THREAD_UID << "::woken up");
}

} // namespace yat
