#include "BufferSave.h"

#include <ctime>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>

using namespace lima;
using namespace std;

#define EDF_HEADER_LEN		1024
#define EDF_HEADER_BUFFER_LEN	(10 * EDF_HEADER_LEN)


BufferSave::BufferSave( FileFormat format, const String& prefix, int idx,
			const String& suffix, bool overwrite, 
			int tot_file_frames ) 
	: m_format(format), m_prefix(prefix), m_idx(idx), m_suffix(suffix),
	  m_overwrite(overwrite), m_tot_file_frames(tot_file_frames)
{
	m_written_frames = 0;
	m_fout = NULL;
	if (m_suffix == "")
		m_suffix = getDefSuffix();
}


BufferSave::BufferSave( ) 
	: m_format(Raw), m_prefix(), m_idx(0), m_overwrite(false),
	  m_tot_file_frames(1)
{
	m_written_frames = 0;
	m_fout = NULL;
	if (m_suffix == "")
		m_suffix = getDefSuffix();
}


BufferSave::~BufferSave( ) 
{
	closeFile();
}

BufferSave::String BufferSave::getDefSuffix() const
{
	return (m_format == EDF) ? ".edf" : ".raw";
}

void BufferSave::openFile()
{
	if (isFileOpen())
		return;

	ostringstream idx;
	idx.width(4);
	idx.fill('0');
	idx << m_idx;
	
	m_file_name = m_prefix + idx.str() + m_suffix;
	m_fout = new ofstream(m_file_name.c_str(), 
			      ios_base::out | ios_base::binary);

}

void BufferSave::closeFile()
{
	if (!isFileOpen())
		return;

	delete m_fout;
	m_fout = NULL;
	m_file_name = "";
	m_written_frames = 0;
	m_idx++;
}

void BufferSave::writeEdfHeader( const FrameInfoType& finfo )
{
	time_t ctime_now;
	struct timeval tod_now;
	char time_str[64], buffer[EDF_HEADER_BUFFER_LEN], *p;
	int l, len, rem;
	const FrameDim *fdim = finfo.frame_dim;

	const Size& frame_size = fdim->getSize();
	int depth = fdim->getDepth();

	int image = m_written_frames + 1;

	time(&ctime_now);
	gettimeofday(&tod_now, NULL);
	ctime_r(&ctime_now, time_str);
	time_str[strlen(time_str) - 1] = '\0';

	p = buffer;
	p += sprintf(p, "{\n");
	p += sprintf(p, "HeaderID = EH:%06u:000000:000000 ;\n", image);
	p += sprintf(p, "Image = %u ;\n", image);
	p += sprintf(p, "ByteOrder = LowByteFirst ;\n");
	p += sprintf(p, "DataType = %s ;\n", 
		      (depth == 1) ? "UnsignedByte" :
		     ((depth == 2) ? "UnsignedShort" : "UnsignedLong"));
	p += sprintf(p, "Size = %d ;\n", fdim->getMemSize());
	p += sprintf(p, "Dim_1 = %d ;\n", frame_size.getWidth());
	p += sprintf(p, "Dim_2 = %d ;\n", frame_size.getHeight());

	p += sprintf(p, "acq_frame_nb = %d ;\n", finfo.acq_frame_nb);
	p += sprintf(p, "time = %s ;\n", time_str);
	p += sprintf(p, "time_of_day = %ld.%06ld ;\n", 
		     tod_now.tv_sec, tod_now.tv_usec);
        if (finfo.frame_timestamp.isSet())
                p += sprintf(p, "time_of_frame = %.6f ;\n",
                             double(finfo.frame_timestamp));

	l = p - buffer;
	len = l;
	rem = len % EDF_HEADER_LEN;
	if (rem > 0)
		len += EDF_HEADER_LEN - rem;
	p += sprintf(p, "%*s}\n", len - (l + 2), "");
	len = p - buffer;

	m_fout->write(buffer, len);
}

void BufferSave::writeFrame( const FrameInfoType& finfo )
{
	const FrameDim *fdim = finfo.frame_dim;
	if (fdim == NULL)
		throw LIMA_HW_EXC(InvalidValue, "Null finfo.fdim");

	openFile();

	if ( m_format == EDF )
		writeEdfHeader(finfo);

	m_fout->write((char *)finfo.frame_ptr, fdim->getMemSize());

	m_written_frames++;
	if (m_written_frames == m_tot_file_frames)
		closeFile();
}

bool BufferSave::isFileOpen() const
{
	return (m_fout != NULL);
}

void BufferSave::setPrefix(const String& prefix)
{
	if (prefix == m_prefix)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, "Set prefix with open file");

	m_prefix = prefix;
}

void BufferSave::getPrefix(String& prefix) const
{
	prefix = m_prefix;
}

void BufferSave::setFormat(FileFormat format)
{
	if (format == m_format)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, "Set format with open file");

	m_format = format;
	m_suffix = getDefSuffix();

}

void BufferSave::getFormat(FileFormat& format) const
{
	format = m_format;
}

void BufferSave::setIndex(int idx)
{
	if (idx == m_idx)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, "Set idx with open file");

	m_idx = idx;
}

void BufferSave::getIndex(int& idx) const
{
	idx = m_idx;
}

void BufferSave::setTotFileFrames(int tot_file_frames)
{
	if (tot_file_frames == m_tot_file_frames)
		return;

	if (isFileOpen())
		throw LIMA_HW_EXC(InvalidValue, 
				  "Set tot_file_frames with open file");

	m_tot_file_frames = tot_file_frames;
}

void BufferSave::getTotFileFrames(int& tot_file_frames) const
{
	tot_file_frames = m_tot_file_frames;
}

