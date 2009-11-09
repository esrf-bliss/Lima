// This code was taken from the extstream.h file (Generig debug in C++)
// in: /segfs/dserver/classes++/extdevice/

#ifndef STREAMUTILS_H
#define STREAMUTILS_H

#include <iostream>
#include <sstream>
#include <vector>

class NullStreamBuf : public std::stringbuf
{
 protected:
	int sync()
	{
		int ret = std::stringbuf::sync();
		str(std::string());
		return ret;
	}
};


class CopyStreamBuf : public std::stringbuf
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


class OCopyStream : public std::ostream
{
 public:
	typedef CopyStreamBuf::StreamList StreamList;

	OCopyStream(StreamList *slist, std::ostream *altstream = NULL)
		: std::ostream(&sb), sb(slist, altstream) {}
	
	OCopyStream::OCopyStream(const OCopyStream& other)
		: std::ostream(&sb),
		sb(other.sb.getStreamList(), other.sb.getAltStream()) {}

 private:
	CopyStreamBuf sb;
};

#endif // STREAMUTILS_H
