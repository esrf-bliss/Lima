#include "HwDetInfoCtrlObj.h"

using namespace lima;

HwMaxImageSizeCallback::HwMaxImageSizeCallback() 
	: m_det_info_ctrl_obj(NULL) 
{
	DEB_CONSTRUCTOR();
}

HwMaxImageSizeCallback::~HwMaxImageSizeCallback()
{
	DEB_DESTRUCTOR();
}

void 
HwMaxImageSizeCallback::setDetInfoCtrlObj(HwDetInfoCtrlObj *det_info_ctrl_obj)
{
	DEB_MEMBER_FUNCT();
	m_det_info_ctrl_obj = det_info_ctrl_obj;
}


HwDetInfoCtrlObj::HwDetInfoCtrlObj()
	: m_max_image_size_cb(NULL)
{
	DEB_CONSTRUCTOR();
}

HwDetInfoCtrlObj::~HwDetInfoCtrlObj()
{
	DEB_DESTRUCTOR();

	if (m_max_image_size_cb)
		unregisterMaxImageSizeCallback(*m_max_image_size_cb);
}

void 
HwDetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(&cb, m_max_image_size_cb);

	if (m_max_image_size_cb) {
		DEB_ERROR() << "An ImageSizeCallback already registered";
		throw LIMA_HW_EXC(InvalidValue, 
				  "An ImageSizeCallback already registered");
	}

	cb.setDetInfoCtrlObj(this);
	m_max_image_size_cb = &cb;
	setMaxImageSizeCallbackActive(true);
}

void 
HwDetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(&cb, m_max_image_size_cb);

	if (m_max_image_size_cb != &cb) {
		DEB_ERROR() << "Requested ImageSizeCallback not registered";
		throw LIMA_HW_EXC(InvalidValue, 
				  "Req. ImageSizeCallback not registered");
	}

	setMaxImageSizeCallbackActive(false);
	m_max_image_size_cb = NULL;
	cb.setDetInfoCtrlObj(NULL);
}

void 
HwDetInfoCtrlObj::maxImageSizeChanged(const Size& size, ImageType image_type)
{
	DEB_MEMBER_FUNCT();
	if (!m_max_image_size_cb) {
		DEB_TRACE() << "No cb registered";
		return;
	}

	m_max_image_size_cb->maxImageSizeChanged(size, image_type);
}
