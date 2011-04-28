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
#include <yat/threading/Message.h>
#include <iostream>

#if !defined (YAT_INLINE_IMPL)
# include <yat/threading/Message.i>
#endif // YAT_INLINE_IMPL

namespace yat
{
// ============================================================================
// Message::cache
// ============================================================================
#if defined(_USE_MSG_CACHE_)
  Message::Cache  Message::m_cache;
#endif

// ============================================================================
// Message::msg_counter
// ============================================================================
#if defined (YAT_DEBUG)
  Message::MessageID Message::msg_counter = 0;
  size_t Message::ctor_counter = 0;
  size_t Message::dtor_counter = 0;
#endif

// ============================================================================
// Message::allocate 
// ============================================================================
Message * Message::allocate (size_t _msg_type, size_t _msg_priority, bool _waitable)
  throw (Exception)
{
  YAT_TRACE_STATIC("Message::allocate");  
  
  yat::Message * msg = 0;
     
  try
  {
    msg = new yat::Message (_msg_type, _msg_priority, _waitable);
    if (msg == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "Message allocation failed",
                    "Message::allocate");
  }
  catch (...)
  {
    THROW_YAT_ERROR("UNKNOWN_ERROR",
                    "Message allocation failed [unknown exception caught]",
                    "Message::allocate");
  }

  return msg;
}

// ============================================================================
// Message::Message
// ============================================================================
Message::Message ()
  : SharedObject (),
    processed_ (false),
    type_ (FIRST_USER_MSG),
    priority_ (DEFAULT_MSG_PRIORITY),
    user_data_ (0),
    msg_data_ (0),
    has_error_ (false),
    cond_ (0),
    size_in_bytes_ (sizeof(yat::Message))
#if defined (YAT_DEBUG)
    , id_ (++Message::msg_counter)
#endif
{
  YAT_TRACE("Message::Message");

#if defined (YAT_DEBUG)
  Message::ctor_counter++;
#endif
}

// ============================================================================
// Message::Message
// ============================================================================
Message::Message (size_t _msg_type, size_t _msg_priority, bool _waitable)
  : SharedObject (),
    processed_ (false),
    type_ (_msg_type),
    priority_ (_msg_priority),
    user_data_ (0),
    msg_data_ (0),
    has_error_ (false),
    cond_ (0),
    size_in_bytes_ (sizeof(yat::Message))
#if defined (YAT_DEBUG)
    , id_ (++Message::msg_counter)
#endif
{
  YAT_TRACE("Message::Message");

#if defined (YAT_DEBUG)
  Message::ctor_counter++;
#endif

  if (_waitable)
    this->make_waitable();
}

// ============================================================================
// Message::~Message
// ============================================================================
Message::~Message ()
{
  YAT_TRACE("Message::~Message");

#if defined (YAT_DEBUG)
  Message::dtor_counter++;
  YAT_LOG("Message::~Message::ctor_counter: " << Message::ctor_counter);
  YAT_LOG("Message::~Message::dtor_counter: " << Message::dtor_counter);
#endif

  if (this->msg_data_)
  {
    delete this->msg_data_;
    this->msg_data_ = 0;
  }

  if (this->cond_)
  {
    if (! this->processed_)
    {
      AutoMutex<Mutex> guard(this->lock_);
      this->cond_->broadcast();
    }
    delete this->cond_;
    this->cond_ = 0;
  }
}

// ============================================================================
// Message::to_string
// ============================================================================
const char * Message::to_string () const
{
  switch (this->type_)
  {
    case TASK_INIT:
      return "TASK_INIT";
      break;
    case TASK_TIMEOUT:
      return "TASK_TIMEOUT";
      break;
    case TASK_PERIODIC:
      return "TASK_PERIODIC";
      break;
    case TASK_EXIT:
      return "TASK_EXIT";
      break;
  }
  return "UNKNOWN OR USER DEFINED MSG";
}

// ============================================================================
// Message::dump
// ============================================================================
void Message::dump () const
{
  std::cout << "---- Msg@" << std::hex << this << std::dec << std::endl;
#if defined (YAT_DEBUG)
  std::cout << "- id.........." << this->id_ << std::endl;
#endif
  std::cout << "- processed..." << (this->processed_ ? "true" : "false") << std::endl;
  std::cout << "- type........" << this->type_ << std::endl;
  std::cout << "- priority...." << this->priority_ << std::endl;
  std::cout << "- has error..." << this->has_error_ << std::endl;
  std::cout << "- user_data..." << std::hex << this->user_data_ << std::dec <<  std::endl;
  std::cout << "- msg_data...." << std::hex << this->msg_data_ << std::dec << std::endl;
  std::cout << "- waitable...." << (this->cond_ ? "true" : "false") << std::endl;
}

// ============================================================================
// Message::make_waitable 
// ============================================================================
void Message::make_waitable ()
  throw (Exception)
{ 
  if (this->cond_)
    return;

  try
  {
    this->cond_ = new yat::Condition(this->lock_);
    if (this->cond_ == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    THROW_YAT_ERROR("MEMORY_ERROR",
                    "memory allocation failed",
                    "Message::make_waitable");
  }
  catch (...)
  {
    THROW_YAT_ERROR("UNKNOWN_ERROR",
                    "memory allocation failed",
                    "Message::make_waitable");
  }
}

} // namespace
