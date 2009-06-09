#include "Espia.h"

using namespace lima;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

Espia::Espia(int dev_nr)
	: m_dev_nr(Invalid), m_dev(ESPIA_DEV_INVAL)
{
	open(dev_nr);
}

Espia::~Espia()
{
	close();
}

void Espia::open(int dev_nr)
{
	if (dev_nr == m_dev_nr)
		return;

	close();

	CHECK_CALL(espia_open(dev_nr, &m_dev));
	m_dev_nr = dev_nr;
}

void Espia::close()
{
	if (m_dev_nr == Invalid)
		return;

	CHECK_CALL(espia_close(m_dev));
	m_dev = ESPIA_DEV_INVAL;
	m_dev_nr = Invalid;
}

void Espia::serWrite(const string& buffer, int block_size, double block_delay, 
		     bool no_wait)
{
	unsigned long len = buffer.length();
	char *ptr = len ? (char *) buffer.data() : NULL;
	CHECK_CALL(espia_ser_write(m_dev, ptr, &len, block_size, 
				   sec2us(block_delay), !no_wait));
}

void Espia::serRead(string& buffer, int len, double timeout)
{
	buffer.resize(len);
	char *ptr = len ? (char *) buffer.data() : NULL;
	unsigned long ret_len = len;
	CHECK_CALL(espia_ser_read(m_dev, ptr, &ret_len, sec2us(timeout)));
	buffer.resize(ret_len);
}

void Espia::serReadStr(string& buffer, int len, const string& term, 
		       double timeout)
{
	buffer.resize(len);
	char *ptr = len ? (char *) buffer.data() : NULL;
	char *term_ptr = (char *) term.data();
	unsigned long ret_len = len;
	CHECK_CALL(espia_ser_read_str(m_dev, ptr, &ret_len, term_ptr,
				      term.length(), sec2us(timeout)));
	buffer.resize(ret_len);
}

void Espia::serFlush()
{
	CHECK_CALL(espia_ser_flush(m_dev));
}
	
void Espia::throwError(int ret, string file, string func, int line)
{
	string err_desc = string("Espia: ") + espia_strerror(ret);
	throw Exception(Hardware, Error, err_desc, file, func, line);
}
