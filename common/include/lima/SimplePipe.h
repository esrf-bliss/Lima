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
#ifndef SIMPLEPIPE_H
#define SIMPLEPIPE_H

#include <string>

class Pipe
{
 public:
	enum {
		ReadFd = 0, WriteFd = 1,
	};

	Pipe(int buff_size = 0);
	~Pipe();

	void write(std::string s);
	std::string read(int len, double timeout = -1);
	std::string readLine(int len, std::string term, double timeout = -1);

	void close(int which);

 private:
	bool waitForInput(double timeout);

	int m_fd[2];
	int m_buff_size;

	static const int DefBuffSize;
};


#endif // SIMPLEPIPE_H
