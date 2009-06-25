#include "EspiaDev.h"

using namespace lima::Espia;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

const double Dev::ResetLinkTime = 0.5;

Dev::Dev(int dev_nb)
{
	m_dev_nb = Invalid;
	m_dev = ESPIA_DEV_INVAL;

	open(dev_nb);
}

Dev::~Dev()
{
	close();
}

void Dev::open(int dev_nb)
{
	if (dev_nb == m_dev_nb)
		return;

	close();

	CHECK_CALL(espia_open(dev_nb, &m_dev));
	m_dev_nb = dev_nb;
}

void Dev::close()
{
	if (m_dev_nb == Invalid)
		return;

	CHECK_CALL(espia_close(m_dev));

	m_dev = ESPIA_DEV_INVAL;
	m_dev_nb = Invalid;
}

void Dev::registerCallback(struct espia_cb_data& cb_data, int& cb_nr)
{
	CHECK_CALL(espia_register_callback(m_dev, &cb_data, &cb_nr));

	try {
		CHECK_CALL(espia_callback_active(m_dev, cb_nr, true));
	} catch (...) {
		unregisterCallback(cb_nr);
		throw;
	}


}

void Dev::unregisterCallback(int& cb_nr)
{
	CHECK_CALL(espia_unregister_callback(m_dev, cb_nr));

	cb_nr = Invalid;
}

void Dev::resetLink()
{
	CHECK_CALL(espia_reset_link(m_dev));
	Sleep(ResetLinkTime);
}

void Dev::getCcdStatus(int& ccd_status)
{
	unsigned char status;
	CHECK_CALL(espia_ccd_status(m_dev, &status, SCDXIPCI_NO_BLOCK));
	ccd_status = status;
}
