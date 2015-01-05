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
// This code was taken from the extstream.h file (Generig debug in C++)
// in: /segfs/dserver/classes++/extdevice/

#ifndef STREAMUTILS_H
#define STREAMUTILS_H

#include <iostream>
#include <sstream>
#include <vector>

#include "LimaCompatibility.h"

class LIMACORE_API NullStreamBuf : public std::stringbuf
{
 protected:
	int sync()
	{
		int ret = std::stringbuf::sync();
		str(std::string());
		return ret;
	}
};


class LIMACORE_API CopyStreamBuf : public std::stringbuf
{
 public:
	typedef std::vector<std::ostream *> StreamList;
	
	CopyStreamBuf(StreamList *slist, std::ostream *altstream = NULL)
		: m_streams(slist), m_alt_stream(altstream) 
	{}

	StreamList *getStreamList() const
	{ return m_streams; }

	std::ostream *getAltStream() const
	{ return m_alt_stream; }

 protected:
	int sync()
	{
		int ret = std::stringbuf::sync();

		std::string out_str = str();
		str(std::string());

		if (m_streams) {
			typedef StreamList::iterator iterator;
			iterator end = m_streams->end();
			for (iterator it = m_streams->begin(); it < end; ++it)
				*(*it) << out_str << std::flush;
		} else if (m_alt_stream)
			*m_alt_stream << out_str << std::flush;

		return ret;
	}

 private:
	StreamList *m_streams;
	std::ostream *m_alt_stream;
};


class LIMACORE_API OCopyStream : public std::ostream
{
 public:
	typedef CopyStreamBuf::StreamList StreamList;

	OCopyStream(StreamList *slist, std::ostream *altstream = NULL)
		: std::ostream(NULL), sb(slist, altstream) 
	{
		init(&sb);
	}
	
	OCopyStream(const OCopyStream& other)
		: std::ios(), std::ostream(NULL),
		sb(other.sb.getStreamList(), other.sb.getAltStream()) 
	{
		init(&sb);
	}

 private:
	CopyStreamBuf sb;
};

#endif // STREAMUTILS_H
