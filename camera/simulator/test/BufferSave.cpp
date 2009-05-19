#include <ctime>
#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include "BufferSave.h"

using namespace lima;
using namespace std;


int BufferSave::writeEdfHeader( const FrameInfoType &finfo, 
                                std::ofstream &file )
{
	time_t ctime_now;
	struct timeval tod_now;
	char time_str[64], buffer[EDF_HEADER_BUFFER_LEN], *p;
	int l, len, rem;

	time(&ctime_now);
	gettimeofday(&tod_now, NULL);
	ctime_r(&ctime_now, time_str);
	time_str[strlen(time_str) - 1] = '\0';

	p = buffer;
	p += sprintf(p, "{\n");
	p += sprintf(p, "HeaderID = EH:%06lu:000000:000000 ;\n", finfo.acq_frame_nb);
	p += sprintf(p, "Image = %lu ;\n", finfo.acq_frame_nb);
	p += sprintf(p, "ByteOrder = LowByteFirst ;\n");
	p += sprintf(p, "DataType = %s ;\n", 
		     (finfo.depth == 1) ? "UnsignedByte" :
		     ((finfo.depth == 2) ? "UnsignedShort" : "UnsignedLong"));
	p += sprintf(p, "Size = %d ;\n", finfo.width*finfo.height*finfo.depth);
	p += sprintf(p, "Dim_1 = %d ;\n", finfo.width);
	p += sprintf(p, "Dim_2 = %d ;\n", finfo.height);

	p += sprintf(p, "time = %s ;\n", time_str);
	p += sprintf(p, "time_of_day = %ld.%06ld ;\n", 
		     tod_now.tv_sec, tod_now.tv_usec);

	l = p - buffer;
	len = l;
	rem = len % EDF_HEADER_LEN;
	if (rem > 0)
		len += EDF_HEADER_LEN - rem;
	p += sprintf(p, "%*s}\n", len - (l + 2), "");
	len = p - buffer;

	file.write(buffer, len);
	return len;
}


BufferSave::BufferSave( const string &prefix, enum FileFormat format ) :
	m_prefix(prefix), m_format(format) 
{
}


BufferSave::~BufferSave( ) 
{
}


int BufferSave::writeFrame( FrameInfoType &finfo )
{
	string name;
	char num_str[64];

	name = m_prefix;
	snprintf(num_str, 63, "%06lu", finfo.acq_frame_nb);
	name += string(num_str);
	name += ".edf";

	ofstream file(name.c_str(), ios_base::out | ios_base::binary);

	if( m_format == FMT_EDF )
		writeEdfHeader(finfo, file);

	file.write((char *)finfo.frame_ptr, finfo.width*finfo.height*finfo.depth);

	file.close();

	return 0;
}
