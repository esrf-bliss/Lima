#include "HwFrameCallback.h"

using namespace lima;

HwFrameCallbackGen::HwFrameCallbackGen()
{
	m_frame_cb = NULL;
}

HwFrameCallbackGen::~HwFrameCallbackGen()
{
	if (m_frame_cb != NULL)
		m_frame_cb->setFrameCallbackGen(NULL);
}

HwFrameCallback::~HwFrameCallback()
{
	if (m_frame_cb_gen != NULL)
		m_frame_cb_gen->unregisterFrameCallback(this);
}

void HwFrameCallbackGen::registerFrameCallback(HwFrameCallback *frame_cb)
{
	if (m_frame_cb != NULL) 
		throw LIMA_HW_EXC(InvalidValue, 
				  "A FrameCallback is already registered");

	frame_cb->setFrameCallbackGen(this);
	m_frame_cb = frame_cb;
	setFrameCallbackActive(true);
}

void HwFrameCallbackGen::unregisterFrameCallback(HwFrameCallback *frame_cb)
{
	if (m_frame_cb != frame_cb) 
		throw LIMA_HW_EXC(InvalidValue, 
				  "Specified FrameCallback is not registered");

	setFrameCallbackActive(false);
	m_frame_cb = NULL;
	frame_cb->setFrameCallbackGen(NULL);
}


void HwFrameCallback::setFrameCallbackGen(HwFrameCallbackGen *frame_cb_gen)
{
	if (frame_cb_gen && m_frame_cb_gen)
		throw LIMA_HW_EXC(InvalidValue, 
				  "HwFrameCallbackGen is already set");
	else if (!frame_cb_gen && !m_frame_cb_gen)
		throw LIMA_HW_EXC(InvalidValue, 
				  "HwFrameCallbackGen is already reset");

	m_frame_cb_gen = frame_cb_gen;
}
