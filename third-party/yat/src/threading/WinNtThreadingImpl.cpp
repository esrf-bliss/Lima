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

// ----------------------------------------------------------------------------
// almost complete rewrite/extension of omniThread portable threading impl.
// see http://omniorb.sourceforge.net for more omniORB details
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <errno.h>
#include <process.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Semaphore.h>
#include <yat/threading/Thread.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/impl/WinNtMutexImpl.i>
# include <yat/threading/impl/WinNtConditionImpl.i>
# include <yat/threading/impl/WinNtSemaphoreImpl.i>
# include <yat/threading/impl/WinNtThreadImpl.i>
#endif

// ----------------------------------------------------------------------------
// SOME PSEUDO CONSTs
// ----------------------------------------------------------------------------
#define MAX_SLEEP_SECONDS (DWORD)4294966  //- this is (2^32 - 2) / 1000
#define MAX_NSECS 1000000000
  
namespace yat {

// ****************************************************************************
// YAT DUMMY_MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// NullMutex::NullMutex
// ----------------------------------------------------------------------------
NullMutex::NullMutex ()
{
 //- noop
}
// ----------------------------------------------------------------------------
// NullMutex::~NullMutex
// ----------------------------------------------------------------------------
NullMutex::~NullMutex ()
{
 //- noop
}

// ****************************************************************************
// YAT MUTEX IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Mutex::Mutex
// ----------------------------------------------------------------------------
Mutex::Mutex ()
{
 YAT_TRACE("Mutex::Mutex");

 this->m_nt_mux = ::CreateMutex(WIN_NT_NULL, FALSE, WIN_NT_NULL);

 DEBUG_ASSERT(this->m_nt_mux);
}

// ----------------------------------------------------------------------------
// Mutex::~Mutex
// ----------------------------------------------------------------------------
Mutex::~Mutex()
{
  YAT_TRACE("Mutex::~Mutex");

  ::CloseHandle(this->m_nt_mux);
}

// ****************************************************************************
// YAT SEMAPHORE IMPL
// ****************************************************************************
#define SEMAPHORE_MAX_COUNT 0x7fffffff

// ----------------------------------------------------------------------------
// Semaphore::Semaphore
// ----------------------------------------------------------------------------
Semaphore::Semaphore (unsigned int _initial_value)
  : m_nt_sem (WIN_NT_NULL)
{
  YAT_TRACE("Semaphore::Semaphore");

  this->m_nt_sem = ::CreateSemaphore(WIN_NT_NULL, 
                                     (LONG)_initial_value, 
                                     (LONG)SEMAPHORE_MAX_COUNT,
                                     WIN_NT_NULL);
  DEBUG_ASSERT(this->m_nt_sem);
}

// ----------------------------------------------------------------------------
// Semaphore::~Semaphore
// ----------------------------------------------------------------------------
Semaphore::~Semaphore()
{
  YAT_TRACE("Semaphore::~Semaphore");

  if (this->m_nt_sem)
    ::CloseHandle(this->m_nt_sem);
}

// ****************************************************************************
// YAT CONDITION IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Condition::Condition
// ----------------------------------------------------------------------------
Condition::Condition (Mutex & external_lock)
 : m_external_lock (external_lock),
   m_waiters_count (0),
   m_nt_sem (WIN_NT_NULL),
   m_waiters_done (WIN_NT_NULL),
   m_was_broadcast (false)
{
  YAT_TRACE("Condition::Condition");

  ::InitializeCriticalSection (&this->m_waiters_count_lock);

  this->m_nt_sem = ::CreateSemaphore (WIN_NT_NULL,
                                   0,
                                   SEMAPHORE_MAX_COUNT,
                                   WIN_NT_NULL);
  DEBUG_ASSERT(this->m_nt_sem);

  this->m_waiters_done = ::CreateEvent(WIN_NT_NULL,
                                       FALSE, //- no auto-reset
                                       FALSE, //- non-signaled initially
                                       WIN_NT_NULL);
  DEBUG_ASSERT(this->m_waiters_done);
}

// ----------------------------------------------------------------------------
// Condition::~Condition
// ----------------------------------------------------------------------------
Condition::~Condition ()
{
  YAT_TRACE("Condition::~Condition");

  ::DeleteCriticalSection(&this->m_waiters_count_lock);

  while (! ::CloseHandle(this->m_nt_sem)) 
    this->broadcast();

  while (! ::CloseHandle(this->m_waiters_done)) 
    this->broadcast();
}

// ----------------------------------------------------------------------------
// Condition::timed_wait
// ----------------------------------------------------------------------------
bool Condition::timed_wait (unsigned long _tmo_msecs)
{
  //- no timiout occured
  bool no_tmo = true;

  //- null tmo means infinite wait
  //- note: in the current impl. we may wait twice the tmo
  if (! _tmo_msecs) _tmo_msecs = INFINITE;

  //- avoid race conditions 
  ::EnterCriticalSection(&this->m_waiters_count_lock);
  this->m_waiters_count++;
  ::LeaveCriticalSection(&this->m_waiters_count_lock);

  //- this call atomically releases the mutex and waits on the semaphore 
  //- until <Condition::signal> or <Condition::broadcast> are called by 
  //- another thread
  DWORD state = ::SignalObjectAndWait(this->m_external_lock.m_nt_mux, 
                                      this->m_nt_sem, 
                                      static_cast<DWORD>(_tmo_msecs), 
                                      FALSE);

  YAT_LOG("Condition::timed_wait::SignalObjectAndWait returned "
          << std::hex
          << state
          << std::dec);

  //- timeout!
  if (state == WAIT_TIMEOUT)
    no_tmo = false;

  //- reacquire lock to avoid race conditions.
  ::EnterCriticalSection(&this->m_waiters_count_lock);

  //- we're no longer waiting...
  this->m_waiters_count--;

  //- check to see if we're the last waiter after <pthread_cond_broadcast>.
  bool last_waiter = this->m_was_broadcast && this->m_waiters_count == 0;

  ::LeaveCriticalSection(&this->m_waiters_count_lock);

  //- we're the last waiter thread during this particular broadcast then let all 
  //- the other threads proceed
  if (last_waiter)
  {
    //- this call atomically signals the <waiters_done_> event and waits until
    //- it can acquire the <external_mutex>.  This is required to ensure fairness 
    if (::SignalObjectAndWait(this->m_waiters_done, 
                              this->m_external_lock.m_nt_mux, 
                              static_cast<DWORD>(INFINITE),
                              FALSE) == WAIT_TIMEOUT)
    {
      //- relock external mutex since that's the guarantee we give to callers
      ::WaitForSingleObject(this->m_external_lock.m_nt_mux, INFINITE);
      return no_tmo;
    }
  }
  else
  {
    //- relock external mutex since that's the guarantee we give to callers
    ::WaitForSingleObject(this->m_external_lock.m_nt_mux, INFINITE);
  }

  return no_tmo;
}


// ----------------------------------------------------------------------------
// Condition::signal
// ----------------------------------------------------------------------------
void Condition::signal ()
{
  //- do we have waiters...
  ::EnterCriticalSection(&this->m_waiters_count_lock);
  bool have_waiters = this->m_waiters_count > 0;
  ::LeaveCriticalSection(&this->m_waiters_count_lock);

  //- if no waiters then do nothing...  
  if (have_waiters)
    ::ReleaseSemaphore(this->m_nt_sem, 1, 0);
}

// ----------------------------------------------------------------------------
// Condition::broadcast
// ----------------------------------------------------------------------------
void Condition::broadcast ()
{
  //- this is needed to ensure that <m_waiters_count> and <m_was_broadcast> 
  //- are consistent relative to each other
  ::EnterCriticalSection(&this->m_waiters_count_lock);

  bool have_waiters = false;

  if (this->m_waiters_count > 0) 
  {
    //- we are broadcasting, even if there is just one waiter...
    //- record that we are broadcasting, which helps optimize
    //- <Condition::wait> for the non-broadcast case
    this->m_was_broadcast = true;
    have_waiters = true;
  }

  if (have_waiters) 
  {
    //- wake up all the waiters atomically
    ::ReleaseSemaphore(this->m_nt_sem, this->m_waiters_count, 0);
    //- release the waiters countlock
    ::LeaveCriticalSection(&this->m_waiters_count_lock);
    //- wait for all the awakened threads to acquire the counting semaphore 
    ::WaitForSingleObject(this->m_waiters_done, INFINITE);
    //- this assignment is okay, even without the <waiters_count_lock_> held 
    //- because no other waiter threads can wake up to access it.
    this->m_was_broadcast = false;
  }
  else
  {
    ::LeaveCriticalSection(&this->m_waiters_count_lock);
  }
}

// ****************************************************************************
// YAT THREAD IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// YAT common thread entry point (non-OO OS intertace to OO YAT interface)
// ----------------------------------------------------------------------------
unsigned __stdcall yat_thread_common_entry_point (Thread::IOArg _p)
{
  YAT_TRACE_STATIC("yat_thread_common_entry_point");

  //- check input (parano. impl.)
  if (! _p) return 0;

  //- reinterpret input
  Thread * me = reinterpret_cast<Thread*>(_p);

  YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " is starting up");

  //- select detached or undetached mode
  if (me->m_detached)
  {
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " will run detached");
    //- just protect yat impl. against user code using a try/catch statement
    try
    {
      me->run(me->m_iarg);
    }
    catch (...)
    {
      //- ignore any exception
    }
  }
  else 
  {
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " will run undetached");
    //- just protect yat impl. against user code using a try/catch statement
    try
    {
      me->m_oarg = me->run_undetached(me->m_iarg);
    }
    catch (...)
    {
      //- ignore any exception
    }
  }

  YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " has leaved its main loop");

  //- set state to terminated
  {
    //- must lock the mutex even in the case of a detached thread. This is because
    //- a thread may run to completion before the thread that created it has had a
    //- chance to get out of start().  By locking the mutex we ensure that the
    //- creating thread must have reached the end of start() before we delete the
    //- thread object.  Of course, once the call to start() returns, the user can
    //- still incorrectly refer to the thread object, but that's their problem.
    MutexLock guard(me->m_lock);
    //- set state to TERMINATED
    me->m_state = yat::Thread::STATE_TERMINATED;
  }

  //- commit suicide in case the thread ran detached
  if (me->m_detached)
  {
    YAT_LOG_STATIC("yat_thread_common_entry_point::thread " << DUMP_THREAD_UID << " is detached and will be deleted now.");
    delete me;
  }

  //- should never get here
  return 0;
}

// ----------------------------------------------------------------------------
// Thread::Thread
// ----------------------------------------------------------------------------
Thread::Thread (Thread::IOArg _iarg, Thread::Priority _p)
 : //- platform independent members
   m_state (yat::Thread::STATE_NEW),
   m_priority (_p),
   m_iarg (_iarg),
   m_oarg (0),
   m_detached (true),
   m_uid (YAT_INVALID_THREAD_UID),
   //- platform specific members
   m_nt_thread_handle (NULL)
{
  YAT_TRACE("Thread::Thread");
}

// ----------------------------------------------------------------------------
// Thread::~Thread
// ----------------------------------------------------------------------------
Thread::~Thread ()
{
  YAT_TRACE("Thread::~Thread");

  YAT_LOG("Thread::~Thread::deleting thread " << DUMP_THREAD_UID);

  //- close the underlying thread handle
  if (this->m_nt_thread_handle)
    ::CloseHandle(this->m_nt_thread_handle);
}

// ----------------------------------------------------------------------------
// Thread::start [detatched thread]
// ----------------------------------------------------------------------------
void Thread::start ()
{
  YAT_TRACE("Thread::start");
  //- mark the thread as detached
  this->m_detached = true;
  //- then spawn it
  this->spawn();
}

// ----------------------------------------------------------------------------
// Thread::start_undetached [undetatched thread]
// ----------------------------------------------------------------------------
void Thread::start_undetached ()
{
  YAT_TRACE("Thread::start_undetached");
  //- mark the thread as undetached
  this->m_detached = false;
  //- then spawn it
  this->spawn();
}

// ----------------------------------------------------------------------------
// Thread::spawn (common to detatched & undetached threads)
// ----------------------------------------------------------------------------
void Thread::spawn () 
{
  YAT_TRACE("Thread::spawn");

  //- enter critical section
  MutexLock guard(this->m_lock);

  //- be sure the thread is not already running or terminated
  if (this->m_state != yat::Thread::STATE_NEW)
  {
    YAT_LOG("Thread::spawn::thread is either already running or terminated");
    return;
  }

  YAT_LOG("Thread::spawn::spawing thread");

  //- spawn the thread
  unsigned int nt_uid;
  this->m_nt_thread_handle = (HANDLE)::_beginthreadex(WIN_NT_NULL,
                                                      0,
                                                      yat_thread_common_entry_point,
                                                      (LPVOID)this,
                                                      CREATE_SUSPENDED,
                                                      &nt_uid);
  //- check result
  if (this->m_nt_thread_handle == 0)
    throw Exception(); //-TODO: GetLastError(), ..., ...
    
  //- store the thread identifier
  this->m_uid = static_cast<yat::ThreadUID>(nt_uid);

  YAT_LOG("Thread::spawn::thread " << DUMP_THREAD_UID << " spawned");

  YAT_LOG("Thread::spawn::changing thread priority");

  //- set the thread priority
  if (! ::SetThreadPriority(this->m_nt_thread_handle, yat_to_nt_priority(this->m_priority)))
    throw Exception(); //-TODO: GetLastError(), ..., ...

  YAT_LOG("Thread::spawn::resuming thread [was created suspended]");

  //- resume the thread (was created suspended)
  if (::ResumeThread(m_nt_thread_handle) == 0xffffffff)
    throw Exception(); //-TODO: GetLastError(), ..., ...

  //- mark the thread as running (before leaving the critical section)
  this->m_state = yat::Thread::STATE_RUNNING;
}

// ----------------------------------------------------------------------------
// Thread::join
// ----------------------------------------------------------------------------
void Thread::join (Thread::IOArg * oarg_)
{
  YAT_TRACE("Thread::join");

  {
    //- enter critical section
    MutexLock guard(this->m_lock);
    //- check thread state
    if (   
           (this->m_state != yat::Thread::STATE_RUNNING) 
        && 
           (this->m_state != yat::Thread::STATE_TERMINATED)
       )
       {
         if (oarg_) *oarg_ = 0;
         delete this;
         return;
       }
  }

  //- be sure the thread is not detached
  if (this->m_detached)
    throw Exception(); //-TODO

  YAT_LOG("Thread::join::waiting for the thread to terminate [WaitForSingleObject]");

  if (::WaitForSingleObject(this->m_nt_thread_handle, INFINITE) != WAIT_OBJECT_0)
    throw Exception(); //-TODO

  YAT_LOG("Thread::join::thread exit successfully");

  //- return the "thread result"
  if (oarg_)
    *oarg_ = this->m_oarg;

  //- commit suicide
  delete this;
}

// ----------------------------------------------------------------------------
// Thread::priority
// ----------------------------------------------------------------------------
void Thread::priority (Priority _p)
{
  YAT_TRACE("Thread::priority");

  //- enter critical section
  MutexLock guard(this->m_lock);

  //- apply priority if thread is running
  if (this->m_state == yat::Thread::STATE_RUNNING)
  {
    YAT_LOG("Thread::priority::thread is running - applying priority");

    if (! ::SetThreadPriority(this->m_nt_thread_handle, yat_to_nt_priority(this->m_priority)))
      throw Exception(); //-TODO: GetLastError()

    YAT_LOG("Thread::priority::thread priority successfully changed");
  }

  //- store new priority
  this->m_priority = _p;
}

// ----------------------------------------------------------------------------
// Thread::yat_to_nt_priority
// ----------------------------------------------------------------------------
int Thread::yat_to_nt_priority (Priority _p)
{
  switch (_p) 
  {
    case yat::Thread::PRIORITY_LOW:
      return THREAD_PRIORITY_LOWEST;

    case yat::Thread::PRIORITY_HIGH:
      return THREAD_PRIORITY_HIGHEST;

    case yat::Thread::PRIORITY_RT:
      return THREAD_PRIORITY_TIME_CRITICAL;

    default:
      return THREAD_PRIORITY_NORMAL;
  }

  //- make some compilers happy
  return THREAD_PRIORITY_NORMAL;
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::self
// ----------------------------------------------------------------------------
ThreadUID ThreadingUtilities::self ()
{
  return static_cast<yat::ThreadUID>(::GetCurrentThreadId());
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::sleep
// ----------------------------------------------------------------------------
void ThreadingUtilities::sleep (unsigned long _secs, unsigned long _nano_secs)
{
  //- requested sleep time <= to max sleep time per ::Sleep call
  if (_secs <= MAX_SLEEP_SECONDS)
  {
    ::Sleep(_secs * 1000 + _nano_secs / 1000000);
    return;
  }

  //- requested sleep time > to max sleep time per ::Sleep call
  DWORD no_of_max_sleeps = _secs / MAX_SLEEP_SECONDS;
  for (DWORD i = 0; i < no_of_max_sleeps; i++)
    ::Sleep(MAX_SLEEP_SECONDS * 1000);
  ::Sleep((_secs % MAX_SLEEP_SECONDS) * 1000 + _nano_secs / 1000000);
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::get_time
// ----------------------------------------------------------------------------
void ThreadingUtilities::get_time (Timespec & abs_time, 
                                   unsigned long delay_msecs)
{
  unsigned long abs_sec_, abs_nano_sec_;
  ThreadingUtilities::get_time_now (abs_sec_, abs_nano_sec_);

  abs_time.tv_sec  = abs_sec_;
  abs_time.tv_nsec = abs_nano_sec_;
  abs_time.tv_nsec += delay_msecs * 1000000;
  abs_time.tv_sec  += abs_time.tv_nsec / MAX_NSECS;
  abs_time.tv_nsec %= MAX_NSECS;
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::get_time
// ----------------------------------------------------------------------------
void ThreadingUtilities::get_time (unsigned long & abs_sec_,
                                   unsigned long & abs_nano_sec_,
                                   unsigned long _rel_sec,
                                   unsigned long _rel_nano_sec)
{
  ThreadingUtilities::get_time_now (abs_sec_, abs_nano_sec_);
  abs_nano_sec_ += _rel_nano_sec;
  abs_sec_ += _rel_sec + abs_nano_sec_ / 1000000000;
  abs_nano_sec_ = abs_nano_sec_ % 1000000000;
}

// ----------------------------------------------------------------------------
// ThreadingUtilities::get_time_now
// ----------------------------------------------------------------------------
void ThreadingUtilities::get_time_now (unsigned long & abs_sec_, 
                                       unsigned long & abs_nano_sec_)
{
  static int days_in_preceding_months[12]
    = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
  static int days_in_preceding_months_leap[12]
    = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

  SYSTEMTIME st;
  ::GetSystemTime(&st);
  abs_nano_sec_ = st.wMilliseconds * 1000000;

  // this formula should work until 1st March 2100
  DWORD days = ((st.wYear - 1970) * 365 + (st.wYear - 1969) / 4
             + ((st.wYear % 4)
             ? days_in_preceding_months[st.wMonth - 1]
             : days_in_preceding_months_leap[st.wMonth - 1])
             + st.wDay - 1);

  abs_sec_ = st.wSecond + 60 * (st.wMinute + 60 * (st.wHour + 24 * days));
}

} // namespace yat
