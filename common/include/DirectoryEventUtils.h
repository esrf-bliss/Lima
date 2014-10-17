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
#ifndef DIRECTORYEVENTUTILS_H
#define DIRECTORYEVENTUTILS_H

#ifdef __linux__
#include <string>
#include "Debug.h"

namespace lima
{
  class DirectoryEvent
  {
  public:
    class _Event;
    struct Parameters
    {
      std::string watch_path;
      std::string file_pattern;
      int	  next_file_number_expected;
    };
    class Callback
    {
    public:
      virtual ~Callback() {};	// Compilation warning
      /** @brief this methode is called before starting to watch the directory set in
	  parameters.watch_path
	  this method should throw an exception if something wrong happens
      */
      virtual void prepare(const DirectoryEvent::Parameters&){};
      /** @brief this methode is called when a file just arrived 
	  and it's the next expeded file number.
	  this method should not throw and return false to stop the directory watching.
	  @params file_number the number of the file
	  @params full_path the full path of the file
	  @params next_file_number_expected you should return the next file number expected
      */	  
      virtual bool nextFileExpected(int file_number,
				    const char *full_path,
				    int &next_file_number_expected) throw() = 0;
      /** @brief a file just arrived but it's not the next expected file number.
       */
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
  private:
    _Event*	m_event;
    bool	m_local;
  };

  inline std::ostream& operator<<(std::ostream &os,
				  const DirectoryEvent::Parameters &params)
  {
    os << "<"
       << "watch_path=" << params.watch_path << ", "
       << "file_pattern=" << params.file_pattern << ", "
       << "next_file_number_expected=" << params.next_file_number_expected
       << ">";
    return os;
  }
}
#endif
#endif
