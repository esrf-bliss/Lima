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

%If (POSIX_PLATFORM)
class Timer
{
%TypeHeaderCode
#include "lima/Timer.h"
%End

public:
  enum Stat {DOWN,UP,RISING_EDGE,FALLING_EDGE};
  class Callback
  {
  public:
    virtual ~Callback();
  protected:
    virtual void start();
    virtual void risingEdge();
    virtual void fallingEdge();
    virtual void end();
  };

  Timer(Timer::Callback* = NULL);
  ~Timer();

  void delayStart(double delay,double uptime,
		  int nb_iter = 1,double latency = 0.);
  void start(double uptime,int nb_iter = 1,double latency = 0.);
  void stop();
};
%End
