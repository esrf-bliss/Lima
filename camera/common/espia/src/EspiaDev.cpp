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

