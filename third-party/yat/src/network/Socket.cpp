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
#if defined (WIN32)
# include <winsock2.h>
#else
# include <sys/socket.h>
# include <sys/errno.h>
# include <sys/time.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
#endif
#include <iostream>
#include <signal.h>
#include <sstream>
#include <yat/network/Socket.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/network/Socket.i>
#endif // YAT_INLINE_IMPL

// ============================================================================
// MISC PLATFORM SPECIFIC TYPDEFs and DEFINEs
// ============================================================================
#if defined(WIN32)
  typedef int socklen_t;
# define err_no WSAGetLastError()
# define SIGPIPE SIGABRT
# define INVALID_SOCKET_DESC INVALID_SOCKET 
#else
# define INVALID_SOCKET_DESC -1
# define err_no errno
#endif

// ============================================================================
// SHORTCUTs
// ============================================================================
#define GENERIC_SOCKET_ERROR yat::SoErr_Other
// ----------------------------------------------------------------------------
#define THROW_SOCKET_ERROR(C, T, O) \
    int yat_err_code = SocketException::native_to_yat_error(C); \
    OSStream _err_oss; \
    _err_oss << T \
             << " [" << SocketException::get_error_text(yat_err_code) << "]" \
             << std::ends; \
    throw yat::SocketException("SOCKET_ERROR", _err_oss.str().c_str(), O, C); \
// ----------------------------------------------------------------------------
#define CHECK_SOCK_DESC(OP) \
  if (this->m_os_desc == INVALID_SOCKET_DESC) \
  { \
    THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR, \
                       "attempt to use a invalid socket [check code]", \
                       OP); \
  }
// ----------------------------------------------------------------------------

namespace yat {

// ----------------------------------------------------------------------------
// Socket: static members
// ----------------------------------------------------------------------------
bool Socket::init_done = false;
// ----------------------------------------------------------------------------
size_t Socket::m_default_rd_buffer_size = DEFAULT_RD_BYTES;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Socket::init
// ----------------------------------------------------------------------------
void Socket::init ()
  throw (SocketException)
{
  if (Socket::init_done)
    return;

#ifdef WIN32
  //- windows hell: magic initialization call 
  WSADATA wsdata;
  if (::WSAStartup(MAKEWORD(2,2), &wsdata))
  {
    THROW_YAT_ERROR("YAT_INTERNAL_ERROR", 
                    "win32::socket2 library initialization failed", 
                    "Address::ns_lookup");
  }
#endif

  Socket::init_done = true;
}

// ----------------------------------------------------------------------------
// Socket::terminate
// ----------------------------------------------------------------------------
void Socket::terminate ()
  throw (SocketException)
{
  //-noop
}

// ----------------------------------------------------------------------------
// Socket::default_rd_buffer_size
// ----------------------------------------------------------------------------
void Socket::default_rd_buffer_size (size_t dbs)
{
  Socket::m_default_rd_buffer_size = dbs;
}

// ----------------------------------------------------------------------------
// Socket::Socket
// ----------------------------------------------------------------------------
Socket::Socket (Socket::Protocol _p) 
  : m_os_desc (INVALID_SOCKET_DESC), 
    m_buffer(0)
{
  YAT_TRACE("Socket::Socket");

  this->open(_p);
}

// ----------------------------------------------------------------------------
// Socket::~Socket
// ----------------------------------------------------------------------------
Socket::~Socket ()
{
  YAT_TRACE("Socket::~Socket");

  try
  {
    this->close();
  }
  catch (...)
  {
    //- ignore any error
  }
}

// ----------------------------------------------------------------------------
// Socket::open
// ----------------------------------------------------------------------------
void Socket::open (Socket::Protocol _p)
  throw (SocketException)
{
  YAT_TRACE("Socket::open");

  if (this->m_os_desc != INVALID_SOCKET_DESC)
    this->close();

  switch (_p)
  {
    case UDP_PROTOCOL:
      this->m_os_desc = static_cast<Socket::OSDescriptor>(::socket(AF_INET, SOCK_DGRAM, 0));
      break;
    case TCP_PROTOCOL:
      this->m_os_desc =  static_cast<Socket::OSDescriptor>(::socket(AF_INET, SOCK_STREAM, 0));
      break;
    default:
      THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR,  
                             "invalid protocol specified [should be UDP or TCP]", 
                             "Socket::open");
      break;
  }
  
  if (this->m_os_desc == INVALID_SOCKET_DESC) 
  {
    THROW_SOCKET_ERROR(err_no, 
                              "Socket descriptor instanciation failed [OS <socket> call failed]", 
                              "Socket::open");
  }
}

// ----------------------------------------------------------------------------
// Socket::close
// ----------------------------------------------------------------------------
void Socket::close ()
  throw (SocketException)
{
  YAT_TRACE("Socket::close");

  if (this->m_os_desc != INVALID_SOCKET_DESC) 
  {
#ifdef WIN32
    ::shutdown(this->m_os_desc, SD_BOTH);

    if (::closesocket(this->m_os_desc))
#else
    ::shutdown(this->m_os_desc, SHUT_RDWR);

    if (::close(this->m_os_desc))
#endif
    {
      THROW_SOCKET_ERROR(err_no, 
                                "OS <close> call failed",
                                "Socket::close");
    }
  }

  this->m_os_desc = INVALID_SOCKET_DESC;
}

// ----------------------------------------------------------------------------
// Socket::get_protocol
// ----------------------------------------------------------------------------
Socket::Protocol Socket::get_protocol () const
  throw (SocketException)
{
  YAT_TRACE("Socket::get_protocol");

  int p = static_cast<int>(this->get_option(SOCK_OPT_PROTOCOL_TYPE));

  return (p == SOCK_STREAM) ? TCP_PROTOCOL : UDP_PROTOCOL;
}

// ----------------------------------------------------------------------------
// Socket::get_address
// ----------------------------------------------------------------------------
Address Socket::get_address () const
  throw (SocketException)
{

  YAT_TRACE("Socket::get_address");

  CHECK_SOCK_DESC("Socket::get_address");

  struct sockaddr sa;

  socklen_t l = sizeof(sa);

  if (::getpeername(this->m_os_desc, &sa, &l)) 
  {
    THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR,
                           "OS <getpeername> call failed [unable to retrieve peer address]",
                           "Socket::get_address");
  }

  struct sockaddr_in * sai = reinterpret_cast<struct sockaddr_in*>(&sa);

  std::string a = inet_ntoa(sai->sin_addr);

  size_t p = ntohs(sai->sin_port);

  return Address(a, p);
}

// ----------------------------------------------------------------------------
// Socket::accept
// ----------------------------------------------------------------------------
Socket::OSDescriptor Socket::accept ()
  throw (SocketException)
{
  YAT_TRACE("Socket::accept");

  CHECK_SOCK_DESC("Socket::accept");

  struct sockaddr_in sa;
  socklen_t l = sizeof(sa);

  OSDescriptor d =  
    static_cast<Socket::OSDescriptor>(::accept(this->m_os_desc, reinterpret_cast<sockaddr*>(&sa), &l));

  if (d < 0)
  {
    THROW_SOCKET_ERROR(err_no,  
                       "OS <accept> call failed", 
                       "Socket::accept");
  }

  return d;
}

// ----------------------------------------------------------------------------
// Socket::bind
// ----------------------------------------------------------------------------
void Socket::bind (size_t _p)
  throw (SocketException)
{
  YAT_TRACE("Socket::bind");

  CHECK_SOCK_DESC("Socket::bind");

  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short)_p);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  ::memset(&sa.sin_zero, 0, 8);
  
  if (::bind(this->m_os_desc, reinterpret_cast<sockaddr*>(&sa), sizeof(sa))) 
  {
    OSStream oss;
    oss << "OS <bind> call failed for port "
        << _p
        << std::ends;
    THROW_SOCKET_ERROR(err_no, oss.str().c_str(), "Socket::bind");
  }
}

// ----------------------------------------------------------------------------
// Socket::connect
// ----------------------------------------------------------------------------
void Socket::connect (const Address & _addr)
  throw (SocketException)
{
  YAT_TRACE("Socket::connect");

  CHECK_SOCK_DESC("Socket::connect");

  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons((unsigned short)_addr.get_port_number());
  sa.sin_addr.s_addr = ::inet_addr(_addr.get_ip_address().c_str());
  ::memset(&sa.sin_zero, 0, 8);
  
  if (::connect(this->m_os_desc, reinterpret_cast<sockaddr*>(&sa), sizeof(sa))) 
  {
    OSStream oss;
    oss << "OS <connect> call failed for host "
        << _addr.get_ip_address()
        << ":"
        << _addr.get_port_number()
        << std::ends;

    THROW_SOCKET_ERROR(err_no, oss.str().c_str(), "Socket::connect");
  }
}

// ----------------------------------------------------------------------------
// Socket::select
// ----------------------------------------------------------------------------
bool Socket::select (size_t _tmo_msecs)
  throw (SocketException)
{
  YAT_TRACE("Socket::select");

  CHECK_SOCK_DESC("Socket::select");
 
  struct timeval tv = {0, 0};
  if (_tmo_msecs)
  {
    tv.tv_sec = (long)_tmo_msecs / 1000;
    tv.tv_usec = ((long)_tmo_msecs - (tv.tv_sec * 1000)) *  1000; 
  }
  
  fd_set sock_list;
  FD_ZERO(&sock_list);
  FD_SET(this->m_os_desc, &sock_list);
  
  int status = ::select(this->m_os_desc + 1, &sock_list, (fd_set*)0, (fd_set*)0, &tv);
  if (-1 == status)
  {
    THROW_SOCKET_ERROR(err_no, "OS <select> call failed", "Socket::select");
  }
  
  return status && FD_ISSET(this->m_os_desc, &sock_list);
}

// ----------------------------------------------------------------------------
// Socket::yat_to_native_option
// ----------------------------------------------------------------------------
int Socket::yat_to_native_option (Socket::Option _opt) const
  throw (SocketException)
{
  YAT_TRACE("Socket::yat_to_native_option");

  int native_opt;

  switch(_opt) 
  {
    case SOCK_OPT_KEEP_ALIVE: 
      native_opt = SO_KEEPALIVE; 
      break;
    case SOCK_OPT_LINGER: 
      native_opt = SO_LINGER; 
      break;
    case SOCK_OPT_NO_DELAY: 
      native_opt = TCP_NODELAY; 
      break;
    case SOCK_OPT_PROTOCOL_TYPE: 
      native_opt = SO_TYPE; 
      break;
    case SOCK_OPT_REUSE_ADDRESS: 
      native_opt = SO_REUSEADDR; 
      break;
    case SOCK_OPT_IBUFFER_SIZE: 
      native_opt = SO_RCVBUF; 
      break;
    case SOCK_OPT_OBUFFER_SIZE: 
      native_opt = SO_SNDBUF; 
      break;
    case SOCK_OPT_ITIMEOUT: 
      native_opt = SO_RCVTIMEO; 
      break;
    case SOCK_OPT_OTIMEOUT: 
      native_opt = SO_SNDTIMEO; 
      break;
    default: 
      THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR, 
                         "invalid socket option specified [programming error - check code]", 
                         "Socket::yat_to_native_option");
  }

  return native_opt;
}

// ----------------------------------------------------------------------------
// Socket::status
// ----------------------------------------------------------------------------
int Socket::status () const
  throw (SocketException)
{
  YAT_TRACE("Socket::status");

  CHECK_SOCK_DESC("Socket::get_option");
  
  int err_status;
  socklen_t l = sizeof(err_status);
  
  if (::getsockopt(this->m_os_desc, SOL_SOCKET, SO_ERROR, (char*)(&err_status), &l))
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <getsockopt> call failed", 
                              "Socket::status");
  }

  return SocketException::native_to_yat_error (err_status);
}

// ----------------------------------------------------------------------------
// Socket::option_level
// ----------------------------------------------------------------------------
int Socket::option_level (Option _opt) const
{
  return (_opt == Socket::SOCK_OPT_NO_DELAY) 
         ? IPPROTO_TCP 
         : SOL_SOCKET;
}

// ----------------------------------------------------------------------------
// Socket::get_sock_option (avoid to break strict-aliasing rules)
// ----------------------------------------------------------------------------
template <typename T> 
void _get_sock_option_ (Socket::OSDescriptor _socket_desc, 
                        int _level, 
                        int _opt,
                        T& dest_)
{
  socklen_t l = sizeof(T);
  
  ::memset(&dest_, 0, l);
  
  if (::getsockopt(_socket_desc, _level, _opt, (char*)&dest_, &l))
  {
    THROW_SOCKET_ERROR(err_no, 
                       "OS <getsockopt> call failed", 
                       "Socket::get_option");
  }
}
    
// ----------------------------------------------------------------------------
// Socket::get_option
// ----------------------------------------------------------------------------
int Socket::get_option (Socket::Option _opt) const
  throw (SocketException)
{
  YAT_TRACE("Socket::get_option");

  CHECK_SOCK_DESC("Socket::get_option");

  int level = this->option_level(_opt);
  
  int native_opt = this->yat_to_native_option(_opt);
  
  int opt_value;

  switch(_opt) 
  {
    case SOCK_OPT_KEEP_ALIVE: 
    case SOCK_OPT_NO_DELAY: 
    case SOCK_OPT_REUSE_ADDRESS: 
#ifdef WIN32
      {
        BOOL b;
        _get_sock_option_(this->m_os_desc, level, native_opt, b);
        opt_value = static_cast<int>(b);
      }
#else 
      {
        _get_sock_option_(this->m_os_desc, level, native_opt, opt_value);
      }
#endif
      break;
    case SOCK_OPT_IBUFFER_SIZE: 
    case SOCK_OPT_OBUFFER_SIZE: 
    case SOCK_OPT_PROTOCOL_TYPE:
      {
        _get_sock_option_(this->m_os_desc, level, native_opt, opt_value);
      }
      break;
    case SOCK_OPT_LINGER:
      {
        struct linger lp;
        _get_sock_option_(this->m_os_desc, level, native_opt, lp);
        if (lp.l_onoff) 
          opt_value = lp.l_linger;
        else 
          opt_value = -1;
      }
      break;
    case SOCK_OPT_ITIMEOUT: 
    case SOCK_OPT_OTIMEOUT:
#ifdef WIN32
      {
        //- on windows timeout is an int
        _get_sock_option_(this->m_os_desc, level, native_opt, opt_value);
      }
#else 
      //- on unix timeout is a timeval
      {
        struct timeval tv;
        _get_sock_option_(this->m_os_desc, level, native_opt, tv);
        opt_value = 1000 
                  * tv.tv_sec 
                  + static_cast<int>(static_cast<double>(tv.tv_usec) / 1000.);
      }
#endif
      break;
    default: 
      THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR, 
                             "invalid socket option specified [programming error - check code]", 
                             "Socket::get_option");
  }

  return opt_value;
}

// ----------------------------------------------------------------------------
// Socket::set_sock_option (avoid to break strict-aliasing rules)
// ----------------------------------------------------------------------------
template <typename T> 
void set_sock_option (Socket::OSDescriptor _socket_desc, 
                      int _level, 
                      int _opt,
                      T& _src)
{
  socklen_t l = sizeof(T);
  
  if (::setsockopt(_socket_desc, _level, _opt, (char*)&_src, l))
  {
    THROW_SOCKET_ERROR(err_no, 
                       "OS <setsockopt> call failed", 
                       "Socket::set_option");
  }
}

// ----------------------------------------------------------------------------
// Socket::set_option
// ----------------------------------------------------------------------------
void Socket::set_option (Socket::Option _opt, int _value)
  throw (SocketException)
{
  YAT_TRACE("Socket::set_option");

  CHECK_SOCK_DESC("Socket::set_option");

  int level = this->option_level(_opt);
            
  int native_opt = this->yat_to_native_option(_opt);
  
  switch(_opt) 
  {
     case SOCK_OPT_KEEP_ALIVE: 
    case SOCK_OPT_NO_DELAY: 
    case SOCK_OPT_REUSE_ADDRESS: 
#ifdef WIN32
      {
        BOOL b = _value ? true : false;
        set_sock_option(this->m_os_desc, level, native_opt, b);
      }
#else
      set_sock_option(this->m_os_desc, level, native_opt, _value);
#endif
      break;
    case SOCK_OPT_IBUFFER_SIZE: 
    case SOCK_OPT_OBUFFER_SIZE: 
      set_sock_option(this->m_os_desc, level, native_opt, _value);
      break;
    case SOCK_OPT_LINGER:
      {
        struct linger lp;
#ifdef WIN32
        lp.l_onoff = static_cast<unsigned short>(_value >= 0);
        lp.l_linger = static_cast<unsigned short>(_value >= 0 ? _value : 0);
#else
        lp.l_onoff = static_cast<int>(_value >= 0);
        lp.l_linger = static_cast<int>(_value >= 0 ? _value : 0);
#endif
        set_sock_option(this->m_os_desc, level, native_opt, _value);
      }
      break;
    case SOCK_OPT_ITIMEOUT: 
    case SOCK_OPT_OTIMEOUT:
      {
        if (_value < 0) _value = 0;
#ifdef WIN32
        // on windows, timeout is specified using an int
        set_sock_option(this->m_os_desc, level, native_opt, _value);
#else
        // on unix, timeout is specified using timeval structure
        struct timeval tv;
        tv.tv_sec = _value / 1000;
        tv.tv_usec = (_value - (tv.tv_sec * 1000)) * 1000;
        set_sock_option(this->m_os_desc, level, native_opt, tv);
#endif
      }
      break;
    default:
      THROW_SOCKET_ERROR(SoErr_OpNotSupported, 
                         "invalid or unsupported socket option", 
                         "Socket::set_option");
      break;
  }
}

// ----------------------------------------------------------------------------
// Socket::receive
// ----------------------------------------------------------------------------
size_t Socket::receive (char * ib, size_t nb)
  throw (SocketException)
{
  YAT_TRACE("Socket::receive [char*]");

  CHECK_SOCK_DESC("Socket::receive");

  size_t rb = 0;
  size_t total_rb = 0;
  bool was_interrupted = false;
  
  do
  {
    //- read some data
    rb = ::recv(this->m_os_desc, ib + total_rb, (int)(nb - total_rb), 0);
    //- in case of error/exception....
    if (rb <= 0)
    {
      //- connection may have been reset/closed by peer
      if (rb == 0) 
      {
        THROW_SOCKET_ERROR(SoErr_ConnectionClosed, 
                           "OS <recv> call failed", 
                           "Socket::receive");
      }
      //- operation may block caller (for non blocking socket)
      if (this->current_op_is_blocking())
      {
        THROW_SOCKET_ERROR(SoErr_WouldBlock, 
                               "OS <recv> call failed", 
                               "Socket::receive");
      }
      
#if ! defined(WIN32)
      if (errno == EINTR)
      {
        was_interrupted = true;
        continue;
      }
#else
      if (::WSAGetLastError() == WSAECONNRESET)
      {
        this->close();
        THROW_SOCKET_ERROR(SoErr_ConnectionClosed, 
                           "OS <recv> call failed", 
                           "Socket::receive");
      }
#endif
      //- reamining error cases
      THROW_SOCKET_ERROR(err_no, 
                         "OS <recv> call failed", 
                         "Socket::receive");
    }
    total_rb += rb;
  } 
  while (was_interrupted);
  
  YAT_LOG("Socket::receive::got " << rb << " bytes from peer");
  
  return total_rb;
}

// ----------------------------------------------------------------------------
// Socket::receive
// ----------------------------------------------------------------------------
size_t Socket::receive (Socket::Data & ib)
  throw (SocketException)
{
  YAT_TRACE("Socket::receive [Socket::Data]");

  CHECK_SOCK_DESC("Socket::receive");

  if (! ib.capacity())
  {
    THROW_SOCKET_ERROR(SoErr_Other,
                       "invalid yat::Buffer [destination buffer has a null capacity]",
                       "Socket::receive");
  }

  //- get some data from socket
  size_t rb = this->receive(ib.base(), ib.capacity());

  //- set buffer length ti actual number of bytes read
  ib.force_length(rb);

  return rb;
}

// ----------------------------------------------------------------------------
// Socket::receive
// ----------------------------------------------------------------------------
size_t Socket::receive (std::string & data_str)
  throw (SocketException)
{
  YAT_TRACE("Socket::receive [std::string]");

  CHECK_SOCK_DESC("Socket::receive");

  //- we may have to reallocate our local buffer
  try
  {
    this->m_buffer.capacity(Socket::m_default_rd_buffer_size);
  }
  catch (const Exception&)
  {
    THROW_SOCKET_ERROR(SoErr_OutOfMemory, 
                           "yat::Buffer reallocation failed", 
                           "Socket::receive");
  }
  catch (...)
  {
    THROW_SOCKET_ERROR(SoErr_OutOfMemory, 
                           "yat::Buffer reallocation failed", 
                           "Socket::receive");
  }
    
  //- clear buffer content
  this->m_buffer.clear();
  
  //- get some data from socket
  size_t rb = this->receive(this->m_buffer.base(), this->m_buffer.capacity());

  try
  {
    //- transfer data from local buffer to caller' std::string
    data_str.assign(this->m_buffer.base(), rb);
  }
  catch (const std::exception &)
  {
    THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR, 
                           "std::exception caught [std::string error]", 
                           "Socket::receive");
  }
  catch (...)
  {
    THROW_SOCKET_ERROR(GENERIC_SOCKET_ERROR, 
                           "unknown exception caught [from std::string member function]", 
                           "Socket::receive");
  }
  
  return rb;
}

// ----------------------------------------------------------------------------
// Socket::send
// ---------------------------------------------------------------------------- 
void Socket::send (const char * ob, size_t nb)
  throw (SocketException)
{
  YAT_TRACE("Socket::send");

  CHECK_SOCK_DESC("Socket::send [char*]");

  //- remaining num. of bytes to be read
  size_t btr = nb;

  //- send data
  while (btr) 
  {
    int sb = ::send(this->m_os_desc, ob + nb - btr, (int)btr, 0);
    if (sb <= 0) 
    {
      //- operation may block caller (for non blocking socket)
      if (this->current_op_is_blocking())
      {
        THROW_SOCKET_ERROR(SoErr_WouldBlock, 
                               "OS <recv> call failed", 
                               "Socket::send");
      }
#if ! defined(WIN32)
      //- under linux, the send call may be interrupted by an "external event"
      if (errno == EINTR)
        continue;
#else
      if (::WSAGetLastError() == WSAECONNRESET)
      {
        this->close();
        THROW_SOCKET_ERROR(SoErr_ConnectionClosed, 
                               "OS <send> call failed", 
                               "Socket::send");
      }
#endif
      //- reamining error cases
      THROW_SOCKET_ERROR(err_no, 
                                "OS <send> call failed", 
                                "Socket::send");
    }
    //- cumulate read bytes
    btr -= sb;
  }
}

// ----------------------------------------------------------------------------
// Socket::send
// ---------------------------------------------------------------------------- 
void Socket::send (const Socket::Data & ob)
  throw (SocketException)
{
  YAT_TRACE("Socket::send [Socket::Data]");

  CHECK_SOCK_DESC("Socket::send");
  
  //- send data
  this->send(ob.base(), ob.length());
}
      
// ----------------------------------------------------------------------------
// Socket::send
// ---------------------------------------------------------------------------- 
void Socket::send (const std::string & os)
  throw (SocketException)
{
  YAT_TRACE("Socket::send [std::string]");

  CHECK_SOCK_DESC("Socket::send");

  //- send data
  this->send(os.c_str(), os.size());
}

// ----------------------------------------------------------------------------
// Socket::set_blocking_mode
// ----------------------------------------------------------------------------
void Socket::set_blocking_mode ()
  throw (SocketException)
{
  YAT_TRACE("Socket::set_blocking_mode");

  CHECK_SOCK_DESC("Socket::set_blocking_mode");

#if defined(WIN32)
  unsigned long n = 0;
  if (::ioctlsocket(this->m_os_desc, FIONBIO, &n))
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <ioctlsocket> call failed", 
                              "Socket::set_blocking_mode");
  }
#else
  long flags = ::fcntl(this->m_os_desc, F_GETFL);
  if (flags == -1)
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <fcntl::F_GETFL> call failed", 
                              "Socket::set_blocking_mode"); 
  }
  flags &= ~O_NONBLOCK;
  if (::fcntl(this->m_os_desc, F_SETFL, flags))
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <fcntl::F_SETFL> call failed", 
                              "Socket::set_blocking_mode");
  }
#endif
}

// ----------------------------------------------------------------------------
// Socket::set_non_blocking_mode
// ----------------------------------------------------------------------------
void Socket::set_non_blocking_mode ()
  throw (SocketException)
{
  YAT_TRACE("Socket::set_non_blocking_mode");

  CHECK_SOCK_DESC("Socket::set_non_blocking_mode");

#if defined(WIN32)
  unsigned long n = 1;
  if (::ioctlsocket(this->m_os_desc, FIONBIO, &n ))
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <ioctlsocket> call failed", 
                              "Socket::set_non_blocking_mode");
  }
#else
  long flags = ::fcntl(this->m_os_desc, F_GETFL);
  if (flags == -1)
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <fcntl::F_GETFL> call failed", 
                              "Socket::set_non_blocking_mode"); 
  }
  flags |= O_NONBLOCK;
  if (::fcntl(this->m_os_desc, F_SETFL, flags))
  {
    THROW_SOCKET_ERROR(err_no, 
                              "OS <fcntl::F_SETFL> call failed", 
                              "Socket::set_non_blocking_mode");
  }
#endif
}

// ----------------------------------------------------------------------------
// Socket::current_op_is_blocking
// ----------------------------------------------------------------------------
bool Socket::current_op_is_blocking ()
{
#if defined(WIN32)
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    switch (errno)
    {
      case EWOULDBLOCK:
        return true;
        break;
      default:
        break;
    }
    return false;
#endif
}

} //- namespace yat

