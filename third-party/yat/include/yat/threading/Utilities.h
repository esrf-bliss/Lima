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

#ifndef _YAT_THREADING_UTILS_H_
#define _YAT_THREADING_UTILS_H_

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <yat/time/Timer.h>
#include <yat/threading/Implementation.h>

// ----------------------------------------------------------------------------
// CONSTs
// ----------------------------------------------------------------------------
#if defined(YAT_MACOSX)
# define YAT_INVALID_THREAD_UID 0
#else
# define YAT_INVALID_THREAD_UID 0xffffffff
#endif

namespace yat {

// ----------------------------------------------------------------------------
//! A dedicated type for thread identifier
// ----------------------------------------------------------------------------
#if defined(YAT_MACOSX)
  typedef _opaque_pthread_t * ThreadUID;
#else
  typedef unsigned long ThreadUID;
#endif
  
// ----------------------------------------------------------------------------
//! The YAT threading utilities
// ----------------------------------------------------------------------------
class YAT_DECL ThreadingUtilities
{
public:
   //! Returns the calling thread identifier.
  static ThreadUID self ();

  //! Causes the caller to sleep for the given time.
  static void sleep (unsigned long secs, unsigned long nanosecs = 0);

  //! Calculates an absolute time in seconds and nanoseconds, suitable for
  //! use in timed_waits, which is the current time plus the given relative 
  //! offset.
  static void get_time (unsigned long & abs_sec,
                        unsigned long & abs_nsec,
                        unsigned long offset_sec = 0,
                        unsigned long offset_nsec = 0);

  //! Calculates an absolute time in seconds and nanoseconds, suitable for
  //! use in timed_waits, which is the current time plus the given relative 
  //! offset.
  static void get_time (Timespec & abs_time, unsigned long offset_msecs);

private:

#if defined (YAT_WIN32)
  //- internal impl
  static void ThreadingUtilities::get_time_now (unsigned long & abs_sec_, 
                                                unsigned long & abs_nano_sec_);
#endif  
};

} // namespace yat 

#endif //- _YAT_THREADING_UTILS_H_
