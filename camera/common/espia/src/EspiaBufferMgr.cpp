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
#include "EspiaBufferMgr.h"
#include "MemUtils.h"
#include <sstream>

using namespace lima;
using namespace lima::Espia;
using namespace std;


BufferMgr::BufferMgr(Acq& acq) 
	: m_acq(acq), m_frame_cb(*this)
{
	DEB_CONSTRUCTOR();
       
	Dev& dev = acq.getDev();
	DEB_PARAM() << DEB_VAR1(dev.getDevNb());

	ostringstream os;
	if (dev.isMeta())
		os << "MetaBufferMgr";
	else
		os << "BufferMgr#" << dev.getDevNb();
	DEB_SET_OBJ_NAME(os.str());
}

BufferMgr::~BufferMgr() 
{
	DEB_DESTRUCTOR();
}

BufferMgr::Cap BufferMgr::getCap()
{
	return Basic | Concat;
}

int BufferMgr::getMaxNbBuffers(const FrameDim& frame_dim,
			       int nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_dim, nb_concat_frames);

	FrameDim buffer_frame_dim;
	getBufferFrameDim(frame_dim, nb_concat_frames, buffer_frame_dim);
	DEB_TRACE() << DEB_VAR1(buffer_frame_dim);

	int max_nb_buffers = GetDefMaxNbBuffers(buffer_frame_dim);
	DEB_RETURN() << DEB_VAR1(max_nb_buffers);
	return max_nb_buffers;
}

void BufferMgr::allocBuffers(int nb_buffers, int nb_concat_frames, 
			     const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	m_acq.bufferAlloc(nb_buffers, nb_concat_frames, frame_dim);
}

const FrameDim& BufferMgr::getFrameDim()
{
	DEB_MEMBER_FUNCT();
	return m_acq.getFrameDim();
}

void BufferMgr::getNbBuffers(int& nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_acq.getNbBuffers(nb_buffers);
}

void BufferMgr::getNbConcatFrames(int& nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	m_acq.getNbBufferFrames(nb_concat_frames);
}

void BufferMgr::releaseBuffers()
{
	DEB_MEMBER_FUNCT();
	m_acq.bufferFree();
}

void *BufferMgr::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	DEB_MEMBER_FUNCT();
	return m_acq.getBufferFramePtr(buffer_nb, concat_frame_nb);
}

void BufferMgr::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	DEB_MEMBER_FUNCT();
	m_acq.getFrameInfo(acq_frame_nb, info);
}

void BufferMgr::setStartTimestamp(Timestamp  start_ts)
{
	DEB_MEMBER_FUNCT();
}

void BufferMgr::getStartTimestamp(Timestamp& start_ts)
{
	DEB_MEMBER_FUNCT();
	m_acq.getStartTimestamp(start_ts);
}

void BufferMgr::setFrameCallbackActive(bool cb_active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_active);

	if (cb_active)
		m_acq.registerFrameCallback(m_frame_cb);
	else
		m_acq.unregisterFrameCallback(m_frame_cb);
}
	

BufferMgr::FrameCallback::FrameCallback(BufferMgr& buffer_mgr) 
	: m_buffer_mgr(buffer_mgr) 
{
	DEB_CONSTRUCTOR();
}

BufferMgr::FrameCallback::~FrameCallback() 
{
	DEB_DESTRUCTOR();
}

bool BufferMgr::FrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();
	return m_buffer_mgr.newFrameReady(frame_info);
}
		
