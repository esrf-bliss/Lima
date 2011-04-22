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

#ifndef _MESSAGE_Q_H_
#define _MESSAGE_Q_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#include <list>
#if defined (YAT_WIN32)
# include <sys/timeb.h>
#else
# include <sys/time.h>
#endif
#include <yat/threading/Semaphore.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Message.h>

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_POST_MSG_TMO   1000
//-----------------------------------------------------------------------------
#define kDEFAULT_LO_WATER_MARK  8
#define kDEFAULT_HI_WATER_MARK  64
//-----------------------------------------------------------------------------
#define kMIN_LO_WATER_MARK      kDEFAULT_LO_WATER_MARK
//-----------------------------------------------------------------------------
#define kMIN_WATER_MARKS_DIFF   kDEFAULT_LO_WATER_MARK
//-----------------------------------------------------------------------------

namespace yat
{

// ============================================================================
// class: MessageQ
// ============================================================================
class YAT_DECL MessageQ
{
  friend class Task;

  typedef std::list<yat::Message *> MessageQImpl;

public:
  //- MessageQ has a state
  typedef enum
  {
    OPEN,
    CLOSED
  } State;
  
  //- MessageQ lo/hi water marks unit
  typedef enum
  {
    //- number of messages in msgQ (default)
    NUM_OF_MSGS,
    //- number of bytes in msgQ
    NUM_OF_BYTES
  } WmUnit;
  
	typedef struct Statistics
  {
    //- default ctor
    Statistics ();
    //- dump stats 
    void dump () const;
    //- did the MessageQ reached the hi-water mark?
    size_t has_been_saturated_;
    //- did the MessageQ reached the low-water mark?
    size_t has_been_unsaturated_;
    //- max. pending charge reached
    unsigned long max_pending_charge_reached_;
    //- max. num of pending msgs reached
    unsigned long max_pending_msgs_reached_;
    //- num. total of msgs posted after <waiting for room in MsgQ>
    unsigned long posted_with_waiting_msg_counter_;
    //- num. total of msgs posted without <waiting for room in MsgQ>
    unsigned long posted_without_waiting_msg_counter_;
    //- num. total of trashed msgs 
    unsigned long trashed_msg_counter_;
    //- num. total of msgs trashed on <post> timeout
    unsigned long trashed_on_post_tmo_counter_;
    //- current pending charge in bytes
    unsigned long pending_charge_;
    //- current pending charge in num. of msgs 
    unsigned long pending_mgs_;
    //- messageQ unit 
    WmUnit wm_unit_;
  } Statistics;

  //- ctor
  MessageQ (size_t lo_wm = kDEFAULT_LO_WATER_MARK,
            size_t hi_wm = kDEFAULT_HI_WATER_MARK,
            bool throw_on_post_tmo = false);

  //- dtor
  virtual ~ MessageQ ();

  //- post a yat::Message into the msgQ
  //- returns 0 if <msg> was successfully posted. tmo expiration, throws an 
  //- if <throw_on_post_msg_timeout> is set to <true>, returns -1 otherwise.
  //- <msg> is destroyed (i.e. released) in case it could not be posted. 
  int post (yat::Message * msg, size_t tmo_msecs = kDEFAULT_POST_MSG_TMO)
    throw (Exception);

  //- extract next message from the msgQ
  Message * next_message (size_t tmo_msecs);
  
  //- Water marks unit mutator
  void wm_unit (WmUnit _wmu);

  //- Water marks unit accessor
  WmUnit wm_unit () const;
  
  //- Low water mark mutator
  void lo_wm (size_t _lo_wm);

  //- Low water mark accessor
  size_t lo_wm () const;

  //- High water mark mutator
  void hi_wm (size_t _hi_wm);

  //- High water mark accessor
  size_t hi_wm () const;
  
  //- Should the msgQ throw an exception on post msg tmo expiration?
  void throw_on_post_msg_timeout (bool _strategy);

  //- Clears msgQ content 
  void clear();

  //- Closes the msqQ
  void close ();
  
  //- Returns the MessageQ stats
  const Statistics & statistics ();
  
private:
  //- periodic msg tmo expired?
	bool periodic_tmo_expired_i (double _tmo_msecs);
  
  //- clears msgQ content (returns num of trashed messages)
  size_t clear_i();

  //- waits for the msQ to contain at least one msg
  //- returns false if tmo expired, true otherwise.
  bool wait_not_empty_i (size_t tmo_msecs);

  //- waits for the msQ to have room for new messages
  //- returns false if tmo expired, true otherwise.
  bool wait_not_full_i (size_t tmo_msecs);

  //- insert a msg according to its priority
  void insert_i (Message * msg)
    throw (Exception);

  //- increment the pending charge
  void inc_pending_charge_i (Message * msg);
  
  //- decrement the pending charge
  void dec_pending_charge_i (Message * msg);
  
  //- use a std::deque to implement msgQ
  MessageQImpl msg_q_;

  //- sync. object in order to make the msgQ thread safe
  Mutex lock_;

  //- Producer(s) synch object
  Condition msg_producer_sync_;

  //- Consumer synch object
  Condition msg_consumer_sync_;
  
  //- state
  MessageQ::State state_;

  //- timeout msg handling flag
  bool enable_timeout_msg_;

  //- periodic msg handling flag
  bool enable_periodic_msg_;

  //- PERIODIC msg timeout
  Timer periodic_msg_timer_;

  //- low water marks
  size_t lo_wm_;

  //- high water marks
  size_t hi_wm_;

  //- msqQ saturation flag
  volatile bool saturated_;

  //- expection activation flag
  bool throw_on_post_msg_timeout_;

  //- flag indicating whether or not the last returned msg was a periodoc msg
  //- we use this flag in order to avoid PERIODIC event flooding in case
  //- the PERIODIC event frequency is really high - which could prevent other
  //- messages from being handled. reciprocally, a very high msg posting freq.
  //- could prevent the PERIODIC msg from being handled. the following tries
  //- to ensure that any msg is "finally" handled.
  bool last_returned_msg_periodic_;

	//- water marks unit
	MessageQ::WmUnit wm_unit_;

	//- depending on the water marks unit, this can be:
  //- 1. the number of pending messages (unit_ = NUM_OF_MSGS)
  //- 2. the number of pending bytes (unit_ = NUM_OF_BYTES)
	size_t pending_charge_;
  
  //- some task/msgQ stats 
	Statistics stats_;

  // = Disallow these operations.
  //--------------------------------------------
  MessageQ & operator= (const MessageQ &);
  MessageQ (const MessageQ &);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/MessageQ.i>
#endif // YAT_INLINE_IMPL

#endif // _MESSAGE_Q_H_
