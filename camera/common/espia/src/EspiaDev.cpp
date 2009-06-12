#include "EspiaDev.h"

using namespace lima;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

EspiaDev::EspiaDev(int dev_nb)
{
	m_dev_nb = Invalid;
	m_dev = ESPIA_DEV_INVAL;

	open(dev_nb);
}

EspiaDev::~EspiaDev()
{
	close();
}

void EspiaDev::open(int dev_nb)
{
	if (dev_nb == m_dev_nb)
		return;

	close();

	CHECK_CALL(espia_open(dev_nb, &m_dev));
	m_dev_nb = dev_nb;
}

void EspiaDev::close()
{
	if (m_dev_nb == Invalid)
		return;

	CHECK_CALL(espia_close(m_dev));

	m_dev = ESPIA_DEV_INVAL;
	m_dev_nb = Invalid;
}

void EspiaDev::registerCallback(struct espia_cb_data& cb_data, int& cb_nr)
{
	CHECK_CALL(espia_register_callback(m_dev, &cb_data, &cb_nr));

	try {
		CHECK_CALL(espia_callback_active(m_dev, cb_nr, true));
	} catch (...) {
		unregisterCallback(cb_nr);
		throw;
	}


}

void EspiaDev::unregisterCallback(int& cb_nr)
{
	CHECK_CALL(espia_unregister_callback(m_dev, cb_nr));

	cb_nr = Invalid;
}


void EspiaDev::serWrite(const string& buffer, int block_size, double block_delay, 
		     bool no_wait)
{
	unsigned long len = buffer.length();
	char *ptr = len ? (char *) buffer.data() : NULL;
	CHECK_CALL(espia_ser_write(m_dev, ptr, &len, block_size, 
				   sec2usec(block_delay), !no_wait));
}

void EspiaDev::serRead(string& buffer, int len, double timeout)
{
	buffer.resize(len);
	char *ptr = len ? (char *) buffer.data() : NULL;
	unsigned long ret_len = len;
	CHECK_CALL(espia_ser_read(m_dev, ptr, &ret_len, sec2usec(timeout)));
	buffer.resize(ret_len);
}

void EspiaDev::serReadStr(string& buffer, int len, const string& term, 
		       double timeout)
{
	buffer.resize(len);
	char *ptr = len ? (char *) buffer.data() : NULL;
	char *term_ptr = (char *) term.data();
	unsigned long ret_len = len;
	CHECK_CALL(espia_ser_read_str(m_dev, ptr, &ret_len, term_ptr,
				      term.length(), sec2usec(timeout)));
	buffer.resize(ret_len);
}

void EspiaDev::serFlush()
{
	CHECK_CALL(espia_ser_flush(m_dev));
}

void EspiaDev::serGetAvailableBytes(int& available_bytes)
{
	unsigned long ret_bytes = 0;
	CHECK_CALL(espia_ser_read(m_dev, NULL, &ret_bytes, 0));
	available_bytes = ret_bytes;
}
	
