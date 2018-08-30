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
#include "lima/Exceptions.h"

#include <sys/select.h>
#include <iostream>
#include <cmath>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;
using namespace lima;

//-------------------------------------------------------------
// Pipe::Stream
//-------------------------------------------------------------

Pipe::Stream::Stream()
	: m_fd(-1)
{
	DEB_CONSTRUCTOR();
}

Pipe::Stream::~Stream()
{
	DEB_DESTRUCTOR();

	while (!m_dup_list.empty())
		restoreDup();
	if (m_fd >= 0)
		close();
}

void Pipe::Stream::setFd(int fd)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(fd);

	if (fd < 0)
		THROW_COM_ERROR(InvalidValue) << DEB_VAR1(fd);
	else if (m_fd >= 0)
		THROW_COM_ERROR(InvalidValue) << "FD already set";
	m_fd = fd;
}

int Pipe::Stream::getFd()
{
	return m_fd;
}

void Pipe::Stream::dupInto(int target_fd)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(target_fd);

	if (target_fd < 0)
		THROW_COM_ERROR(InvalidValue) << DEB_VAR1(target_fd);
	else if (m_fd < 0)
		THROW_COM_ERROR(InvalidValue) << "FD not set";
	else if (target_fd == m_fd)
		THROW_COM_ERROR(InvalidValue) << "dup on same FD";

	int copy_fd = dup(target_fd);
	if (copy_fd < 0)
		THROW_COM_ERROR(Error) << "cannot dup new FD";
	if (dup2(m_fd, target_fd) != target_fd) {
		::close(copy_fd);
		THROW_COM_ERROR(Error) << "cannot dup current FD";
	}
	m_dup_list.push_back(DupData(m_fd, target_fd, copy_fd));
	m_fd = target_fd;
}

void Pipe::Stream::restoreDup()
{
	DEB_MEMBER_FUNCT();

	if (m_dup_list.empty())
		THROW_COM_ERROR(Error) << "no dup info";
	DupData dup_data = m_dup_list.back();
	if (m_fd != dup_data.target)
		THROW_COM_ERROR(Error) << "dup FD mismatch";
	if (dup2(dup_data.copy, dup_data.target) != dup_data.target)
		THROW_COM_ERROR(Error) << "cannot restore dup FD";
	m_dup_list.pop_back();
	::close(dup_data.copy);
	m_fd = dup_data.prev;
}

void Pipe::Stream::close()
{
	DEB_MEMBER_FUNCT();

	if (m_fd < 0)
		THROW_COM_ERROR(Error) << "no open FD";
	::close(m_fd);
	m_fd = -1;
}

//-------------------------------------------------------------
// Pipe
//-------------------------------------------------------------

const int Pipe::DefBuffSize = 4096;

Pipe::Pipe(int buff_size, bool binary)
	: m_buff_size(buff_size ? buff_size : DefBuffSize), m_binary(binary)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR3(buff_size, m_buff_size, m_binary);

	int fd[NbPipes];
	if (::pipe(fd) < 0)
		THROW_COM_ERROR(Error) << "error creating pipe";
	DEB_TRACE() << DEB_VAR2(fd[ReadFd], fd[WriteFd]);
	m_stream[ReadFd].setFd(fd[ReadFd]);
	m_stream[WriteFd].setFd(fd[WriteFd]);
}

void Pipe::dupInto(EndFd which, int target_fd)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(which, target_fd);
	m_stream[which].dupInto(target_fd);
}

void Pipe::restoreDup(EndFd which)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(which);
	m_stream[which].restoreDup();
}

void Pipe::close(EndFd which)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(which);
	m_stream[which].close();
}

void Pipe::write(string s)
{
	DEB_MEMBER_FUNCT();
	if (!m_binary)
		DEB_PARAM() << DEB_VAR1(s);

	int fd = m_stream[WriteFd].getFd();
	if (::write(fd, s.data(), s.size()) < s.size())
		THROW_COM_ERROR(Error) << "error writing to pipe";
}

string Pipe::read(int len, double timeout)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(len, timeout);
	string s = readFunct(len, timeout, DEB_PTR());
	if (!m_binary)
		DEB_RETURN() << DEB_VAR1(s);
	return s;
}

string Pipe::readFunct(int len, double timeout, DebObj *deb_ptr)
{
	DEB_FROM_PTR(deb_ptr);

	string s;
	while (int(s.size()) < len) {
		if (!waitForInput(timeout, deb_ptr))
			break;

		ssize_t req_read = len - s.size();
		if (req_read > m_buff_size)
			req_read = m_buff_size;
		int fd = m_stream[ReadFd].getFd();
		char buffer[m_buff_size];
		ssize_t read_bytes = ::read(fd, buffer, req_read);
		if (read_bytes < 0)
			THROW_COM_ERROR(Error) << "error reading from pipe";
		else if (read_bytes == 0)
			break;
		s.append(buffer, read_bytes);
	}

	return s;
}

std::string Pipe::readLine(int len, string term, double timeout)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR3(len, term, timeout);

	if (term.size() == 0)
		return read(len, timeout);

	string s;
	while ((s.size() == 0) || (s.find(term) == s.npos)) {
		string r = readFunct(1, timeout, DEB_PTR());
		if (r.size() == 0)
			break;
		s.append(r);
	}

	if (!m_binary)
		DEB_RETURN() << DEB_VAR1(s);
	return s;
}

bool Pipe::waitForInput(double timeout, DebObj *deb_ptr)
{
	DEB_FROM_PTR(deb_ptr);

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
	if (ret < 0)
		THROW_COM_ERROR(Error) << "Error in select-read pipe "
				       << "(" << DEB_VAR1(timeout) << "): "
				       << strerror(errno);
	return (ret > 0);
}
