//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef TIMER_H
#define TIMER_H
#include "lima/ThreadUtils.h"

#ifdef WIN32

#else  // unix
#include <features.h>
#if (_POSIX_C_SOURCE - 0) >= 199309L
#include <time.h>
#include <signal.h>
namespace lima
{
  class Timer
  {
  public:
    enum Stat {DOWN,DELAY,UP,RISING_EDGE,FALLING_EDGE};
    class Callback
    {
    protected:
      virtual void start() {}
      virtual void risingEdge() {}
      virtual void fallingEdge() {}
      virtual void end() {}
    private:
      friend class Timer;
    };

    Timer(Callback* = NULL);
    ~Timer();
  
    void delayStart(double delay,double uptime,
		    int nb_iter = 1,double latency = 0.);
    void start(double uptime,int nb_iter = 1,double latency = 0.);
    void stop();
  private:
    static void _rising_handle(union sigval);
    friend void _rising_handle(union sigval);

    static void _falling_handle(union sigval);
    friend void _falling_handle(union sigval);
    
    void _stop();

    timer_t 	m_rising_timer_id;
    timer_t 	m_falling_timer_id;
    int 	m_iter;
    Callback*	m_callback;
    Mutex	m_mutex;
    Stat	m_stat;
  };
}
#else
#warning Timer not implemented
#endif

#endif	// WIN32

#endif
