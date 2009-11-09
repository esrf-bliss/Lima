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


SerialLine::SerialLine(Dev& edev, const string& line_term, 
		       double timeout, int block_size, 
		       double block_delay) 
	: HwSerialLine(line_term, timeout, block_size, block_delay),
	  m_dev(edev)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM_VAR1(edev.getDevNb());

	if (edev.isMeta()) {
		DEB_ERROR() << "Specified Dev is a meta-device";
		throw LIMA_HW_EXC(NotSupported, "Can't create a "
		                          "SerialLine for a meta-device");
	}

	ostringstream os;
	os << "Serial#" << edev.getDevNb();
	DEB_SET_OBJ_NAME(os.str());
}


SerialLine::~SerialLine()
{
	DEB_DESTRUCTOR();
}


Dev& SerialLine::getDev()
{
	return m_dev;
}


void SerialLine::write(const string& buffer, bool no_wait)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR2(buffer, no_wait);

	unsigned long len = buffer.size();
	char *ptr = len ? (char *) buffer.data() : NULL;
	int block_size; getBlockSize(block_size);
	double block_delay; getBlockDelay(block_delay);

	DEB_TRACE() << "Calling espia_ser_write";
	CHECK_CALL(espia_ser_write(m_dev, ptr, &len, block_size, 
				   Sec2USec(block_delay), !no_wait));
}


void SerialLine::read(string& buffer, int max_len, double timeout)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR2(max_len, timeout);

	buffer.resize(max_len);
	char *ptr = max_len ? (char *) buffer.data() : NULL;
	unsigned long ret_len = max_len;
	unsigned long tmout = Sec2USec(checkDefTimeout(timeout));

	DEB_TRACE() << "Calling espia_ser_read";
	CHECK_CALL(espia_ser_read(m_dev, ptr, &ret_len, tmout));
	buffer.resize(ret_len);

	DEB_RETURN_VAR1(buffer);
}


void SerialLine::readStr(string& buffer, int max_len, 
			 const string& term, double timeout)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR3(max_len, term, timeout);

	buffer.resize(max_len);
	char *ptr = max_len ? (char *) buffer.data() : NULL;
	char *term_ptr = (char *) term.data();
	unsigned long ret_len = max_len;
	unsigned long tmout = Sec2USec(checkDefTimeout(timeout));

	DEB_TRACE() << "Calling espia_ser_read_str";
	CHECK_CALL(espia_ser_read_str(m_dev, ptr, &ret_len, term_ptr,
				      term.size(), tmout));
	buffer.resize(ret_len);

	DEB_RETURN_VAR1(buffer);
}


void SerialLine::flush()
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Calling espia_ser_flush";
	CHECK_CALL(espia_ser_flush(m_dev));
}


void SerialLine::getNbAvailBytes(int& avail_bytes)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Calling espia_ser_read";
	unsigned long ret_bytes = 0;
	CHECK_CALL(espia_ser_read(m_dev, NULL, &ret_bytes, 0));
	avail_bytes = ret_bytes;
	DEB_RETURN_VAR1(avail_bytes);
}
