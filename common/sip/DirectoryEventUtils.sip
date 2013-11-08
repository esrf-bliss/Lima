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
class DirectoryEvent
{
%TypeHeaderCode
#include <DirectoryEventUtils.h>
using namespace lima;
%End

public:
  struct Parameters
  {
    std::string watch_path;
    std::string file_pattern;
    int	  next_file_number_expected;
  };
  class Callback
  {
  public:
    virtual void prepare(const DirectoryEvent::Parameters &parameters);
    virtual bool nextFileExpected(int file_number,
				  const char *full_path,
				  int &next_file_number_expected /In,Out/) throw() = 0;
    virtual bool newFile(int file_number,const char *full_path) throw() = 0;
  };

  DirectoryEvent(bool local,Callback&);
  ~DirectoryEvent();
    
  void prepare(const Parameters&);
  void start();
  void stop();
  bool isStopped() const;
  int  getNextFileNumberExpected() const;

  void watch_moved_to();
  void watch_close_write();
};
%End
