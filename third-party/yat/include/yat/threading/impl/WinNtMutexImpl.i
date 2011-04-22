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

namespace yat {

// ****************************************************************************
// YAT NULL MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// NullMutex::lock
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::lock ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::acquire
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::acquire ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::unlock
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::unlock ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::release
// ----------------------------------------------------------------------------
YAT_INLINE void NullMutex::release ()
{
 //- noop
}

// ----------------------------------------------------------------------------
// NullMutex::try_lock
// ----------------------------------------------------------------------------
YAT_INLINE MutexState NullMutex::try_lock ()
{
  return yat::MUTEX_LOCKED;
}

// ----------------------------------------------------------------------------
// NullMutex::try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState NullMutex::try_acquire ()
{
  return yat::MUTEX_LOCKED;
}

// ****************************************************************************
// YAT MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Mutex::lock
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::lock ()
{
  //-TODO: the following may fail
  ::WaitForSingleObject(this->m_nt_mux, INFINITE);
}

// ----------------------------------------------------------------------------
// Mutex::acquire
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::acquire ()
{
  this->lock();
}
// ----------------------------------------------------------------------------
// Mutex::try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::try_acquire ()
{
  return this->try_lock();
}

// ----------------------------------------------------------------------------
// Mutex::try_lock
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::try_lock ()
{
  switch (::WaitForSingleObject(this->m_nt_mux, 0))
  {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
      return MUTEX_LOCKED;
    default:
      break;
  }
  return MUTEX_BUSY;
}

// ----------------------------------------------------------------------------
// Mutex::timed_try_lock
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::timed_try_lock (unsigned long tmo_msecs)
{
  switch (::WaitForSingleObject(this->m_nt_mux, tmo_msecs))
  {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
      return MUTEX_LOCKED;
    default:
      break;
  }
  return MUTEX_BUSY;
}

// ----------------------------------------------------------------------------
// Mutex::timed_try_acquire
// ----------------------------------------------------------------------------
YAT_INLINE MutexState Mutex::timed_try_acquire (unsigned long tmo_msecs)
{
  return this->timed_try_lock(tmo_msecs);
}

// ----------------------------------------------------------------------------
// Mutex::unlock
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::unlock ()
{
  //-TODO: the following may fail
  ::ReleaseMutex(this->m_nt_mux); 
}

// ----------------------------------------------------------------------------
// Mutex::acquire
// ----------------------------------------------------------------------------
YAT_INLINE void Mutex::release ()
{
  this->unlock();
}

} // namespace yat
