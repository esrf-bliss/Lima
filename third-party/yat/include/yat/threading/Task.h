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

#ifndef _TASK_H_
#define _TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Thread.h>
#include <yat/threading/MessageQ.h>

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_TASK_TMO_MSECS         5000
#define kDEFAULT_THD_PERIODIC_TMO_MSECS 1000
//-----------------------------------------------------------------------------

namespace yat
{

// ============================================================================
// class: Task
// ============================================================================
class YAT_DECL Task : public yat::Thread
{
  //! a yat::Task = an undetached yat::Thread + a yat::MessageQ
  //-TODO: write some doc!

public:

  //! yat::Task configuration class
  struct YAT_DECL Config
  {
    //- enable TIMEOUT messages
    bool enable_timeout_msg;
    //- timeout msg period in msec
    size_t timeout_msg_period_ms;
    //- enable PERIODIC messages
    bool enable_periodic_msg;
    //- periodic msg period in msec
    size_t periodic_msg_period_ms;
    //- should we process msg under critical section?
    //- not recommended! for backward compatibility only.
    bool lock_msg_handling;
    //- msgQ low water mark
    size_t lo_wm;
    //- msgQ high water mark
    size_t hi_wm;
    //- throw exception on post message timeout
    bool throw_on_post_tmo;
    //- user data (passed back in all msg)
    Thread::IOArg user_data;
    //- default ctor
    Config ();
    //- ctor
    Config (bool   enable_timeout_msg,
            size_t timeout_msg_period_ms,
            bool   enable_periodic_msg,
            size_t periodic_msg_period_ms,
            bool   lock_msg_handling,
            size_t lo_wm,
            size_t hi_wm,
            bool   throw_on_post_tmo,
            Thread::IOArg user_data);
  };

  //- default ctor
  Task ();
  
  //- config ctor
  Task (const Config& cfg);

  //- dtor
  virtual ~Task ();

  //- starts the task
  virtual void go (size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Exception);
    
  //- starts the task 
  //- an exception is thrown in case the specified message:
  //-   * is not of type TASK_INIT
  //-   * is not "waitable"
  virtual void go (Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Exception);
  
  //! aborts the task (join with the underlying thread before returning).
  //! provides an implementation to the Thread::exit pure virtual method.
  virtual void exit ()
    throw (Exception);

  //- posts a message to the task
  virtual void post (Message * msg, size_t tmo_msecs = kDEFAULT_POST_MSG_TMO)
    throw (Exception);

  //- wait for a msg to be handled
  virtual void wait_msg_handled (Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS)
    throw (Exception);
    
  //- timeout msg period mutator
  void set_timeout_msg_period (size_t p_msecs);
  
  //- periodic msg period accessor
  size_t get_timeout_msg_period () const;
  
  //- enable/disable timeout messages
  void enable_timeout_msg (bool enable);

  //- returns timeout messages handling status
  bool timeout_msg_enabled () const;

  //- periodic msg period mutator
  void set_periodic_msg_period (size_t p_msecs);
  
  //- periodic msg period accessor
  size_t get_periodic_msg_period () const;

  //- enable/disable periodic messages
  void enable_periodic_msg (bool enable);

  //- returns period messages handling status
  bool periodic_msg_enabled () const;

  //- MsgQ water marks unit mutator
  void msgq_wm_unit (MessageQ::WmUnit _wmu);

  //- MsgQ water marks unit accessor
  MessageQ::WmUnit msgq_wm_unit () const;

  //- MsgQ low water mark mutator (MsgQ unit dependent)
  void msgq_lo_wm (size_t _lo_wm);
  
  //- MsgQ low water mark accessor (MsgQ unit dependent)
  size_t msgq_lo_wm () const;
  
  //- MsgQ high water mark mutator (MsgQ unit dependent)
  void msgq_hi_wm (size_t _hi_wm);
  
  //- MsgQ high water mark accessor (MsgQ unit dependent)
  size_t msgq_hi_wm () const;
  
  //- MsgQ high water mark accessor (MsgQ unit dependent)
  const MessageQ::Statistics & msgq_statistics ();
  
protected:
  //- run_undetached
  virtual Thread::IOArg run_undetached (Thread::IOArg);
  
  //- message handler
  virtual void handle_message (yat::Message& msg)
     throw (yat::Exception) = 0;

  //- returns the underlying msgQ
  MessageQ & message_queue ();

private:
  //- actual_timeout
  size_t actual_timeout () const;

  //- the associated messageQ
  MessageQ msg_q_;

  //- timeout msg control flag
  bool timeout_msg_enabled_;

  //- timeout msg period
  size_t timeout_msg_period_ms_;

  //- periodic msg control flag
  bool periodic_msg_enabled_;

  //- periodic msg period 
  size_t periodic_msg_period_ms_;

  //- user data passed to entry point
  Thread::IOArg user_data_;

  //- should we process msg under critical section?
  bool lock_msg_handling_;

#if defined (YAT_DEBUG)
  //- some statistics counter
  unsigned long next_msg_counter;
  unsigned long user_msg_counter;
  unsigned long ctrl_msg_counter;
#endif
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/Task.i>
#endif // YAT_INLINE_IMPL

#endif // _TASK_H_
