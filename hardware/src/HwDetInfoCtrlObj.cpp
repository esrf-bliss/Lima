#include "HwDetInfoCtrlObj.h"

using namespace lima;

void 
HwMaxImageSizeCallback::setDetInfoCtrlObj(HwDetInfoCtrlObj *det_info_ctrl_obj)
{
	m_det_info_ctrl_obj = det_info_ctrl_obj;
}

HwDetInfoCtrlObj::~HwDetInfoCtrlObj()
{
	m_max_image_size_cb = NULL;
}

void 
HwDetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback *cb)
{
	if (m_max_image_size_cb) 
		throw LIMA_HW_EXC(InvalidValue, 
				  "An ImageSizeCallback already registered");

	cb->setDetInfoCtrlObj(this);
	m_max_image_size_cb = cb;
	setMaxImageSizeCallbackActive(true);
}

void 
HwDetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback *cb)
{
	if (m_max_image_size_cb != cb) 
		throw LIMA_HW_EXC(InvalidValue, 
				  "ImageSizeCallback is not registered");

	setMaxImageSizeCallbackActive(false);
	m_max_image_size_cb = NULL;
	cb->setDetInfoCtrlObj(NULL);
}

void 
HwDetInfoCtrlObj::maxImageSizeChanged(const Size& size, ImageType image_type)
{
	if (!m_max_image_size_cb)
		return;

	m_max_image_size_cb->maxImageSizeChanged(size, image_type);
}
