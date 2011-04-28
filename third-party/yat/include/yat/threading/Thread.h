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
 
#ifndef _YAT_THREAD_H_
#define _YAT_THREAD_H_

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <yat/threading/Mutex.h>

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_THREAD_IMPLEMENTATION)
# error "implementation header file incomplete [no thread implementation]"
#endif

namespace yat {

// ----------------------------------------------------------------------------
//! The YAT Thread abstract class
// ----------------------------------------------------------------------------
class YAT_DECL Thread
{
  //! This abstract class can't be used as this and must be derived.
  //! Provides both "detached" and "undetached" (i.e. joinable) behaviour.

public:
  //! A dedicated type for thread entry point argument (user specified data).
  typedef void * IOArg;

  //! The possible thread priorities (default is NORMAL).
  //! Be aware that setting the thread priority to HIGH or RT may 
  //! prevent other threads from running (CPU starvation).
  enum Priority
  {
    PRIORITY_LOW,
    PRIORITY_NORMAL,
    PRIORITY_HIGH,
    PRIORITY_RT
  };

  //! The possible thread states
  enum State
  {
    //! Thread object exists but thread hasn't started yet.
    //! In this state, the thread UID (identifier) is undefined.
    STATE_NEW,
    //! Thread is running.
    STATE_RUNNING,
    //! Thread has terminated but storage has not been reclaimed (i.e. waiting to be joined).
    STATE_TERMINATED
  };

  //! Returns the the thread unique indentifier.
  //! In case the thread is not yet running (THREAD_STATE_NEW), self will returns
  //! YAT_INVALID_THREAD_UID (since the thread UID is not defined in this state).
  ThreadUID self () const;

  //! Set the priority of the thread.
  //! In case the thread is running, the priority is immediately applied.
  void priority (Priority p)
    throw (Exception);

  //! Returns the current priority of the thread.
  Thread::Priority priority ();

  //! Returns the current state of the thread.
  //! \remarks Locks the associated Mutex (\c m_lock)
  Thread::State state ();

  //! This pure virtual member _must_ cause the "run" (for detached threads)
  //! or "run_undetached" (for undetached threads) to return. In other words,
  //! exit _must_ make the thread quit its "infinite loop" and return. Its
  //! content in purely application dependent - that's why the actual
  //! implementation is delegated to the derived class.
  virtual void exit () = 0;

  //! Allows another thread to run.
  static void yield ();

  //! Causes the thread to sleep for the given time.
  static void sleep (unsigned long msecs);
  
  //! Causes the thread to be detached.  
  //! In this case the thread executes the run member function.
  void start ()
    throw (Exception);

  //! Causes the thread to be undetached.
  //! In this case the thread executes the run_undetached member function.
  void start_undetached ()
    throw (Exception);

protected:
  //! This constructor is used in a derived class.  The thread will
  //! execute the run() or run_undetached() member functions depending on
  //! whether start() or start_undetached() is called respectively.
  Thread (IOArg a = 0, Priority p = yat::Thread::PRIORITY_NORMAL);

  //! Join causes the calling thread to wait for another's completion,
  //! putting the return value in the variable of type IOArg whose address
  //! is given (unless passed a null pointer). Only undetached threads
  //! may be joined. Storage for the thread will be reclaimed. May throw an 
  //! exception in case the thread is either "not running" or "terminated".
  //! An exception will also be thrown in case the thread is "detached" or
  //! in case the underlying OS "wait for the thread to terminate" call fails.
  void join (Thread::IOArg *)
    throw (Exception);

  //! The Thread destructor cannot be called by user (except via a derived class).
  //! Use exit() instead. This also means a thread object must be allocated with
  //! new - it cannot be statically or automatically allocated. The destructor of
  //! a class that inherits from omni_thread shouldn't be public either (otherwise 
  //! the thread object can be destroyed while the underlying thread is still running).
  virtual ~Thread ();

  //! Default implementation of the run method (detached thread).
  //! Should be overridden in a derived class. Called by start()
  virtual void run (Thread::IOArg)
  {
    //- noop
    DEBUG_ASSERT(true);
  }

  //! Default implementation of the run_undetached method (undetached thread).
  //! Should be overridden in a derived class. Called by start_undetached()
  virtual IOArg run_undetached (Thread::IOArg)
  {
    DEBUG_ASSERT(true);
    return 0;
  }

  //! The following mutex is used to protect any members which can change
  //! after construction (such as m_state, m_priority, ...)
  yat::Mutex m_lock;

   //! Returns the current state of the thread.
  //! \remarks Does not lock the associated Mutex (\c m_lock)
  Thread::State state_i () const;

private:
  //! The current TState of the thread.
  State m_state;

  //! The current TPriority of the thread.
  Priority m_priority;

  //! The thread input argument
  Thread::IOArg  m_iarg;

  //! The thread returned value
  Thread::IOArg  m_oarg;

  //! Detached/undetached flag
  bool m_detached;

  //! The thread identifier
  ThreadUID m_uid;

  //! Not implemented private members
  Thread (const Thread&);
  Thread & operator= (const Thread&);

  //- platform specific implementation
  YAT_THREAD_IMPLEMENTATION;
};

} // namespace yat 

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtThreadImpl.i>
# else
#  include <yat/threading/impl/PosixThreadImpl.i>
# endif
#endif

#endif //- _YAT_THREAD_H_
