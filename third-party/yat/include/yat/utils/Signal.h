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
 * \author Ramon Sune - ALBA
 */
 
#pragma once

#include <list>
#include <yat/utils/Callback.h>
#include <yat/threading/Mutex.h>

/// @file
/// Here we define yat::Signal. This is, a class to which different callbacks
/// can be registered and then will all be run when it is are triggered.
/// It is usefull to implement observers: we will call run() on the signal
/// object without knowing who and how many are interested in this.
/// In UnsafeSignal the list of observers is not mutex protected.
/// The callbacks are defined as yat::Callback objects.

namespace yat 
{

template <typename ParamType_, typename LockType_ = yat::NullMutex> 
class Signal
{
public:
  YAT_DEFINE_CALLBACK(Slot, ParamType_);

protected:
  mutable LockType_ lock_;
  std::list<Slot> observers_;

  bool _run(ParamType_ param, std::list<Slot> & observers)
  {
    typename std::list<Slot>::iterator 
                i(observers.begin()),
                e(observers.end());
    bool everythingOk = true;
    for (; i!=e; ++i) {
      try {
        (*i)(param);
      } catch(...) {
        everythingOk = false;
      }
    }
    return everythingOk;
  }

public:

  bool run(ParamType_ param)
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    return this->_run(param, this->observers_);
  }

  bool connected() const
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    return !this->observers_.empty();
  }

  void connect(Slot cb)
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    this->observers_.remove(cb);
    this->observers_.push_front(cb);
  }

  void disconnect(Slot cb)
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    this->observers_.remove(cb);
  }
};

} // namespace yat
