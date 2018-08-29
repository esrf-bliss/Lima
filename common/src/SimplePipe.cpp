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
// Pipe::Stream
//-------------------------------------------------------------

Pipe::Stream::Stream()
	: m_fd(-1)
{
}

Pipe::Stream::~Stream()
{
	while (!m_dup_list.empty())
		restoreDup();
	if (m_fd >= 0)
		close();
}

void Pipe::Stream::setFd(int fd)
{
	if (fd < 0) {
		cerr << "Error: invalid Pipe::Stream FD" << endl;
		throw exception();
	} else if (m_fd >= 0) {
		cerr << "Error: Pipe::Stream FD already set" << endl;
		throw exception();
	}
	m_fd = fd;
}

int Pipe::Stream::getFd()
{
	return m_fd;
}

void Pipe::Stream::dupInto(int target_fd)
{
	if (target_fd < 0) {
		cerr << "Error: invalid Pipe::Stream target FD" << endl;
		throw exception();
	} else if (m_fd < 0) {
		cerr << "Error: Pipe::Stream FD not set" << endl;
		throw exception();
	} else if (target_fd == m_fd) {
		cerr << "Error: dup Pipe::Stream on same FD" << endl;
		throw exception();
	}
	int copy_fd = dup(target_fd);
	if (copy_fd < 0) {
		cerr << "Error: cannot dup Pipe::Stream new FD" << endl;
		throw exception();
	}
	if (dup2(m_fd, target_fd) != target_fd) {
		::close(copy_fd);
		cerr << "Error: cannot dup Pipe::Stream current FD" << endl;
		throw exception();
	}
	m_dup_list.push_back(DupData(m_fd, target_fd, copy_fd));
	m_fd = target_fd;
}

void Pipe::Stream::restoreDup()
{
	if (m_dup_list.empty()) {
		cerr << "Error: no Pipe::Stream dup info" << endl;
		throw exception();
	}
	DupData dup_data = m_dup_list.back();
	if (m_fd != dup_data.target) {
		cerr << "Error: Pipe::Stream dup FD mismatch" << endl;
		throw exception();
	}
	if (dup2(dup_data.copy, dup_data.target) != dup_data.target) {
		cerr << "Error: cannot restore Pipe::Stream dup FD" << endl;
		throw exception();
	}
	m_dup_list.pop_back();
	::close(dup_data.copy);
	m_fd = dup_data.prev;
}

void Pipe::Stream::close()
{
	if (m_fd < 0) {
		cerr << "Error: no Pipe::Stream open FD" << endl;
		throw exception();
	}
	::close(m_fd);
	m_fd = -1;
}

//-------------------------------------------------------------
// Pipe
//-------------------------------------------------------------

const int Pipe::DefBuffSize = 4096;

Pipe::Pipe(int buff_size)
	: m_buff_size(buff_size ? buff_size : DefBuffSize)
{
	int fd[NbPipes];
	if (::pipe(fd) < 0) {
		cerr << "Error creating pipe" << endl;
		throw exception();
	}
	m_stream[ReadFd].setFd(fd[ReadFd]);
	m_stream[WriteFd].setFd(fd[WriteFd]);
}

void Pipe::dupInto(EndFd which, int target_fd)
{
	m_stream[which].dupInto(target_fd);
}

void Pipe::restoreDup(EndFd which)
{
	m_stream[which].restoreDup();
}

void Pipe::close(EndFd which)
{
	m_stream[which].close();
}

void Pipe::write(string s)
{
	int fd = m_stream[WriteFd].getFd();
	if (::write(fd, s.data(), s.size()) < 0) {
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
		int fd = m_stream[ReadFd].getFd();
		ssize_t read_bytes = ::read(fd, buffer, req_read);
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
	int fd = m_stream[ReadFd].getFd();
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
