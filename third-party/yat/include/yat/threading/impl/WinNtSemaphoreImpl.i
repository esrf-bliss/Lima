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

// ----------------------------------------------------------------------------
// Semaphore::wait
// ----------------------------------------------------------------------------
YAT_INLINE void Semaphore::wait (void)
{
  DWORD result = ::WaitForSingleObject(this->m_nt_sem, INFINITE);

  DEBUG_ASSERT(result != WAIT_OBJECT_0);
}

// ----------------------------------------------------------------------------
// Semaphore::timed_wait
// ----------------------------------------------------------------------------
YAT_INLINE bool Semaphore::timed_wait (unsigned long _tmo_msecs)
{
  DWORD result = ::WaitForSingleObject(this->m_nt_sem, _tmo_msecs);

  if (result == WAIT_TIMEOUT)
    return false;

  DEBUG_ASSERT(result == WAIT_OBJECT_0);

  return true;
}

// ----------------------------------------------------------------------------
// Semaphore::try_wait
// ----------------------------------------------------------------------------
YAT_INLINE SemaphoreState Semaphore::try_wait (void)
{
  DWORD result = ::WaitForSingleObject(this->m_nt_sem, 0);

  if (result == WAIT_TIMEOUT)
    return SEMAPHORE_NO_RSC;

  DEBUG_ASSERT(result == WAIT_OBJECT_0);

  return SEMAPHORE_DEC;
}

// ----------------------------------------------------------------------------
// Semaphore::post
// ----------------------------------------------------------------------------
YAT_INLINE void Semaphore::post (void)
{
  BOOL result = ::ReleaseSemaphore(this->m_nt_sem, 1, WIN_NT_NULL);

  DEBUG_ASSERT(result == TRUE);
}

} // namespace yat
