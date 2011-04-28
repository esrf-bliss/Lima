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

#ifndef _YAT_MUTEX_H_
#define _YAT_MUTEX_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/time/Timer.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Implementation.h>

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_MUTEX_IMPLEMENTATION)
# error "implementation header file incomplete [no mutex implementation]"
#endif

namespace yat {

// ----------------------------------------------------------------------------
//! <YAT_MUTEX>::try_lock may return one of the following MutexState
// ----------------------------------------------------------------------------
typedef enum
{
  MUTEX_LOCKED,
  MUTEX_BUSY,
} MutexState;

// ----------------------------------------------------------------------------
//! The YAT NullMutex class
// ----------------------------------------------------------------------------
class YAT_DECL NullMutex
{
  //! This is the yat NullMutex class.
  //!
  //! Provides a "do nothing" Mutex impl. May be used as template argument
  //! in order to control the template instanciation and avoiding locking
  //! overhead where thread safety is not required.
  //!
  //! template <typename LOCK> class OptionalThreadSafetyImpl
  //! {
  //! public:
  //!   inline void do_something ()
  //!   {
  //!      yat::AutoMutex<LOCK>(this->m_mutex);
  //!      ...
  //!   }
  //! private:
  //    LOCK m_mutex;
  //! }
  //!
  //! OptionalThreadSafetyImpl<yat::Mutex> will be thread safe while...
  //! OptionalThreadSafetyImpl<yat::NullMutex> will not be!
  //!
  //! This class is not supposed to be derived.

public:
  //! Constructor.
  NullMutex ();

  //! Destructor.
  ~NullMutex ();

  //! Locks (i.e. acquires) the mutex.
  void lock ();

  //! Locks (i.e. acquires) the mutex.
  void acquire ();

  //! Locks (i.e. acquires) the mutex. Always returns MUTEX_LOCKED.
  MutexState try_lock ();

  //! Locks (i.e. acquires) the mutex. Always returns MUTEX_LOCKED.
  MutexState try_acquire ();

  //! Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! \param tmo_msecs The "dummy" timeout in milliseconds
  //! Always returns MUTEX_LOCKED.
  MutexState timed_try_lock (unsigned long tmo_msecs);

  //! Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! \param tmo_msecs The "dummy" timeout in milliseconds
  //! Always returns MUTEX_LOCKED.
  MutexState timed_try_acquire (unsigned long tmo_msecs);

  //! Unlocks (i.e. releases) the mutex.
  void unlock ();

  //! Unlocks (i.e. releases) the mutex.
  void release ();

private:
  //! Not implemented private member
  NullMutex (const NullMutex&);
  //! Not implemented private member
  NullMutex & operator= (const NullMutex&);
};

// ----------------------------------------------------------------------------
//! The YAT Mutex class
// ----------------------------------------------------------------------------
class YAT_DECL Mutex
{
  //! This is the yat Mutex implementation.
  //!
  //! This class is not supposed to be derived (no virtual destructor).

public:
  //! Constructor.
  Mutex ();

  //! Destructor.
  ~Mutex ();

  //! Locks (i.e. acquires) the mutex.
  void lock ();

  //! Locks (i.e. acquires) the mutex.
  void acquire ();

  //! Tries to lock (i.e. acquire) the mutex.
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if it is already owned by another thread.
  MutexState try_lock ();
  
  //! Tries to lock (i.e. acquire) the the mutex.
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if it is already owned by another thread.
  MutexState try_acquire ();

  //! Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! \param tmo_msecs The timeout in milliseconds
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if still owned by another thread after tmo expiration.
  MutexState timed_try_lock (unsigned long tmo_msecs);

  //! Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! \param tmo_msecs The timeout in milliseconds
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if still owned by another thread after tmo expiration.
  MutexState timed_try_acquire (unsigned long tmo_msecs);

  //! Unlocks (i.e. releases) the mutex.
  void unlock ();
  
  //! Unlocks (i.e. releases) the mutex.
  void release ();

private:
  //! Not implemented private member
  Mutex (const Mutex&);
  //! Not implemented private member
  Mutex & operator= (const Mutex&);

  //- platform specific implementation
  YAT_MUTEX_IMPLEMENTATION;
};

// ----------------------------------------------------------------------------
//! The YAT "auto mutex" class
// ----------------------------------------------------------------------------
template <typename LOCK_TYPE = yat::Mutex> class AutoMutex
{
  //! An "auto mutex" providing an auto lock/unlock mechanism.
  //!
  //! The AutoMutex is ideal in context where some exceptions may be thrown.
  //! Whatever is the exit path of your code, the <AutoMutex> will garantee
  //! that the associated <Mutex> is properly unlock.
  //!
  //! This class is template since it may be used in contexts in which the
  //! thread safety is optionnal (see yat::NullMutex for an example).
  //!
  //! AutoMutex provides an efficient and safe alternative to:
  //!
  //! { //- enter critical section
  //!   my_mutex.lock();
  //!   ...your critical section code goes here (may throw an exception)...
  //!   my_mutex.unlock();
  //! } //- leave critical section
  //!
  //! In such a context, you can use a instance AutoMutex as follows:
  //!
  //! { //- enter critical section
  //!   yat::AutoMutex<> guard(my_mutex);
  //!   ...your critical section code goes here (may throw an exception)...
  //! } //- leave critical section
  //!
  //! This has the advantage that my_mutex.unlock() will be called automatically
  //! even if an exception is thrown. Since the AutoMutex is created on the stack
  //! its destructor will be called whatever is the exit path of critical section.
  //!
  //! Note that AutoMutex can be used with any "LOCK_TYPE" which interface contains
  //! both a lock() and a unlock() method. The yat::SharedObject class of
  //! such a compatible "LOCK_TYPE". 
  //!
public:
  //! Constructor (locks the associated Mutex)
  AutoMutex (LOCK_TYPE & _lock)
    : m_lock (_lock)
  {
    m_lock.lock();
  }

  //! Destructor (unlocks the associated Mutex)
  ~AutoMutex ()
  {
    m_lock.unlock();
  }

private:
  //! The associated Mutex
  LOCK_TYPE & m_lock;

  //! Not implemented private member
  AutoMutex (const AutoMutex&);
  //! Not implemented private member
  AutoMutex & operator= (const AutoMutex&);
};

// ----------------------------------------------------------------------------
//! MutexLock: an AutoMutex specialisation (for backforward compatibility)
// ----------------------------------------------------------------------------
typedef AutoMutex<Mutex> MutexLock;

// ----------------------------------------------------------------------------
//! A global mutex used for thread safe logging
// ----------------------------------------------------------------------------
#if defined (YAT_DEBUG)
  extern Mutex g_logging_mux;
#endif

} // namespace yat 

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtMutexImpl.i>
# else
#  include <yat/threading/impl/PosixMutexImpl.i>
# endif
#endif

#endif //- _YAT_MUTEX_H_
