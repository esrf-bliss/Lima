#include "EspiaSerialLine.h"
#include "Espia.h"

using namespace lima;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)


EspiaSerialLine::EspiaSerialLine( EspiaDev& edev, const string& line_term, 
                                  double timeout, int block_size, 
                                  double block_delay ) :
	m_dev(edev),
	HwSerialLine(line_term, timeout, block_size, block_delay)
{
}


EspiaSerialLine::~EspiaSerialLine()
{
}


void EspiaSerialLine::write( const string& buffer, bool no_wait )
{
	unsigned long len = buffer.length();
	char *ptr = len ? (char *) buffer.data() : NULL;
	int block_size; getBlockSize(block_size);
	double block_delay; getBlockDelay(block_delay);

	CHECK_CALL( espia_ser_write(m_dev, ptr, &len, block_size, 
				    m_dev.sec2usec(block_delay), !no_wait) );
}


void EspiaSerialLine::read( string& buffer, int max_len, double timeout )
{
	buffer.resize(max_len);
	char *ptr = max_len ? (char *) buffer.data() : NULL;
	unsigned long ret_len = max_len;
	unsigned long tmout = m_dev.sec2usec(checkDefTimeout(timeout));

	CHECK_CALL( espia_ser_read(m_dev, ptr, &ret_len, tmout) );
	buffer.resize(ret_len);
}


void EspiaSerialLine::readStr( string& buffer, int max_len, 
                               const string& term, double timeout )
{
	buffer.resize(max_len);
	char *ptr = max_len ? (char *) buffer.data() : NULL;
	char *term_ptr = (char *) term.data();
	unsigned long ret_len = max_len;
	unsigned long tmout = m_dev.sec2usec(checkDefTimeout(timeout));

	CHECK_CALL( espia_ser_read_str(m_dev, ptr, &ret_len, term_ptr,
				      term.length(), tmout) );
	buffer.resize(ret_len);
}


void EspiaSerialLine::flush()
{
	CHECK_CALL(espia_ser_flush(m_dev));
}


void EspiaSerialLine::getNumAvailBytes( int &avail_bytes )
{
	unsigned long ret_bytes = 0;

	CHECK_CALL(espia_ser_read(m_dev, NULL, &ret_bytes, 0));
	avail_bytes = ret_bytes;
}
