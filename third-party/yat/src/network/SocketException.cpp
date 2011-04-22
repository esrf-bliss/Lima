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
#if ! defined(WIN32)
# include <sys/errno.h>
#elif defined (WIN32_LEAN_AND_MEAN)
# include <winsock2.h>  
#endif
#include <yat/CommonHeader.h>
#include <yat/network/SocketException.h>

namespace yat {

// ============================================================================
// SocketException::SocketException
// ============================================================================
SocketException::SocketException (
                                  const char *r,
                                  const char *d,
                                  const char *o, 
                                  int nec,
                                  int sv
                                 )
 : Exception(r ,d, o, nec, sv)
{
  YAT_TRACE("SocketException::SocketException");
  
  this->m_yat_err_code = SocketException::native_to_yat_error(nec);
}


// ============================================================================
// SocketException::SocketException
// ============================================================================
SocketException::SocketException (
                                  const std::string& r,
                                  const std::string& d,
                                  const std::string& o, 
                                  int nec, 
                                  int sv
                                 )
 : Exception(r ,d, o, nec, sv)
{
  YAT_TRACE("SocketException::SocketException");
  
  this->m_yat_err_code = SocketException::native_to_yat_error(nec);
}

// ============================================================================
// SocketException::SocketException
// ============================================================================
SocketException::~SocketException ()
{
  YAT_TRACE("SocketException::~SocketException");
}

// ============================================================================
// SocketException::code
// ============================================================================
int SocketException::code () const
{
  YAT_TRACE("SocketException::code");
  
  return this->m_yat_err_code;
}

// ============================================================================
// SocketException::is_a
// ============================================================================
bool SocketException::is_a (int _code) const
{
  YAT_TRACE("SocketException::code");
  
  return this->m_yat_err_code == _code;
}

// ============================================================================
// SocketException::text
// ============================================================================
std::string SocketException::text () const
{
  return SocketException::get_error_text(this->m_yat_err_code);
}

// ============================================================================
// SocketException::dump
// ============================================================================
void SocketException::dump () const
{
  YAT_LOG("-- yat::SocketException ----------------------");
  
  YAT_LOG("\tInitial error: " << SocketException::get_error_text(this->m_yat_err_code));
  
  for (size_t e = 0; e < this->errors.size(); e++)
  {
    YAT_LOG("\tErr[" 
            << e << "]:reason..." 
            << this->errors[e].reason); 
              
    YAT_LOG("\tErr[" 
            << e << "]:desc....." 
            << this->errors[e].desc); 
              
              
    YAT_LOG("\tErr[" 
            << e 
            << "]:desc....." 
            << this->errors[e].origin); 
              
              
    YAT_LOG("\tErr[" 
            << e 
            << "]:code....." 
            << this->errors[e].code); 
              
  }
  
  YAT_LOG("----------------------------------------------");             
}

// ============================================================================
// SocketException::native_to_yat_error
// ============================================================================
SocketError SocketException::native_to_yat_error (int _os_err_code)
{
  SocketError os_native_err;

  switch (_os_err_code)
  {
#ifdef WIN32
    case WSAEACCES:
      os_native_err = SoErr_PrivilegedPort; 
      break;
    case WSAEADDRINUSE:
      os_native_err = SoErr_AddressInUse; 
      break;
    case WSAEADDRNOTAVAIL: 
      os_native_err = SoErr_AddressNotAvailable; 
      break;
    case WSAECONNREFUSED: 
      os_native_err = SoErr_ConnectionRefused; 
      break;
    case WSAECONNRESET: 
      os_native_err = SoErr_ConnectionClosed; 
      break;
    case WSAEHOSTDOWN: 
      os_native_err = SoErr_ConnectionRefused; 
      break;
    case WSAEHOSTUNREACH: 
      os_native_err = SoErr_ConnectionRefused; 
      break;
    case WSAEINTR: 
      os_native_err = SoErr_ConnectionClosed; 
      break;
    case WSAEISCONN: 
      os_native_err = SoErr_IsConnected; 
      break;
    case WSAEMSGSIZE: 
      os_native_err = SoErr_DatagramTooLong; 
      break;
    case WSAENETRESET: 
      os_native_err = SoErr_ConnectionClosed; 
      break;
    case WSAENOPROTOOPT: 
      os_native_err = SoErr_InvalidOption; 
      break;
    case WSAENOTCONN: 
      os_native_err = SoErr_NotConnected; 
      break;
    case WSANOTINITIALISED: 
      os_native_err = SoErr_BadDescriptor; 
      break;
    case WSAENOTSOCK: 
      os_native_err = SoErr_BadDescriptor; 
      break;
    case WSAEOPNOTSUPP: 
      os_native_err = SoErr_OpNotSupported; 
      break;
    case WSAESHUTDOWN: 
      os_native_err = SoErr_BadDescriptor; 
      break;
    case WSAETIMEDOUT: 
      os_native_err = SoErr_TimeOut; 
      break;
#else
    case 0:
      os_native_err = SoErr_NoError; 
      break;
    case EFAULT:
      os_native_err = SoErr_BadMemAddress; 
      break;
    case EACCES: 
      os_native_err = SoErr_PrivilegedPort; 
      break;
    case EADDRINUSE: 
      os_native_err = SoErr_AddressInUse; 
      break;
    case EADDRNOTAVAIL: 
      os_native_err = SoErr_AddressNotAvailable; 
      break;
    case EWOULDBLOCK:
      os_native_err = SoErr_WouldBlock; 
      break;
    case EBADF: 
      os_native_err = SoErr_BadDescriptor; 
      break;
    case ECONNREFUSED: 
      os_native_err = SoErr_ConnectionRefused; 
      break;
    case ECONNRESET: 
      os_native_err = SoErr_ConnectionClosed; 
      break;
    case EINTR: 
      os_native_err = SoErr_OSInterrupt; 
      break;
    case EISCONN: 
      os_native_err = SoErr_IsConnected; 
      break;
    case EMSGSIZE: 
      os_native_err = SoErr_DatagramTooLong; 
      break;
    case ENOPROTOOPT: 
      os_native_err = SoErr_InvalidOption; 
      break;
    case ENOTCONN: 
      os_native_err = SoErr_NotConnected; 
      break;
    case ENOTSOCK: 
      os_native_err = SoErr_BadDescriptor; 
      break;
    case EPIPE: 
      os_native_err = SoErr_ConnectionClosed; 
      break;
    case ETIMEDOUT: 
      os_native_err = SoErr_TimeOut; 
      break;
    case EINPROGRESS:
      os_native_err = SoErr_InProgress; 
      break;
#endif
    default: 
      os_native_err = SoErr_Other;
      break;
  }

  return os_native_err;
}

// ============================================================================
// SocketException::yat_to_native_error
// ============================================================================
int SocketException::yat_to_native_error (SocketError _yat_err_code)
{
  int os_native_err;

  switch (_yat_err_code)
  {
#ifdef WIN32
    case SoErr_PrivilegedPort:
      os_native_err = WSAEACCES; 
      break;
    case SoErr_AddressInUse:
      os_native_err = WSAEADDRINUSE; 
      break;
    case SoErr_AddressNotAvailable: 
      os_native_err = WSAEADDRNOTAVAIL; 
      break;
    case SoErr_ConnectionRefused: 
      os_native_err = WSAECONNREFUSED; 
      break;
    case SoErr_ConnectionClosed: 
      os_native_err = WSAECONNRESET; 
      break;
    case SoErr_IsConnected: 
      os_native_err = WSAEISCONN; 
      break;
    case SoErr_DatagramTooLong: 
      os_native_err = WSAEMSGSIZE; 
      break;
    case SoErr_InvalidOption: 
      os_native_err = WSAENOPROTOOPT; 
      break;
    case SoErr_NotConnected: 
      os_native_err = WSAENOTCONN; 
      break;
    case SoErr_BadDescriptor: 
      os_native_err = WSAENOTSOCK; 
      break;
    case SoErr_OpNotSupported: 
      os_native_err = WSAEOPNOTSUPP; 
      break;
    case SoErr_TimeOut: 
      os_native_err = WSAETIMEDOUT; 
      break;
#else
    case SoErr_NoError:
      os_native_err = 0; 
      break;
    case SoErr_BadMemAddress:
      os_native_err = EFAULT; 
      break;
    case SoErr_PrivilegedPort: 
      os_native_err = EACCES; 
      break;
    case SoErr_AddressInUse: 
      os_native_err = EADDRINUSE; 
      break;
    case SoErr_AddressNotAvailable: 
      os_native_err = EADDRNOTAVAIL; 
      break;
    case SoErr_WouldBlock:
      os_native_err = EWOULDBLOCK; 
      break;
    case SoErr_BadDescriptor: 
      os_native_err = ENOTSOCK;
      break;
    case SoErr_ConnectionRefused: 
      os_native_err =  ECONNREFUSED; 
      break;
    case SoErr_ConnectionClosed: 
      os_native_err = ECONNRESET; 
      break;
    case SoErr_OSInterrupt: 
      os_native_err = EINTR; 
      break;
    case SoErr_IsConnected: 
      os_native_err = EISCONN; 
      break;
    case SoErr_DatagramTooLong: 
      os_native_err = EMSGSIZE; 
      break;
    case SoErr_InvalidOption: 
      os_native_err = ENOPROTOOPT; 
      break;
    case SoErr_NotConnected: 
      os_native_err = ENOTCONN; 
      break;
    case SoErr_TimeOut: 
      os_native_err = ETIMEDOUT; 
      break;
    case SoErr_InProgress:
      os_native_err = EINPROGRESS; 
      break;
#endif
    default: 
      os_native_err = 0;
      break;
  }

  return os_native_err;
}

// ============================================================================
// SocketException::get_error_text
// ============================================================================
std::string SocketException::get_error_text (int _err)
{
  std::string txt;

  switch (_err)
  {
    //- No Error.
    case SoErr_NoError:
      txt = "no error";
      break;
    //- The receive buffer pointer(s) point outside the processes address space
    case SoErr_BadMemAddress:
      txt = "receive buffer points outside the processes address space <SoErr_BadMemAddress>";
      break;
    //- Address is already in use (bind & connect).
    case SoErr_AddressInUse:
      txt = "address already in use <SoErr_AddressInUse>";
      break;
    //- Address not available on machine (bind & connect).
    case SoErr_AddressNotAvailable:
      txt = "address not available <SoErr_AddressNotAvailable>";
      break;
    //- Invalid socket descriptor <Socket).
    case SoErr_BadDescriptor:
      txt = "invalid socket descriptor <SoErr_BadDescriptor>";
      break; 
    //- Message signature is invalid.
    case SoErr_BadMessage:
      txt = "message signature is invalid <SoErr_BadMessage>";
      break;
    //- Connection was closed (or broken) by other party.
    case SoErr_ConnectionClosed:
      txt = "connection closed by peer <SoErr_ConnectionClosed>";
      break;
    //- Connection refused by server.
    case SoErr_ConnectionRefused:
      txt = "connection refused <SoErr_ConnectionRefused>";
      break; 
    //- Datagram too long to send atomically.
    case SoErr_DatagramTooLong:
      txt = "UDP datagram too long <SoErr_DatagramTooLong>";
      break; 
    //- Invalid option for socket protocol.
    case SoErr_InvalidOption:
      txt = "invalid option for socket protocol <SoErr_InvalidOption>";
      break; 
    //- %Socket is already connected.
    case SoErr_IsConnected:
      txt = "socket is already connected <SoErr_IsConnected>"; 
      break; 
    //- %Socket is not connected.
    case SoErr_NotConnected:
      txt = "socket is not connected <SoErr_NotConnected>"; 
      break; 
    //- Operation is not supported for this socket.
    case SoErr_OpNotSupported:
      txt = "operation not supported <SoErr_OpNotSupported>"; 
      break;
    //- User does not have access to privileged ports (bind).
    case SoErr_PrivilegedPort:
      txt = "privileged port - access denied <SoErr_PrivilegedPort>"; 
      break; 
    //- Time out was reached for operation (receive & send).
    case SoErr_TimeOut:
      txt = "timeout expired <SoErr_TimeOut>"; 
      break; 
    //- Current operation is blocking (non-blocking socket)
    case SoErr_WouldBlock:
      txt = "operation would block <SoErr_WouldBlock>"; 
      break;  
    //- Op. in progress (non-blocking socket)
    case SoErr_InProgress:
      txt = "operation in progress <SoErr_InProgress>"; 
      break;  
    //- Op. interrupted by OS event (signal) 
    case SoErr_OSInterrupt:
      txt = "operation interrupted by OS event <SoErr_OSInterrupt>"; 
      break; 
    //- Memory error
    case SoErr_OutOfMemory:
      txt = "memory allocation failed <SoErr_OutOfMemory>"; 
      break; 
    //- Any other OS specific error.
    default:
    case SoErr_Other:
      txt = "unknown or generic socket error <SoErr_Other>"; 
      break; 
  }

  return txt;
}

} //- namespace
