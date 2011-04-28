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

// ****************************************************************************
// YAT DUMMY_MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// DummyMutex::lock
// ----------------------------------------------------------------------------
INLINE_IMPL void DummyMutex::lock ()
{
 //- noop
}
// ----------------------------------------------------------------------------
// DummyMutex::acquire
// ----------------------------------------------------------------------------
INLINE_IMPL void DummyMutex::acquire ()
{
 //- noop
}
// ----------------------------------------------------------------------------
// DummyMutex::unlock
// ----------------------------------------------------------------------------
INLINE_IMPL void DummyMutex::unlock ()
{
 //- noop
}
// ----------------------------------------------------------------------------
// DummyMutex::release
// ----------------------------------------------------------------------------
INLINE_IMPL void DummyMutex::release ()
{
 //- noop
}
// ----------------------------------------------------------------------------
// DummyMutex::try_lock
// ----------------------------------------------------------------------------
INLINE_IMPL MutexState DummyMutex::try_lock ()
{
  return yat::MUTEX_LOCKED;
}
// ----------------------------------------------------------------------------
// DummyMutex::try_acquire
// ----------------------------------------------------------------------------
INLINE_IMPL MutexState DummyMutex::try_acquire ()
{
  return yat::MUTEX_LOCKED;
}

// ****************************************************************************
// YAT MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Mutex::lock
// ----------------------------------------------------------------------------
INLINE_IMPL void Mutex::lock ()
{
#error no impl
}
// ----------------------------------------------------------------------------
// Mutex::acquire
// ----------------------------------------------------------------------------
INLINE_IMPL void Mutex::acquire ()
{
  this->lock();
}
// ----------------------------------------------------------------------------
// Mutex::try_acquire
// ----------------------------------------------------------------------------
MutexState Mutex::try_acquire ()
{
  return this->try_lock();
}
// ----------------------------------------------------------------------------
// Mutex::unlock
// ----------------------------------------------------------------------------
INLINE_IMPL void Mutex::unlock ()
{
#error no impl
}
// ----------------------------------------------------------------------------
// Mutex::acquire
// ----------------------------------------------------------------------------
INLINE_IMPL void Mutex::release ()
{
  this->unlock();
}

// ****************************************************************************
// YAT THREAD IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Thread::priority
// ----------------------------------------------------------------------------
INLINE_IMPL Priority Thread::priority () const
{
  //- enter critical section
  yat::SmartMutex guard(this->m_lock);

  return this->m_priority;
}
// ----------------------------------------------------------------------------
// Thread::state
// ----------------------------------------------------------------------------
INLINE_IMPL State Thread::state () const
{
  //- enter critical section
  yat::SmartMutex guard(this->m_lock);

  return this->m_state;
}
// ----------------------------------------------------------------------------
// Thread::yield
// ----------------------------------------------------------------------------
INLINE_IMPL void Thread::yield ()
{
#error no impl
}
// ----------------------------------------------------------------------------
// Thread::sleep
// ----------------------------------------------------------------------------
INLINE_IMPL void Thread::sleep (unsigned long _tmo_msecs)
{
  ThreadingUtilities::sleep(0, 1000000 * _tmo_msecs);
}
// ----------------------------------------------------------------------------
// Thread::self
// ----------------------------------------------------------------------------
INLINE_IMPL ThreadUID Thread::self () const
{
  return this->m_uid;
}

} // namespace yat
