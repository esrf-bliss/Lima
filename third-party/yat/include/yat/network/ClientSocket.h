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

#ifndef _YAT_CLIENT_SOCKET_H_
#define _YAT_CLIENT_SOCKET_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/network/Socket.h>

namespace yat { 
  
// ----------------------------------------------------------------------------
//! The YAT ClientSocket class
// ----------------------------------------------------------------------------
class YAT_DECL ClientSocket : public Socket
{
  //! This is the yat network socket class.
  //!
  //! Base class for both TCPClientSocket and UDPClientSocket.

public:
  //! Construct new CleintSocket
  //! 
  //! \param p The associated protocol. Defaults to \c yat::TCP_PROTOCOL
  //!
  //! \remarks May throw an Exception.
  ClientSocket (Protocol p = TCP_PROTOCOL);
      
  //! Release any allocated resource.
  virtual ~ClientSocket ();

  //! Bind socket to specified port
  //! 
  //! \param p The port
  void bind (size_t _p = 0)
    throw (SocketException);
    
  //! Connect to peer socket.
  //!
  //! \param a The peer address
  void connect (const Address & a)
    throw (SocketException);

  //! Disonnect from peer socket.
  void disconnect ()
    throw (SocketException);
    
  //! Could we read without blocking?
  //!
  //! \return True if there is some input data pending, false otherwise.
  bool can_read_without_blocking ()
    throw (SocketException);

  //! Wait till some data is available for reading or \c _tmo_msecs expires
  //!
  //! \param _tmo_msecs The timeout in milliseconds (0 means no timeout - return immediatly)
  //! \param _throw_exception Throw an exception in case \c _tmo_msecs expires
  bool wait_input_data (size_t _tmo_msecs, bool _throw_exception = true)
    throw (SocketException);
};  
  
} //-  namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/network/ClientSocket.i>
#endif // YAT_INLINE_IMPL

#endif //- _YAT_CLIENT_SOCKET_H_
