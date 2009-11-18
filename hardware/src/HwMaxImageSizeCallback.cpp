#include "HwMaxImageSizeCallback.h"

using namespace lima;

HwMaxImageSizeCallback::HwMaxImageSizeCallback() 
	: m_mis_cb_gen(NULL) 
{
	DEB_CONSTRUCTOR();
}

HwMaxImageSizeCallback::~HwMaxImageSizeCallback()
{
	DEB_DESTRUCTOR();

	if (m_mis_cb_gen)
		m_mis_cb_gen->unregisterMaxImageSizeCallback(*this);
}

void 
HwMaxImageSizeCallback::setMaxImageSizeCallbackGen(
				HwMaxImageSizeCallbackGen *mis_cb_gen)
{
	DEB_MEMBER_FUNCT();
	m_mis_cb_gen = mis_cb_gen;
}


HwMaxImageSizeCallbackGen::HwMaxImageSizeCallbackGen()
	: m_mis_cb(NULL)
{
	DEB_CONSTRUCTOR();
}

HwMaxImageSizeCallbackGen::~HwMaxImageSizeCallbackGen()
{
	DEB_DESTRUCTOR();
	if (m_mis_cb)
		unregisterMaxImageSizeCallback(*m_mis_cb);
}

void 
HwMaxImageSizeCallbackGen::registerMaxImageSizeCallback(
				HwMaxImageSizeCallback& cb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(&cb, m_mis_cb);

	if (m_mis_cb) {
		DEB_ERROR() << "An ImageSizeCallback already registered";
		throw LIMA_HW_EXC(InvalidValue, 
				  "An ImageSizeCallback already registered");
	}

	cb.setMaxImageSizeCallbackGen(this);
	m_mis_cb = &cb;
	setMaxImageSizeCallbackActive(true);
}

void 
HwMaxImageSizeCallbackGen::unregisterMaxImageSizeCallback(
				HwMaxImageSizeCallback& cb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(&cb, m_mis_cb);

	if (m_mis_cb != &cb) {
		DEB_ERROR() << "Requested ImageSizeCallback not registered";
		throw LIMA_HW_EXC(InvalidValue, 
				  "Req. ImageSizeCallback not registered");
	}

	setMaxImageSizeCallbackActive(false);
	m_mis_cb = NULL;
	cb.setMaxImageSizeCallbackGen(NULL);
}

void 
HwMaxImageSizeCallbackGen::maxImageSizeChanged(const Size& size, 
					       ImageType image_type)
{
	DEB_MEMBER_FUNCT();
	if (!m_mis_cb) {
		DEB_TRACE() << "No cb registered";
		return;
	}

	m_mis_cb->maxImageSizeChanged(size, image_type);
}

