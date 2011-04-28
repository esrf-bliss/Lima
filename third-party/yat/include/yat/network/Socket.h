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

#ifndef _YAT_SOCKET_H_
#define _YAT_SOCKET_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/memory/DataBuffer.h>
#include <yat/network/Address.h>
#include <yat/network/SocketException.h>

// ============================================================================
// CONSTs
// ============================================================================
#define DEFAULT_RD_BYTES 512

namespace yat { 
  
// ----------------------------------------------------------------------------
//! The YAT Socket class
// ----------------------------------------------------------------------------
class YAT_DECL Socket
{
  //! This is the yat network socket class.
  //!
  //! Base class for both TCPClientSocket and UDPClientSocket.

public:
  //! OS socket descriptor.
  typedef int OSDescriptor;

  //! Create a dedicated type for Socket I/O data.
  typedef Buffer<char> Data;

  //! Supported socket protocols.
  enum Protocol 
  {
    //! Transfer Control Protocol.
    TCP_PROTOCOL,
    //! User Datagram Protocol.
    UDP_PROTOCOL
  };
    
  //! Socket options.
  enum Option 
  {
    //- keep connection alive
    SOCK_OPT_KEEP_ALIVE,
    //- time to linger on close (in seconds)
    SOCK_OPT_LINGER, 
    //- disable the Nagle algorithm for packet coalescing
    SOCK_OPT_NO_DELAY,
    //- socket protocol type
    SOCK_OPT_PROTOCOL_TYPE,
    //- allow reuse of a TCP address without delay
    SOCK_OPT_REUSE_ADDRESS,
    //- size of receive buffer (in bytes)
    SOCK_OPT_IBUFFER_SIZE,
    //- time out period for receive operations (in seconds)
    SOCK_OPT_ITIMEOUT,
    //- size of send buffer (in bytes)
    SOCK_OPT_OBUFFER_SIZE,
    //- time out period for send operations (in seconds)
    SOCK_OPT_OTIMEOUT
  };
    
  //! YAT Socket internal intialization cooking.
  //! Must be called prior to any other yat::Socket or yat::Address call.
  //! Call this from your main function or dll entry point.  
  static void init ()
    throw (SocketException);

  //! YAT Socket internal termination cooking.
  //! Must be called prior to any other yat::Socket or yat::Address call.
  //! Call this at the end of your main function or dll exit point.  
  static void terminate ()
    throw (SocketException);

  //! Returns socket error status
  //! See SocketException::SocketError enum for returned values
  int status () const
    throw (SocketException);
  
  //! Return the socket protocols.
  Protocol get_protocol () const
    throw (SocketException);
      
  //! Return address the associated addr.
  Address get_address () const
    throw (SocketException);
      
  //! Return current value of the specified socket option.
  //!
  //! \param op The option name
  int get_option (Socket::Option op) const
    throw (SocketException);

  //! Switch socket I/O to blocking mode.
  void set_blocking_mode ()
    throw (SocketException);

  //! Switch socket I/O to non-blocking mode.
  void set_non_blocking_mode ()
    throw (SocketException);

  //! Change the value of the specified socket option.
  //!
  //! \param op The option name
  //! \param value New value for \c op
  //! \remark 
  //!  Most option associated to booleans. Some execeptions...
  //!  SOCK_OPT_ITIMEOUT and SOCK_OPT_OTIMEOUT should be specified in millseconds.
  void set_option (Socket::Option op, int value = 0)
    throw (SocketException);
  
  //! Receive (i.e. read) data from the socket.
  //!
  //! \param ib The buffer in which the data will be placed.
  //! \param nb The number of bytes to read. 
  //! \return the actual number of bytes received.
  size_t receive (char * ib, size_t nb)
    throw (SocketException);

  //! Receive (i.e. read) data from the socket.
  //!
  //! \param ib The buffer in which the data will be placed.
  //! \return the actual number of bytes received.
  //! \remarks 
  //!  Reads up to ib.capacity() bytes on the socket. Upon return
  //!  ib.length() will give the actual number of bytes read.
  size_t receive (Socket::Data & ib)
    throw (SocketException);
      
  //! Receive (i.e. read) data from the socket.
  //!
  //! \param is The string in which the data will be placed.
  //! \return the actual number of bytes received.
  //! \remarks Read up to \c Socket::MAX_RD_BYTES bytes.
  //! Upon return the string size will equal the actual number of bytes read. 
  size_t receive (std::string & is)
    throw (SocketException);

  //! Receive (i.e. read) data from the socket.
  //!
  //! \param ib The buffer in which the data will be placed.
  //!
  //! \remarks Read \c ib.length() bytes from the socket. 
  size_t operator>> (Socket::Data & ib)
    throw (SocketException);

  //! Receive (i.e. read) data from the socket.
  //!
  //! \param is The string in which the data will be placed.
  //!
  //! \remarks Read up to \c Socket::MAX_RD_BYTES bytes.
  //! Upon return the string size will equal the actual number of bytes read. 
  size_t operator>> (std::string& is)
    throw (SocketException);

  //! Send (i.e. write) data to the socket.
  //!
  //! \param ob The buffer containing the data to be sent.
  //! \param nb The number of bytes to write.
  void send (const char * ob, size_t nb)
    throw (SocketException);

  //! Send (i.e. write) data to the socket.
  //!
  //! \param ob The buffer containing the data to be sent.
  //!
  //! \remarks 
  //! Write ob.length() bytes onto the socket.
  void send (const Socket::Data & ob)
    throw (SocketException);
      
  //! Send (i.e. write) data to the socket.
  //!
  //! \param is The string containing the data to be sent.
  void send (const std::string & os)
    throw (SocketException);

  //! Send (i.e. write) data to the socket.
  //!
  //! \param ob The buffer containing the data to be sent.
  //! \remarks Send \c ob.length() bytes on the socket.
  void operator<< (const Socket::Data & ob)
    throw (SocketException);

  //! Send (i.e. write) data to the socket.
  //!
  //! \param os The string containing the data to be sent.
  //! \remarks Send \c os.size() bytes on the socket.
  void operator<< (const std::string& os)
    throw (SocketException);
  
  //! Set default read buffer size (in bytes)
  //!
  static void default_rd_buffer_size (size_t dbs);

protected:
  //! Construct new socket
  //! 
  //! \param p The associated protocol. Defaults to \c yat::TCP_PROTOCOL
  //!
  //! \remarks May throw an Exception.
  explicit Socket (Protocol p = TCP_PROTOCOL);
      
  //! Release any allocated resource.
  virtual ~Socket ();

  //! Instanciate the underlying OS socket descriptor.
  //! Called from Socket::Socket (ctor).
  //!
  //! \param p The associated protocol.
  //!
  //! \remarks This method allows to change the associated protocol at runtime.
  void open (Protocol p)
    throw (SocketException);

  //! Close the underlying OS socket descriptor.
  void close ()
    throw (SocketException);

  //! Accept connection on bound port.
  OSDescriptor accept ()
    throw (SocketException);
      
  //! Connect to peer socket.
  //!
  //! \param a The peer address 
  void connect (const Address & a)
    throw (SocketException);

  //! Bind the socket to the specified port
  //!
  //! \param p The port on which the socket is to be binded 
  //! 
  //! \remarks
  //! May fail if port \c p is already reserved by another process or thread. 
  //! It may also fail after the connection is released (see option SOCK_OPT_REUSE_ADDRESS).
  void bind (size_t p)
    throw (SocketException);
      
  //! Wait for activity on the socket (tmo in milliseconds)
  //!
  //! \param _tmo_msecs The timeout in milliseconds (0 means no timeout - return immediatly)
  //! 
  //! \return True if there is some activity on the socket before tmo expires
  bool select (size_t _tmo_msecs = 0)
    throw (SocketException);
    
  //! Given a yat socket option, returns the associated native socket option.
  //!
  //! \param o yat socket option
  int yat_to_native_option (Option o) const
    throw (SocketException);
    
  //! Given a yat socket option, returns its associated level.
  //!
  //! \param o yat socket option
  int option_level (Option o) const;

private:
  //! The underlying OS socket descriptor.
  OSDescriptor m_os_desc;

  //! Return true is the current operation would block, returns 
  //! false otherwise.
  bool current_op_is_blocking ();

  //! Data buffer used in some send/receive operations. 
  Socket::Data m_buffer;

  //! Init done flag
  static bool init_done;

  //! Default read buffer size
  static size_t m_default_rd_buffer_size;

  //! Not implemented private member
  Socket (const Socket&);
  //! Not implemented private member
  Socket& operator= (const Socket&);
};  
  
} //-  namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/network/Socket.i>
#endif // YAT_INLINE_IMPL

#endif //- _YAT_SOCKET_H_
