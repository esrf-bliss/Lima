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

#include "lima/Debug.h"

#include <string>
#include <vector>

namespace lima 
{

class Pipe
{
	DEB_CLASS(DebModCommon, "Pipe");
 public:
	enum EndFd {
		ReadFd, WriteFd,
	};

	Pipe(int buff_size = 0, bool binary = false);

	void write(std::string s);
	std::string read(int len, double timeout = -1);
	std::string readLine(int len, std::string term, double timeout = -1);

	void dupInto(EndFd which, int target_fd);
	void restoreDup(EndFd which);

	void close(EndFd which);

 private:
	enum {
		NbPipes = 2,
	};

	class Stream {
		DEB_CLASS(DebModCommon, "Pipe::Stream");
	public:
		Stream();
		~Stream();

		void setFd(int fd);
		int getFd();
		void dupInto(int target_fd);
		void restoreDup();
		void close();

	private:
		struct DupData {
			int prev;
			int target;
			int copy;
			DupData()
			: prev(-1), target(-1), copy(-1) {}
			DupData(int p, int t, int c)
			: prev(p), target(t), copy(c) {}
		};
		typedef std::vector<DupData> DupDataList;

		int m_fd;
		DupDataList m_dup_list;
	};

	std::string readFunct(int len, double timeout, DebObj *deb_ptr);
	bool waitForInput(double timeout, DebObj *deb_ptr);

	Stream m_stream[NbPipes];
	int m_buff_size;
	bool m_binary;

	static const int DefBuffSize;
};

} // namespace lima

#endif // SIMPLEPIPE_H
