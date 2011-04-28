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

#ifndef _YAT_TIMER_H_
#define _YAT_TIMER_H_

#include <yat/CommonHeader.h>


// ============================================================================
// TIME MACROS
// ============================================================================
#include <ctime>
#include <limits>
#if defined (YAT_WIN32)
# include <sys/timeb.h>
# include <time.h>
#else
# include <sys/time.h>
#endif

typedef struct _timeval
{
  int tv_sec;
  int tv_usec;
  time_t tv_utc;
  _timeval ()
    : tv_sec(0), tv_usec(0), tv_utc(0) {}
} _timeval;
# define _TIMESTAMP _timeval

#if defined (YAT_WIN32)
  typedef struct
  {
    int tv_sec;
    int tv_nsec;
  } _timespec;
# define _TIMESPEC _timespec
#else
# define _TIMESPEC timespec
#endif

#if defined (YAT_WIN32)  
# define _GET_TIME(T) \
  do \
  { \
    struct _timeb _timeb_now; \
    ::_ftime(&_timeb_now); \
    T.tv_sec = static_cast<int>(_timeb_now.time); \
    T.tv_usec = static_cast<int>(1000 * _timeb_now.millitm); \
    ::time(&T.tv_utc); \
  } while (0)
#else
# define _GET_TIME(T) \
  do \
  { \
    struct timeval _now; \
    ::gettimeofday(&_now, 0); \
    T.tv_sec = _now.tv_sec; \
    T.tv_usec = _now.tv_usec; \
    ::time(&T.tv_utc); \
  } while (0)
#endif

# define _MAX_DATE_LEN 256
# define _TIMESTAMP_TO_DATE(T,S) \
  do \
  { \
    struct tm * tmv = ::localtime(&T.tv_utc); \
    char b[_MAX_DATE_LEN]; \
    ::memset(b, 0, _MAX_DATE_LEN); \
    ::strftime(b, _MAX_DATE_LEN, "%a, %d %b %Y %H:%M:%S", tmv); \
    S = std::string(b); \
  } while (0)
# define  _RESET_TIMESTAMP(T) \
  do \
  { \
    T.tv_sec = 0; \
    T.tv_usec = 0; \
    T.tv_utc = 0; \
  } while (0)
# define  _COPY_TIMESTAMP(S, D) \
  do \
  { \
    D.tv_sec = S.tv_sec; \
    D.tv_usec = S.tv_usec; \
    D.tv_utc = S.tv_utc; \
  } while (0)
  
  
#define  _ELAPSED_SEC(B, A) \
  static_cast<double>((A.tv_sec - B.tv_sec) + (1.E-6 * (A.tv_usec - B.tv_usec)))

#define  _ELAPSED_MSEC(B, A) _ELAPSED_SEC(B, A) * 1.E3

#define  _ELAPSED_USEC(B, A) _ELAPSED_SEC(B, A) * 1.E6

#define _IS_VALID_TIMESTAMP(T) T.tv_sec != 0 || T.tv_usec != 0

#define  _TMO_EXPIRED(B, A, TMO) _ELAPSED_SEC (B, A) > TMO


#if defined (YAT_WIN32)
  namespace std 
  { 
    using ::clock_t; 
    using ::clock; 
  }
#endif

namespace yat
{
  typedef _TIMESTAMP Timestamp;
  typedef _TIMESPEC  Timespec;

#if ! defined (YAT_WIN32) 
 
// ============================================================================
// class Timer
// ============================================================================
class Timer
{
public:
	//- instanciate then resets the Timer
  Timer () 
  { 
    this->restart();
  } 
  
  //- resets the Timer
  inline void restart() 
  {
    ::gettimeofday(&_start_time, NULL); 
  }

  //- returns elapsed time in seconds
  inline double elapsed_sec ()             
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return (now.tv_sec - _start_time.tv_sec) + 1e-6 * (now.tv_usec - _start_time.tv_usec);
  }

  //- returns elapsed time in milliseconds
  inline double elapsed_msec ()             
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return 1e3 * (now.tv_sec - _start_time.tv_sec) + 1e-3 * (now.tv_usec - _start_time.tv_usec);
  }

  //- returns elapsed time in microseconds
  inline double elapsed_usec ()              
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return 1e6 * (now.tv_sec - _start_time.tv_sec) + (now.tv_usec - _start_time.tv_usec);
  }

private:
  struct timeval _start_time;
};

#else // ! YAT_WIN32

// ============================================================================
// class: Timer
// ============================================================================
class Timer 
{
public:
  //- instanciates/resets the Timer
  Timer () 
  {
    _start.QuadPart = 0;
    _stop.QuadPart = 0;
    ::QueryPerformanceFrequency(&_frequency);
    ::QueryPerformanceCounter(&_start);
  }
  
  //- resets the Timer
  inline void restart() 
  {
    ::QueryPerformanceCounter(&_start) ;
  }
  
  //- returns elapsed time in seconds
  inline double elapsed_sec ()             
  { 
    ::QueryPerformanceCounter(&_stop);
    LARGE_INTEGER dt;
    dt.QuadPart = _stop.QuadPart - _start.QuadPart;
    return li_to_secs(dt);
  }

  //- returns elapsed time in milliseconds
  inline double elapsed_msec ()           
  { 
    return  1.E3 * elapsed_sec (); 
  }

  //- returns elapsed time in microseconds
  inline double elapsed_usec ()            
  { 
    return  1.E6 * elapsed_sec (); 
  }
  
private:
  LARGE_INTEGER _start;
  LARGE_INTEGER _stop;
  LARGE_INTEGER _frequency;
  inline double li_to_secs ( LARGE_INTEGER & li) {
    return (double)li.QuadPart / (double)_frequency.QuadPart;
  }
};

#endif // ! YAT_WIN32

// ============================================================================
// class: Timeout
// ============================================================================
class Timeout 
{
public:
  //- Timeout value type
  typedef double TimeoutValue;
  
  //- Timeout units
  typedef enum  
  {
    TMO_UNIT_SEC,  //- second
    TMO_UNIT_MSEC, //- millisecond
    TMO_UNIT_USEC, //- microsecond
  } TimeoutUnit;
  
  //- instanciates a Timeout (default dtor)
  Timeout () 
  	: _unit(TMO_UNIT_MSEC), _tmo(0), _enabled(false)
  {
		//- noop 
  }
  
  //- instanciates a Timeout
  Timeout (TimeoutValue tmo_in_unit, TimeoutUnit unit = TMO_UNIT_MSEC, bool enabled = false) 
  	: _unit(unit), _tmo(tmo_in_unit), _enabled(enabled)
  {
		//- noop 
  }
  
  //- restarts/(re)enables the Timeout 
  //- in the <disabled> state, Timeout::expired will always return false
  inline void restart () 
  {
  	enable();
  }
  
  //- enables the Timeout 
  //- in the <disabled> state, Timeout::expired will always return false
  inline void enable (bool restart_timer = true) 
  {
  	if (restart_timer) _t.restart();
    _enabled = true;
  }
  
  //- disables the Timeout 
  //- in the <disabled> state, Timeout::expired will always return false
  inline void disable () 
  {
    _enabled = false;
  }
    
  //- enabled?
  inline bool enabled () const
  {
    return _enabled;
  }
  
  //- Timeout expired?
  inline bool expired ()
  {
    //- a disabled Timeout never expire
    if (! _enabled)
    	return false;
    //- however, an enabled Timeout might expire :-)
    double dt = 0.;
    switch (_unit) 
    {
      case TMO_UNIT_SEC:
        dt = _t.elapsed_sec();
        break;
      case TMO_UNIT_MSEC:
        dt = _t.elapsed_msec();
        break;
      case TMO_UNIT_USEC:
        dt = _t.elapsed_usec();
        break;
      default:
        break;
    }
    return dt >= _tmo;
  }
  
  //- time to expiration in Timeout unit?
  //- a negative value means: expired ... tmo-unit ago 
  //- a positive value means: will expired in ... tmo-unit
  //- for a disabled Timeout, returns the infinity value 
  inline TimeoutValue time_to_expiration ()
  {
    //- undefined if disabled...
    if (! _enabled)
      return std::numeric_limits<TimeoutValue>::infinity();
    double dt = 0.;
    switch (_unit) 
    {
      case TMO_UNIT_SEC:
        dt = _t.elapsed_sec();
        break;
      case TMO_UNIT_MSEC:
        dt = _t.elapsed_msec();
        break;
      case TMO_UNIT_USEC:
        dt = _t.elapsed_usec();
        break;
      default:
        break;
    }
    return _tmo - dt;
  }
  
  //- changes the Timeout value
  inline void set_value (TimeoutValue tmo) 
  {
    _tmo = tmo;
  }
  
  //- returns the Timeout value 
  inline TimeoutValue get_value () const
  {
    return _tmo;
  }
  
  //- changes the Timeout unit
  inline void set_unit (TimeoutUnit unit) 
  {
    _unit = unit;
  }
  
  //- returns the Timeout unit 
  inline TimeoutUnit get_unit () const
  {
    return _unit;
  }
  
private:
  Timer _t;
	TimeoutUnit _unit;
  double _tmo;
  bool _enabled;
};

} //- namespace yat

#endif
