#include "HwFrameCallback.h"

using namespace lima;

HwFrameCallbackGen::HwFrameCallbackGen()
	: m_frame_cb(NULL)
{
	DEB_CONSTRUCTOR();
}

HwFrameCallbackGen::~HwFrameCallbackGen()
{
	DEB_DESTRUCTOR();

	if (m_frame_cb)
		unregisterFrameCallback(*m_frame_cb);
}

void HwFrameCallbackGen::registerFrameCallback(HwFrameCallback& frame_cb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(&frame_cb, m_frame_cb);

	if (m_frame_cb) {
		DEB_ERROR() << "A FrameCallback is already registered";
		throw LIMA_HW_EXC(InvalidValue, 
				  "A FrameCallback is already registered");
	}

	frame_cb.setFrameCallbackGen(this);
	m_frame_cb = &frame_cb;
	setFrameCallbackActive(true);
}

void HwFrameCallbackGen::unregisterFrameCallback(HwFrameCallback& frame_cb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(&frame_cb, m_frame_cb);

	if (m_frame_cb != &frame_cb) { 
		DEB_ERROR() << "Specified FrameCallback is not registered";
		throw LIMA_HW_EXC(InvalidValue, 
				  "Specified FrameCallback is not registered");
	}

	setFrameCallbackActive(false);
	m_frame_cb = NULL;
	frame_cb.setFrameCallbackGen(NULL);
}


bool HwFrameCallbackGen::newFrameReady(const HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();

	if (!m_frame_cb) {
		DEB_TRACE() << "No cb registered";
		return false;
	}

	return m_frame_cb->newFrameReady(frame_info);
}


HwFrameCallback::HwFrameCallback()
	: m_frame_cb_gen(NULL)
{
	DEB_CONSTRUCTOR();
}

HwFrameCallback::~HwFrameCallback()
{
	DEB_DESTRUCTOR();

	if (m_frame_cb_gen)
		m_frame_cb_gen->unregisterFrameCallback(*this);
}

void HwFrameCallback::setFrameCallbackGen(HwFrameCallbackGen *frame_cb_gen)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_cb_gen, m_frame_cb_gen);

	if (frame_cb_gen && m_frame_cb_gen) {
		DEB_ERROR() << "HwFrameCallbackGen is already set";
		throw LIMA_HW_EXC(InvalidValue, 
				  "HwFrameCallbackGen is already set");
	} else if (!frame_cb_gen && !m_frame_cb_gen) {
		DEB_ERROR() << "HwFrameCallbackGen is not set";
		throw LIMA_HW_EXC(InvalidValue, 
				  "HwFrameCallbackGen is not set");
	}

	m_frame_cb_gen = frame_cb_gen;
}

void HwFrameCallbackGen::setFrameCallbackActive(bool cb_active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_active);
}
