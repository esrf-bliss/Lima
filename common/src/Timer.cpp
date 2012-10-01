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
#include "Timer.h"

#ifdef WIN32

#else  // unix
#if (_POSIX_C_SOURCE - 0) >= 199309L
using namespace lima;

/** @brief Timer constructor
 */
Timer::Timer(Timer::Callback *callback) :
  m_callback(callback),
  m_stat(DOWN)
{
  struct sigevent sev;
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = this;
  sev.sigev_notify_function = Timer::_rising_handle;
  sev.sigev_notify_attributes = NULL;

  timer_create(CLOCK_REALTIME,&sev,&m_rising_timer_id);
  sev.sigev_notify_function = Timer::_falling_handle;
  timer_create(CLOCK_REALTIME,&sev,&m_falling_timer_id);
}
/** @brief Timer destructor
 */
Timer::~Timer()
{
  timer_delete(m_rising_timer_id);
  timer_delete(m_falling_timer_id);
}
/** @brief similare to start by with a delay
    @see Timer::start
 */
void Timer::delayStart(double delay,double uptime,int nb_tick,double latency)
{
  m_iter = nb_tick;
  struct itimerspec rising_value;
  struct itimerspec falling_value;
  
  time_t second = time_t(delay);
  rising_value.it_value.tv_sec = second;
  rising_value.it_value.tv_nsec = long((delay - second) * 1e9);
  
  double nextRising = uptime + latency;
  second = time_t(nextRising);
  rising_value.it_interval.tv_sec = second;
  rising_value.it_interval.tv_nsec = long((nextRising - second) * 1e9);

  falling_value.it_interval = rising_value.it_interval;

  double nextFalling = delay + uptime;
  second = time_t(nextFalling);
  falling_value.it_value.tv_sec = second;
  falling_value.it_value.tv_nsec = long((nextFalling - second) * 1e9);

  if(m_callback)
    m_callback->start();

  AutoMutex lock(m_mutex);
  m_stat = DELAY;
  timer_settime(m_rising_timer_id,0,&rising_value,NULL);
  timer_settime(m_falling_timer_id,0,&falling_value, NULL);
}
/** @brief start timer.
    This will create a periodic signal during nb tick with an uptime and a latency (all in second)
    @param uptime the time between a RISING_EDGE and a FALLING_EDGE
    @param nb_tick the total period of the signal if <= 0 infinite
    @param latency the time between a FALLING_EDGE and a RISING_EDGE 
*/
    
void Timer::start(double uptime,int nb_tick,double latency)
{
  m_iter = nb_tick;
  struct itimerspec falling_value;
  struct itimerspec rising_value;

  time_t second = time_t(uptime);
  falling_value.it_value.tv_sec = second;
  falling_value.it_value.tv_nsec = long((uptime - second) * 1e9);


  if(nb_tick != 1)
    {
      double nextRising = uptime + latency;
      second = time_t(nextRising);
      falling_value.it_interval.tv_sec = second;
      falling_value.it_interval.tv_nsec = long((nextRising - second) * 1e9);
      rising_value.it_value = rising_value.it_interval = falling_value.it_interval;
    }
  else
    {
      rising_value.it_value.tv_sec = falling_value.it_interval.tv_sec = 0;
      rising_value.it_value.tv_nsec = falling_value.it_interval.tv_nsec = 0;
    }
  AutoMutex lock(m_mutex);
  m_stat = RISING_EDGE;
  lock.unlock();

  if(m_callback)
    {
      m_callback->start();
      m_callback->risingEdge();
    }

  lock.lock();
  if(m_stat == RISING_EDGE)
    {
      m_stat = UP;
      
      timer_settime(m_falling_timer_id,0,&falling_value, NULL);
      timer_settime(m_rising_timer_id,0,&rising_value,NULL);
    }
}

void Timer::stop()
{
  AutoMutex lock(m_mutex);
  _stop();
}

void Timer::_stop()
{

  struct itimerspec value;
  
  value.it_value.tv_sec = value.it_interval.tv_sec = 0;
  value.it_value.tv_nsec = value.it_interval.tv_nsec = 0;
  timer_settime(m_falling_timer_id,0,&value, NULL);
  timer_settime(m_rising_timer_id,0,&value,NULL);

  if(m_callback && m_stat != FALLING_EDGE)
    m_callback->fallingEdge();
  if(m_callback && m_stat != DOWN)
    m_callback->end();
  m_stat = DOWN;
}

void Timer::_rising_handle(union sigval usr_arg)
{
  Timer *self = (Timer*)usr_arg.sival_ptr;

  AutoMutex lock(self->m_mutex);
  self->m_stat = Timer::RISING_EDGE;
  lock.unlock();
  if(self->m_callback)
    self->m_callback->risingEdge();

  lock.lock();
  if(self->m_stat == Timer::RISING_EDGE)
    self->m_stat = Timer::UP;
}

void Timer::_falling_handle(union sigval usr_arg)
{
  Timer *self = (Timer*)usr_arg.sival_ptr;

  AutoMutex lock(self->m_mutex);
  self->m_stat = Timer::FALLING_EDGE;
  lock.unlock();

  if(self->m_callback)
    self->m_callback->fallingEdge();

  lock.lock();
  if(self->m_iter > 0 && !--self->m_iter)
    self->_stop();
}
#endif	// _POSIX_C_SOURCE
#endif	// unix
