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
namespace yat {

// ----------------------------------------------------------------------------
// Thread::priority
// ----------------------------------------------------------------------------
YAT_INLINE Thread::Priority Thread::priority ()
{
  //- enter critical section
  yat::MutexLock guard(this->m_lock);

  return this->m_priority;
}
// ----------------------------------------------------------------------------
// Thread::state
// ----------------------------------------------------------------------------
YAT_INLINE Thread::State Thread::state ()
{
  //- enter critical section
  yat::MutexLock guard(this->m_lock);

  return this->m_state;
}
// ----------------------------------------------------------------------------
// Thread::state
// ----------------------------------------------------------------------------
YAT_INLINE Thread::State Thread::state_i () const
{
  return this->m_state;
}
// ----------------------------------------------------------------------------
// Thread::yield
// ----------------------------------------------------------------------------
YAT_INLINE void Thread::yield ()
{
#if YAT_HAS_PTHREAD_YIELD == 1
# if (PthreadDraftVersion == 6)
  ::pthread_yield(NULL);
# elif (PthreadDraftVersion < 9)
  ::pthread_yield();
# endif
#else
  ::sched_yield();
#endif
}
// ----------------------------------------------------------------------------
// Thread::sleep
// ----------------------------------------------------------------------------
YAT_INLINE void Thread::sleep (unsigned long _msecs)
{
#define kNSECS_PER_SEC  1000000000
#define kNSECS_PER_MSEC 1000000

  unsigned long secs = 0;
  unsigned long nanosecs = kNSECS_PER_MSEC * _msecs;

	while (nanosecs >= kNSECS_PER_SEC)
	{
		secs += 1;
		nanosecs -= kNSECS_PER_SEC;
	}

  ThreadingUtilities::sleep(secs, nanosecs);

#undef kNSECS_PER_MSEC
#undef kNSECS_PER_SEC
}
// ----------------------------------------------------------------------------
// Thread::self
// ----------------------------------------------------------------------------
YAT_INLINE ThreadUID Thread::self () const
{
  return this->m_uid;
}

} // namespace yat
