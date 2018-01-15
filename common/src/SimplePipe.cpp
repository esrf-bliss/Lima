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
#include "lima/SimplePipe.h"

#include <sys/select.h>
#include <iostream>
#include <cmath>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;

//-------------------------------------------------------------
// SimpePipe
//-------------------------------------------------------------

const int Pipe::DefBuffSize = 4096;

Pipe::Pipe(int buff_size)
	: m_buff_size(buff_size ? buff_size : DefBuffSize)
{
	if (::pipe(m_fd) < 0) {
		cerr << "Error creating pipe" << endl;
		throw exception();
	}
}

Pipe::~Pipe()
{
	close(ReadFd);
	close(WriteFd);
}

void Pipe::close(int which)
{
	int& fd = m_fd[which];
	if (fd)
		::close(fd);
	fd = 0;
}

void Pipe::write(string s)
{
	if (::write(m_fd[WriteFd], s.data(), s.size()) < 0) {
		cerr << "Error writing to pipe" << endl;
		throw exception();
	}
}

string Pipe::read(int len, double timeout)
{
	char buffer[m_buff_size];

	string s;
	while (int(s.size()) < len) {
		if (!waitForInput(timeout))
			break;

		ssize_t req_read = len - s.size();
		if (req_read > m_buff_size)
			req_read = m_buff_size;
		ssize_t read_bytes = ::read(m_fd[ReadFd], buffer, req_read);
		if (read_bytes < 0) {
			cerr << "Error reading from pipe" << endl;
			throw exception();
		} else if (read_bytes == 0)
			break;
		s.append(buffer, read_bytes);
	}

	return s;
}

std::string Pipe::readLine(int len, string term, double timeout)
{
	if (term.size() == 0)
		return read(len, timeout);

	string s;
	while ((s.size() == 0) || (s.find(term) == s.npos)) {
		string r = read(1, timeout);
		if (r.size() == 0)
			break;
		s.append(r);
	}

	return s;
}

bool Pipe::waitForInput(double timeout)
{
	int fd = m_fd[ReadFd];
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	struct timeval tout_ts = {0, 0};
	if (timeout > 0) {
		tout_ts.tv_sec = long(floor(timeout));
		tout_ts.tv_usec = long((timeout - tout_ts.tv_sec) * 1e6);
	}
	struct timeval *tout_ptr = (timeout >= 0) ? &tout_ts : NULL;

	int ret = select(fd + 1, &rfds, NULL, NULL, tout_ptr);
	if (ret < 0) {
		cerr << "Error in select-read pipe "
		     << "(timeout=" << timeout << ")"
		     << ": " << strerror(errno) << endl;
		throw exception();
	}

	return (ret > 0);
}
