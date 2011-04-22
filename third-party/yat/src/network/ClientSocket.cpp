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
#include <yat/network/ClientSocket.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/network/ClientSocket.i>
#endif // YAT_INLINE_IMPL


namespace yat {

// ----------------------------------------------------------------------------
// ClientSocket::ClientSocket
// ----------------------------------------------------------------------------
ClientSocket::ClientSocket (Socket::Protocol _p) 
  : Socket(_p)

{
  YAT_TRACE("ClientSocket::ClientSocket");
}

// ----------------------------------------------------------------------------
// ClientSocket::~ClientSocket
// ----------------------------------------------------------------------------
ClientSocket::~ClientSocket ()
{
  YAT_TRACE("ClientSocket::~ClientSocket");

  this->close();
}

// ----------------------------------------------------------------------------
// ClientSocket::bind
// ----------------------------------------------------------------------------
void ClientSocket::bind (size_t _p)
  throw (SocketException)
{
  YAT_TRACE("ClientSocket::bind");

  this->Socket::bind(_p);
}

// ----------------------------------------------------------------------------
// ClientSocket::connect
// ----------------------------------------------------------------------------
void ClientSocket::connect (const Address & _addr)
  throw (SocketException)
{
  YAT_TRACE("ClientSocket::connect");

  this->Socket::connect(_addr);
}

// ----------------------------------------------------------------------------
// ClientSocket::disconnect
// ----------------------------------------------------------------------------
void ClientSocket::disconnect ()
  throw (SocketException)
{
  YAT_TRACE("ClientSocket::disconnect");

  this->close();
}

// ----------------------------------------------------------------------------
// ClientSocket::can_read_without_blocking
// ----------------------------------------------------------------------------
bool ClientSocket::can_read_without_blocking ()
  throw (SocketException)
{
  YAT_TRACE("ClientSocket::can_read_without_blocking");

  return this->select(0);
}

// ----------------------------------------------------------------------------
// ClientSocket::wait_input_data
// ----------------------------------------------------------------------------
bool ClientSocket::wait_input_data (size_t _tmo, bool _throw)
  throw (SocketException)
{
  YAT_TRACE("ClientSocket::wait_input_data");
 
  if (! this->select(_tmo))
  {
    if (! _throw)
      return false;

    throw yat::SocketException("SOCKET_ERROR", 
                               SocketException::get_error_text(SoErr_TimeOut), 
                               "ClientSocket::wait_input_data", 
                               SocketException::yat_to_native_error(SoErr_TimeOut)); 
  } 
  
  return true;
}

} //- namespace yat

