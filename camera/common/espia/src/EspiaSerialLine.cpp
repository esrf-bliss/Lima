/***************************************************************//**
 * @file   EspiaSerialLine.cpp
 * @brief  This file contains the Espia::SerialLine class implementation
 *
 * @author A.Kirov, A.Homs
 * @date   03/06/2009
 *******************************************************************/

#include "EspiaSerialLine.h"
#include "Espia.h"

using namespace lima::Espia;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)


SerialLine::SerialLine( Dev& edev, const string& line_term, 
			double timeout, int block_size, 
			double block_delay ) :
	HwSerialLine(line_term, timeout, block_size, block_delay),
	m_dev(edev)
{
	if( edev.isMeta() )
		throw LIMA_HW_EXC(NotSupported, "Can't create a "
		                          "SerialLine for a meta-device");
}


SerialLine::~SerialLine()
{
}


Dev& SerialLine::getDev()
{
	return m_dev;
}


void SerialLine::write( const string& buffer, bool no_wait )
{
	unsigned long len = buffer.size();
	char *ptr = len ? (char *) buffer.data() : NULL;
	int block_size; getBlockSize(block_size);
	double block_delay; getBlockDelay(block_delay);

	CHECK_CALL( espia_ser_write(m_dev, ptr, &len, block_size, 
				    Sec2USec(block_delay), !no_wait) );
}


void SerialLine::read( string& buffer, int max_len, double timeout )
{
	buffer.resize(max_len);
	char *ptr = max_len ? (char *) buffer.data() : NULL;
	unsigned long ret_len = max_len;
	unsigned long tmout = Sec2USec(checkDefTimeout(timeout));

	CHECK_CALL( espia_ser_read(m_dev, ptr, &ret_len, tmout) );
	buffer.resize(ret_len);
}


void SerialLine::readStr( string& buffer, int max_len, 
			  const string& term, double timeout )
{
	buffer.resize(max_len);
	char *ptr = max_len ? (char *) buffer.data() : NULL;
	char *term_ptr = (char *) term.data();
	unsigned long ret_len = max_len;
	unsigned long tmout = Sec2USec(checkDefTimeout(timeout));

	CHECK_CALL( espia_ser_read_str(m_dev, ptr, &ret_len, term_ptr,
				      term.size(), tmout) );
	buffer.resize(ret_len);
}


void SerialLine::flush()
{
	CHECK_CALL(espia_ser_flush(m_dev));
}


void SerialLine::getNbAvailBytes( int &avail_bytes )
{
	unsigned long ret_bytes = 0;

	CHECK_CALL(espia_ser_read(m_dev, NULL, &ret_bytes, 0));
	avail_bytes = ret_bytes;
}
