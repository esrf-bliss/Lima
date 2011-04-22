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

#ifndef _YAT_CONDITION_H_
#define _YAT_CONDITION_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Implementation.h>

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_CONDITION_IMPLEMENTATION)
# error "implementation header file incomplete [no condition implementation]"
#endif

namespace yat {

// ----------------------------------------------------------------------------
// FORWARD DECL
// ----------------------------------------------------------------------------
class Mutex;

// ----------------------------------------------------------------------------
//! \class Condition
//! \brief The YAT Condition variable class
//!
//! The Windows implementation is based on D.C.Schmidt & Al solution describes
//! in the following article: http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
//!
//! Under Linux (and any other \c POSIX platforms), the code relies on the local
//! \c pthread implementation.
//!
//! \remarks
//! While its destructor is virtual, this class is not supposed to be derived.\n
//! Be sure to clearly understand the internal behaviour before trying to do so.
// ----------------------------------------------------------------------------
class YAT_DECL Condition
{
public:
  //! Constructor.
  //!
  //! Each condition must be associated to a mutex that must be hold while
  //! evaluating the condition. It means that \a external_mutex must be locked
  //! prior to any to call to the Condition interface. See \link
  //! http://www.cs.wustl.edu/~schmidt/win32-cv-1.html D.C.Schmidt and I.Pyarali
  //! \endlink article for details.
  Condition (yat::Mutex& external_mutex);

  //! Destructor.
  //!
  //! While this destructor is virtual, this class is not supposed to be derived.
  //! Be sure to understand the internal behaviour before trying to do so.
  virtual ~Condition ();

  //! Wait until the condition is either \link Condition::signal signaled\endlink
  //! or \link Condition::broadcast broadcasted\endlink by another thread.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  void wait ();

  //! Wait for the condition to be \link Condition::signal signaled\endlink
  //! or \link Condition::broadcast broadcasted\endlink by another thread.
  //! Returns \c false in case the specified timeout expired before the condition 
  //! was notified. Returns \c true otherwise.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  //!
  //! \param tmo_msecs The timeout in milliseconds
  //! \return \c false [timeout expired] or \c true [condition notified]
  bool timed_wait (unsigned long tmo_msecs);

  //! Signals the condition by notifying \b one of the waiting threads.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  void signal ();

  //! Broadcasts the condition by notifying \b all waiting threads.
  //!
  //! The associated \a external_mutex <b>must be locked</b> by the calling thread.
  void broadcast ();

private:
  //! The so called "external mutex" (see D.Schmidt's article)
  Mutex & m_external_lock;
  
  //! Not implemented private member
  Condition (const Condition&);
  //! Not implemented private member
  Condition & operator= (const Condition&);
  
  //! hidden/abstract platform specific implementation
  YAT_CONDITION_IMPLEMENTATION;
};

} // namespace yat

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtConditionImpl.i>
# else
#  include <yat/threading/impl/PosixConditionImpl.i>
# endif
#endif

#endif //- _YAT_CONDITION_H_
