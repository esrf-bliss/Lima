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

namespace yat
{

// ============================================================================
// MessageQ::periodic_tmo_expired
// ============================================================================
YAT_INLINE bool MessageQ::periodic_tmo_expired_i (double _tmo_msecs)
{
  return this->periodic_msg_timer_.elapsed_msec() >= _tmo_msecs;
}

// ============================================================================
// MessageQ::lo_wm
// ============================================================================
YAT_INLINE void MessageQ::lo_wm (size_t _lo_wm)
{
  this->lo_wm_ = _lo_wm;  

  if (this->lo_wm_ < kMIN_LO_WATER_MARK)
    this->lo_wm_ = kMIN_LO_WATER_MARK;

  if ((this->hi_wm_ - this->lo_wm_) < kMIN_WATER_MARKS_DIFF)
    this->hi_wm_ = this->lo_wm_ + kMIN_WATER_MARKS_DIFF;
}

// ============================================================================
// MessageQ::lo_wm
// ============================================================================
YAT_INLINE size_t MessageQ::lo_wm ()  const
{
  return this->lo_wm_;
}

// ============================================================================
// MessageQ::hi_wm
// ============================================================================
YAT_INLINE void MessageQ::hi_wm (size_t _hi_wm)
{
  this->hi_wm_ = _hi_wm;

  if ((this->hi_wm_ - this->lo_wm_) < kMIN_WATER_MARKS_DIFF)
    this->hi_wm_ = this->lo_wm_ + kMIN_WATER_MARKS_DIFF;
}

// ============================================================================
// MessageQ::hi_wm
// ============================================================================
YAT_INLINE size_t MessageQ::hi_wm ()  const
{
  return this->hi_wm_;
}

// ============================================================================
// MessageQ::throw_on_post_msg_timeout
// ============================================================================
YAT_INLINE void MessageQ::throw_on_post_msg_timeout (bool _strategy)
{
  this->throw_on_post_msg_timeout_ = _strategy;
}

// ============================================================================
// MessageQ::clear
// ============================================================================
YAT_INLINE void MessageQ::clear ()
{
  YAT_TRACE("MessageQ::clear");

  yat::AutoMutex<> guard(this->lock_);

  this->clear_i();
}

// ============================================================================
// MessageQ::wm_unit
// ============================================================================
YAT_INLINE void MessageQ::wm_unit (MessageQ::WmUnit _u)
{
  this->wm_unit_ = _u;
  this->stats_.wm_unit_ = _u; 
}

// ============================================================================
// MessageQ::unit
// ============================================================================
YAT_INLINE MessageQ::WmUnit MessageQ::wm_unit () const
{
  return this->wm_unit_;
}

// ============================================================================
// MessageQ::inc_pending_charge_i
// ============================================================================
YAT_INLINE void MessageQ::inc_pending_charge_i (Message * _msg)
{
  this->pending_charge_ += (this->wm_unit_ == NUM_OF_MSGS)
                         ? 1
                         : _msg->size_in_bytes();
}
  
// ============================================================================
// MessageQ::dec_pending_charge_i
// ============================================================================
YAT_INLINE void MessageQ::dec_pending_charge_i (Message * _msg)
{ 
  this->pending_charge_ -= (this->wm_unit_ == NUM_OF_MSGS)
                         ? 1
                         : _msg->size_in_bytes();
}

// ============================================================================
// MessageQ::statistics
// ============================================================================
YAT_INLINE const MessageQ::Statistics & MessageQ::statistics ()
{
  {
    MutexLock guard(this->lock_);
    this->stats_.pending_charge_ = this->pending_charge_;
    this->stats_.pending_mgs_ = this->msg_q_.size();
  }
  
  return this->stats_;
}

} //- namespace
