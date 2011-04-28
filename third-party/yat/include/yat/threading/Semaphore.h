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

#ifndef _YAT_SEMAPHORE_H_
#define _YAT_SEMAPHORE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Implementation.h>
#if ! defined(WIN32)
# include <yat/threading/Mutex.h>
# include <yat/threading/Condition.h>
#endif

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_SEMAPHORE_IMPLEMENTATION)
# error "implementation header file incomplete [no semaphore implementation]"
#endif

namespace yat {

// ----------------------------------------------------------------------------
//! <YAT_SEMAPHORE>::try_wait may return one of the following SemaphoreState
// ----------------------------------------------------------------------------
typedef enum
{
  //! semaphore is currently "decrementable"
  SEMAPHORE_DEC,
  //! no resource available (semaphore value is 0)
  SEMAPHORE_NO_RSC,
} SemaphoreState;

// ----------------------------------------------------------------------------
//! The YAT Semaphore class
// ----------------------------------------------------------------------------
class YAT_DECL Semaphore
{
  //! This is the yat Semaphore class.
  //!
  //! This class is not supposed to be derived.

public:
  //! Constructor (may throw an Exception)
  Semaphore (unsigned int initial = 1);

  //! Destructor.
  ~Semaphore ();

  //! If semaphore value is > 0 then decrement it and carry on. 
  //! If it's already 0 then block untill the semaphore is either "signaled" 
  //! or "broascasted" (see post, signal and broadcast members below).
  void wait ();

  //! If semaphore value is > 0 then decrements it and returns true. Returns 
  //! "false" in case the specified timeout expired before the semaphore
  //! has been "signaled" or "broascasted" by another thread.
  bool timed_wait (unsigned long tmo_msecs);

  //! If the current semaphore value is > 0, then crements it and returns 
  //! SEMAPHORE_DEC. In case the semaphore has reached its maximum value,
  //! this method does not block and "immediately" returns SEMAPHORE_NO_RSC.
  SemaphoreState try_wait ();

  //! If any threads are blocked in wait(), wake one of them up. 
  //! Otherwise increments the value of the semaphore. 
  void post ();

private:
  //! Not implemented private member
  Semaphore (const Semaphore&);
  //! Not implemented private member
  Semaphore & operator= (const Semaphore&);
  
  //- platform specific implementation
  YAT_SEMAPHORE_IMPLEMENTATION;
};

// ----------------------------------------------------------------------------
//! The YAT "auto semaphore" class
// ----------------------------------------------------------------------------
class YAT_DECL AutoSemaphore
{
  //! An "auto semaphore" providing an auto wait/post mechanism.

public:
  //! Constructor (wait on the associated Semaphore)
  AutoSemaphore (Semaphore & _sem)
    : m_sem (_sem)
  {
    m_sem.wait();
  }

  //! Destructor (post the associated Semaphore)
  ~AutoSemaphore ()
  {
    m_sem.post();
  }

private:
  //! The associated Mutex
  Semaphore & m_sem;

  //! Not implemented private member
  AutoSemaphore (const AutoSemaphore&);
  //! Not implemented private member
  AutoSemaphore & operator= (const AutoSemaphore&);
};

} // namespace yat

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtSemaphoreImpl.i>
# else
#  include <yat/threading/impl/PosixSemaphoreImpl.i>
# endif
#endif

#endif //- _YAT_SEMAPHORE_H_
