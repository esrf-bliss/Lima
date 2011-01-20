//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
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

	if (m_mis_cb)
		THROW_HW_ERROR(InvalidValue) 
			<< "An ImageSizeCallback already registered";

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

	if (m_mis_cb != &cb)
		THROW_HW_ERROR(InvalidValue) 
			<< "Requested ImageSizeCallback not registered";

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

void HwMaxImageSizeCallbackGen::setMaxImageSizeCallbackActive(bool cb_active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_active);
}
