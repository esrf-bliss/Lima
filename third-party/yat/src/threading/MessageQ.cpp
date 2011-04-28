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

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <math.h>
#include <algorithm>
#include <yat/CommonHeader.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/MessageQ.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/MessageQ.i>
#endif // YAT_INLINE_IMPL

namespace yat
{

// ============================================================================
// MessageQ::Stats::Stats
// ============================================================================
 MessageQ::Statistics::Statistics()
  : has_been_saturated_ (0),
    has_been_unsaturated_ (0),
    max_pending_charge_reached_ (0),
    max_pending_msgs_reached_ (0),
    posted_with_waiting_msg_counter_ (0),
    posted_without_waiting_msg_counter_ (0),
    trashed_msg_counter_ (0),
    trashed_on_post_tmo_counter_ (0),
    pending_charge_ (0),
    pending_mgs_ (0),
    wm_unit_ (MessageQ::NUM_OF_MSGS)
{
	//- noop
}

// ============================================================================
// MessageQ::Stats::dump
// ============================================================================
void MessageQ::Statistics::dump () const
{
  std::cout << "MessageQ::statistics::has reached hw................" 
            << this->has_been_saturated_ 
            << " times" 
            << std::endl;
            
  std::cout << "MessageQ::statistics::has reached lw................" 
            << this->has_been_unsaturated_ 
            << " times" 
            << std::endl;
  
  if (this->wm_unit_ == NUM_OF_BYTES)
  {
  	std::cout << "MessageQ::statistics::has contained up to..........." 
            	<< this->max_pending_charge_reached_ 
            	<< " bytes"
            	<< std::endl;
  }
  
  std::cout << "MessageQ::statistics::has contained up to..........." 
            << this->max_pending_msgs_reached_ 
            << " msgs"
            << std::endl;
            
  std::cout << "MessageQ::statistics::immediate msg posting........." 
            << this->posted_without_waiting_msg_counter_ 
            << std::endl;
  
  std::cout << "MessageQ::statistics::delayed msg posting..........." 
            << this->posted_with_waiting_msg_counter_ 
            << std::endl;
  
  std::cout << "MessageQ::statistics::trashed on post tmo..........." 
            << this->trashed_on_post_tmo_counter_ 
            << std::endl;
  
  std::cout << "MessageQ::statistics::trashed on other criteria....." 
            << this->trashed_msg_counter_ 
            << std::endl;

  std::cout << "MessageQ::statistics::pending charge................." 
            << this->pending_charge_
            << " bytes"
            << std::endl;

  std::cout << "MessageQ::statistics::pending msgs..................." 
            << this->pending_mgs_
            << " msgs"
            << std::endl;

  unsigned long total_msg = this->posted_with_waiting_msg_counter_ 
                          + this->posted_without_waiting_msg_counter_
                          + this->trashed_msg_counter_
                          + this->trashed_on_post_tmo_counter_;

  std::cout << "MessageQ::statistics::total msg....................." 
            << total_msg 
            << std::endl;
}

// ============================================================================
// MessageQ::MessageQ
// ============================================================================
MessageQ::MessageQ (size_t _lo_wm, size_t _hi_wm, bool _throw_on_post_tmo)
:   
    msg_q_ (0),
    msg_producer_sync_ (lock_),
    msg_consumer_sync_ (lock_),
    state_(MessageQ::OPEN),
    enable_timeout_msg_ (false),
    enable_periodic_msg_ (false),
    periodic_msg_timer_ (),
    lo_wm_ (_lo_wm),
    hi_wm_ (_hi_wm),
    saturated_ (false),
    throw_on_post_msg_timeout_ (_throw_on_post_tmo),
    last_returned_msg_periodic_ (false),
    wm_unit_ (NUM_OF_MSGS),
    pending_charge_ (0),
    stats_()
{
  YAT_TRACE("MessageQ::MessageQ");
}

// ============================================================================
// MessageQ::~MessageQ
// ============================================================================
MessageQ::~MessageQ ()
{
  YAT_TRACE("MessageQ::~MessageQ");
  
  MutexLock guard(this->lock_);

  this->state_ = MessageQ::CLOSED;

  this->clear_i();
}

// ============================================================================
// MessageQ::close
// ============================================================================
void MessageQ::close ()
{
  //- YAT_TRACE("MessageQ::close");
  
  MutexLock guard(this->lock_);

  this->state_ = MessageQ::CLOSED;
}

// ============================================================================
// MessageQ::clear_i
// ============================================================================
size_t MessageQ::clear_i ()
{
  size_t num_msg_in_q = this->msg_q_.size();

  while (! this->msg_q_.empty())
  {
    Message * m = this->msg_q_.front ();
    if (m) m->release();
    this->msg_q_.pop_front();
  }
  
  this->pending_charge_ = 0;

  return num_msg_in_q;
}

// ============================================================================
// MessageQ::post
// ============================================================================
int MessageQ::post (yat::Message * msg, size_t _tmo_msecs)
  throw (Exception)
{
  YAT_TRACE("MessageQ::post");

  //- check input
  if (! msg) return 0;

  //- can't post any TIMEOUT or PERIODIC msg (yat::Task model violation)
  if (msg->type() == TASK_TIMEOUT || msg->type() == TASK_PERIODIC)
  {
    this->stats_.trashed_msg_counter_++;
    //- silently trash the message
    msg->release();
    return 0;
  }
  
  { //- critical section
  
  	//- lock (required to protect the msgQ and for cond. vars. to work properly)
  	MutexLock guard(this->lock_);

  	//- can only post a msg on an opened MsgQ
  	if (this->state_ != MessageQ::OPEN)
  	{
    	this->stats_.trashed_msg_counter_++;
    	//- silently trash the message (should we throw an exception instead?)
    	msg->release();
    	return 0;
  	}

  	//- we force post of ctrl message even if the msQ is saturated
  	if (msg->is_task_ctrl_message()) 
  	{
    	//- insert msg according to its priority
    	try
    	{
      	this->insert_i(msg);
    	}
    	catch (...)
    	{
      	//- insert_i released the message (no memory leak)
      	THROW_YAT_ERROR("INTERNAL_ERROR",
                      	"Could not post ctrl message [msgQ insertion error]",
                      	"MessageQ::post");
    	}
      
    	//- wakeup msg consumers (tell them there is a message to handle)
    	//- this will work since we are under critical section 
    	msg_consumer_sync_.broadcast();
      
    	//- compute stats 
    	this->stats_.posted_without_waiting_msg_counter_++;

    	//- done (skip remaining code)
    	return 0;
  	}

  	//- is the messageQ saturated?
  	if (! this->saturated_ && (this->pending_charge_ >= this->hi_wm_))
  	{
    	YAT_LOG("MessageQ::post::**** SATURATED ****");
    	//- compute stats 
    	this->stats_.has_been_saturated_++;
    	//- mark msgQ as saturated
    	this->saturated_ = true;
  	}

  	//- msg is not a ctrl message...
  	//- wait for the messageQ to have room for new messages
  	if (! this->wait_not_full_i(_tmo_msecs))
  	{
    	YAT_LOG("MessageQ::post::tmo expired");
    	//- can't post msg, destroy it in order to avoid memory leak
    	msg->release(); 
    	//- compute stats 
    	this->stats_.trashed_on_post_tmo_counter_++;
    	//- throw exception if the messageQ is configured to do so
    	if (this->throw_on_post_msg_timeout_)
    	{
      	THROW_YAT_ERROR("TIMEOUT_EXPIRED",
                      	"Could not post message [timeout expired]",
                      	"MessageQ::post");
    	}
    	//- return if we didn't throw an exception
    	return -1;
  	}

  	//- ok there is enough room to post our msg
  	DEBUG_ASSERT(this->pending_charge_ <= this->hi_wm_);

  	//- insert the message according to its priority
  	try
  	{
    	this->insert_i(msg);
  	}
  	catch (...)
  	{
    	//- insert_i released the message (no memory leak)
    	THROW_YAT_ERROR("INTERNAL_ERROR",
                    	"Could not post message [msgQ insertion error]",
                    	"MessageQ::post");
  	}

  	//- compute stats 
  	if (this->pending_charge_ > this->stats_.max_pending_charge_reached_)
    	this->stats_.max_pending_charge_reached_ = this->pending_charge_;

  	if (this->msg_q_.size() > this->stats_.max_pending_msgs_reached_)
    	this->stats_.max_pending_msgs_reached_ = static_cast<unsigned long>(this->msg_q_.size());

  	//- wakeup msg consumers (tell them there is a new message to handle)
  	//- this will work since we are still under critical section
  	msg_consumer_sync_.broadcast ();
    
	} //- critical section
  
  return 0;
}

// ============================================================================
// MessageQ::next_message
// ============================================================================
yat::Message * MessageQ::next_message (size_t _tmo_msecs)
{
  YAT_TRACE("MessageQ::next_message");

  YAT_LOG("MessageQ::next_message::waiting for next message");

  //- enter critical section (required for cond.var. to work properly)
  MutexLock guard(this->lock_);

  //- wait for the messageQ to contain at least one message or tmo expired
  if (! this->wait_not_empty_i(_tmo_msecs))
  {
    //- <wait_not_empty_i> returned <false> : means no msg in msg queue after <_tmo_msecs>
    YAT_LOG("MessageQ::next_message::tmo expired [MessageQ::wait_not_empty_i returned false]");
    
    //- is it time to return a periodic message?
    if (this->enable_periodic_msg_ && this->periodic_tmo_expired_i(_tmo_msecs))
    {
      this->last_returned_msg_periodic_ = true;
      this->periodic_msg_timer_.restart();
      return new Message(TASK_PERIODIC); 
    }
    
    //- Task timeout expired (see <wait_not_empty_i> impl for details)
    if (this->enable_timeout_msg_)
    {
      this->last_returned_msg_periodic_ = false;
      return new Message(TASK_TIMEOUT);
    }
    
    //- no msg 
    this->last_returned_msg_periodic_ = false; 
    
    return 0;
  }
  
  //- <wait_not_empty_i> returned <true> : means there is at least one msg in msg queue

  //- ok, there should be at least one message in the messageQ
  DEBUG_ASSERT(this->msg_q_.empty() == false);

  //- we are still under critical section since the "Condition::timed_wait" 
  //- located in "wait_not_empty_i" garantee that the associated mutex (i.e. 
  //- this->lock_ in the present case) is acquired when the function returns
  
  //- get msg from Q
  yat::Message * msg = this->msg_q_.front();
                           
  //- parano. debugging
  DEBUG_ASSERT(msg != 0);

  //- if the extracted msg is a TASK ctrl msg...
  if (msg->is_task_ctrl_message())
  {
    //... then extract it from the Q and return it
    this->msg_q_.pop_front();
    
    //- dec pending charge  
    this->dec_pending_charge_i(msg);
    
    //- if we reach the low water mark, then wake up msg producer(s) 
    if (this->saturated_ && this->pending_charge_ <= this->lo_wm_)
    {
      YAT_LOG("MessageQ::next_message::**** UNSATURATED ****");
      //- compute stats 
    	this->stats_.has_been_unsaturated_++;
      //- no more saturated
      this->saturated_ = false;
      //- this will work since we are still under critical section
      this->msg_producer_sync_.broadcast();
    }
    
    //- we are about to return a ctrl msg so...
    this->last_returned_msg_periodic_ = false;

    //- return ctrl message
    return msg;
  }

  //- avoid PERIODIC msg starvation (see note above)
  if (
       this->enable_periodic_msg_ 
         && 
       this->periodic_tmo_expired_i (_tmo_msecs) 
         && 
       this->last_returned_msg_periodic_ == false
     )
  {
    //- we didn't actually extract the <msg> from the Q.
    //- we just accessed it using "pop_front", so no need to reinject it into the Q
    this->last_returned_msg_periodic_ = true;
    this->periodic_msg_timer_.restart();
    return new Message(TASK_PERIODIC); 
  }
  
  //- we are about to return a msg from the Q so...
  this->last_returned_msg_periodic_ = false;
  
  //- then extract it from the Q and return it
  this->msg_q_.pop_front();
  
  //- dec pending charge  
  this->dec_pending_charge_i(msg);
    
  //- if we reach the low water mark, then wakeup msg producer(s) 
  if (this->saturated_ && this->pending_charge_ <= this->lo_wm_)
  {
    //- compute stats 
    this->stats_.has_been_unsaturated_++;
    //- no longer saturated
    this->saturated_ = false;
    //- this will work since we are still under critical section
    this->msg_producer_sync_.broadcast(); 
  }
  
  return msg;
}

// ============================================================================
// MessageQ::wait_not_empty_i
// ============================================================================
bool MessageQ::wait_not_empty_i (size_t _tmo_msecs)
{
  //- <this->lock_> MUST be locked by the calling thread
  //----------------------------------------------------

  //- while the messageQ is empty...
  while (this->msg_q_.empty ())
  {
     //- wait for a msg or tmo expiration 
    if (! this->msg_consumer_sync_.timed_wait(static_cast<unsigned long>(_tmo_msecs)))
      return false; 
  }

  //- at least one message available in the MsgQ
  return true;
} 


// ============================================================================
// MessageQ::wait_not_full_i
// ============================================================================
bool MessageQ::wait_not_full_i (size_t _tmo_msecs)
{
  //- <this->lock_> MUST be locked by the calling thread
  //----------------------------------------------------

 	//- compute stats
 	if ( ! this->saturated_)
 	{
		//- compute stats
   	this->stats_.posted_without_waiting_msg_counter_++;
  	return true;
  }
  
  //- while the messageQ is empty...
  while (this->saturated_)
  {
    //- wait for a msg or tmo expiration 
    if (! this->msg_producer_sync_.timed_wait(static_cast<unsigned long>(_tmo_msecs)))
      return false; 
    //- compute stats
    this->stats_.posted_with_waiting_msg_counter_++;
  }
    
  //- at least one message available in the MsgQ
  return true;
}

// ============================================================================
// Binary predicate
// ============================================================================
static bool insert_msg_criterion (Message * const m1, Message * const m2)
{
  return m2->priority() < m1->priority();  
}
 
// ============================================================================
// MessageQ::insert_i
// ============================================================================
void MessageQ::insert_i (Message * _msg)
  throw (Exception)
{
  DEBUG_ASSERT(_msg != 0);
  
  try
  {
    if (this->msg_q_.empty())
    {
      //- optimization: no need to take count of the msg priority
      this->msg_q_.push_front (_msg);
    }
    else
    { 
      //- insert msg according to its priority
      MessageQImpl::iterator pos = std::upper_bound(this->msg_q_.begin(),
                                                    this->msg_q_.end(),
                                                    _msg,
                                                    insert_msg_criterion);
      this->msg_q_.insert(pos, _msg);
    }
    
    //- inc pending charge  
    this->inc_pending_charge_i(_msg);
  }
  catch (...)
  {
    Exception e("INTERNAL_ERROR", 
                "could insert message into the message queue", 
                "MessageQ::insert_i");
    _msg->set_error(e);
    _msg->processed();
    _msg->release();
  }
}

} // namespace
