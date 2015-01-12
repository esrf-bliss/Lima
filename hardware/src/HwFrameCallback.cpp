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
#include "lima/HwFrameCallback.h"

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
		THROW_HW_ERROR(InvalidValue) << 
			"A FrameCallback is already registered";
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
		THROW_HW_ERROR(InvalidValue) << 
			"Specified FrameCallback is not registered";
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
		THROW_HW_ERROR(InvalidValue) <<  
			"HwFrameCallbackGen is already set";
	} else if (!frame_cb_gen && !m_frame_cb_gen) {
		THROW_HW_ERROR(InvalidValue) << 
			"HwFrameCallbackGen is not set";
	}

	m_frame_cb_gen = frame_cb_gen;
}

void HwFrameCallbackGen::setFrameCallbackActive(bool cb_active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_active);
}
