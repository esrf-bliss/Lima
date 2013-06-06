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
#include "HwBufferMgr.h"

#include <cstring>

using namespace lima;

/*******************************************************************
 * \brief BufferAllocMgr destructor
 *******************************************************************/

BufferAllocMgr::BufferAllocMgr()
{
	DEB_CONSTRUCTOR();
}

BufferAllocMgr::~BufferAllocMgr()
{
	DEB_DESTRUCTOR();
}

void BufferAllocMgr::clearBuffer(int buffer_nb)
{
	ClearBuffer(getBufferPtr(buffer_nb), 1, getFrameDim());
}

void BufferAllocMgr::clearAllBuffers()
{
	DEB_MEMBER_FUNCT();
	int nb_buffers;
	getNbBuffers(nb_buffers);
	DEB_TRACE() << "Clearing " << DEB_VAR1(nb_buffers) << " buffers";
	for (int i = 0; i < nb_buffers; i++)
		clearBuffer(i);
}


/*******************************************************************
 * \brief SoftBufferAllocMgr constructor
 *******************************************************************/

SoftBufferAllocMgr::SoftBufferAllocMgr()
{
	DEB_CONSTRUCTOR();
}

SoftBufferAllocMgr::~SoftBufferAllocMgr()
{
	DEB_DESTRUCTOR();
	releaseBuffers();
}

int SoftBufferAllocMgr::getMaxNbBuffers(const FrameDim& frame_dim)
{
	return GetDefMaxNbBuffers(frame_dim);
}

void SoftBufferAllocMgr::allocBuffers(int nb_buffers,
				      const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(nb_buffers, frame_dim);

	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0) 
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_dim);
       
	int max_buffers = getMaxNbBuffers(frame_dim);
	if ((nb_buffers < 1) || (nb_buffers > max_buffers)) 
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(nb_buffers);

	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if ((frame_dim == m_frame_dim) && (nb_buffers == curr_nb_buffers)) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	releaseBuffers();

	try {
		m_buffer_list.reserve(nb_buffers);
		for (int i = 0; i < nb_buffers; ++i) {
			MemBuffer *buffer = new MemBuffer(frame_size);
			m_buffer_list.push_back(buffer);
		}
	} catch (...) {
		DEB_ERROR() << "Error alloc. buffer #" << m_buffer_list.size();
		releaseBuffers();
		throw;
	}

	m_frame_dim = frame_dim;
}

void SoftBufferAllocMgr::releaseBuffers()
{
	DEB_MEMBER_FUNCT();

	BufferList& bl = m_buffer_list;
	for (BufferListCIt it = bl.begin(); it != bl.end(); ++it)
		delete *it;
	bl.clear();
	m_frame_dim = FrameDim();
}

const FrameDim& SoftBufferAllocMgr::getFrameDim()
{
	DEB_MEMBER_FUNCT();
	DEB_RETURN() << DEB_VAR1(m_frame_dim);
	return m_frame_dim;
}

void SoftBufferAllocMgr::getNbBuffers(int& nb_buffers)
{
	DEB_MEMBER_FUNCT();
	nb_buffers = m_buffer_list.size();
	DEB_RETURN() << DEB_VAR1(nb_buffers);
}

void *SoftBufferAllocMgr::getBufferPtr(int buffer_nb)
{
	DEB_MEMBER_FUNCT();
	void *ptr = m_buffer_list[buffer_nb]->getPtr();
	DEB_RETURN() << DEB_VAR1(ptr);
	return ptr;
}


/*******************************************************************
 * \brief BufferCbMgr destructor
 *******************************************************************/

BufferCbMgr::BufferCbMgr()
{
	DEB_CONSTRUCTOR();
}

BufferCbMgr::~BufferCbMgr()
{
	DEB_DESTRUCTOR();
}

void BufferCbMgr::clearBuffer(int buffer_nb)
{
	int nb_concat_frames;
	getNbConcatFrames(nb_concat_frames);
	ClearBuffer(getBufferPtr(buffer_nb, 0), nb_concat_frames, 
		    getFrameDim());
}

void BufferCbMgr::clearAllBuffers()
{
	DEB_MEMBER_FUNCT();
	int nb_buffers;
	getNbBuffers(nb_buffers);
	DEB_TRACE() << "Clearing " << DEB_VAR1(nb_buffers) << " buffers";
	for (int i = 0; i < nb_buffers; i++)
		clearBuffer(i);
}

void BufferCbMgr::setStartTimestamp(Timestamp start_ts)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(start_ts);

	if (!start_ts.isSet())
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(start_ts);

	m_start_ts = start_ts;
}

void BufferCbMgr::getStartTimestamp(Timestamp& start_ts) 
{
	DEB_MEMBER_FUNCT();
	start_ts = m_start_ts;
	DEB_RETURN() << DEB_VAR1(start_ts);
}

void BufferCbMgr::getBufferFrameDim(const FrameDim& single_frame_dim,
				    int nb_concat_frames, 
				    FrameDim& buffer_frame_dim)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(single_frame_dim, nb_concat_frames);

	if (nb_concat_frames < 1) 
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(nb_concat_frames);

	buffer_frame_dim = single_frame_dim;
	Size buffer_size = buffer_frame_dim.getSize();
	buffer_size *= Point(1, nb_concat_frames);
	buffer_frame_dim.setSize(buffer_size);

	DEB_RETURN() << DEB_VAR1(buffer_frame_dim);
}
 
void BufferCbMgr::acqFrameNb2BufferNb(int acq_frame_nb, int& buffer_nb,
				      int& concat_frame_nb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(acq_frame_nb);

	int nb_buffers, nb_concat_frames;
	getNbBuffers(nb_buffers);
	getNbConcatFrames(nb_concat_frames);

	if((nb_buffers<1) || (nb_concat_frames < 1)){
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR2(nb_buffers, nb_concat_frames);
	}

	buffer_nb = (acq_frame_nb / nb_concat_frames) % nb_buffers;
	concat_frame_nb = acq_frame_nb % nb_concat_frames;

	DEB_PARAM() << DEB_VAR2(buffer_nb, concat_frame_nb);
	
}

BufferCbMgr::Cap lima::operator |(BufferCbMgr::Cap c1, BufferCbMgr::Cap c2)
{
	return BufferCbMgr::Cap(int(c1) | int(c2));
}

BufferCbMgr::Cap lima::operator &(BufferCbMgr::Cap c1, BufferCbMgr::Cap c2)
{
	return BufferCbMgr::Cap(int(c1) & int(c2));
}



/*******************************************************************
 * \brief StdBufferCbMgr constructor
 *******************************************************************/

StdBufferCbMgr::StdBufferCbMgr(BufferAllocMgr& alloc_mgr)
	: m_alloc_mgr(&alloc_mgr)

{
	DEB_CONSTRUCTOR();
	m_nb_concat_frames = 1;
	m_fcb_act = false;
}

StdBufferCbMgr::~StdBufferCbMgr()
{
	DEB_DESTRUCTOR();
}

BufferCbMgr::Cap StdBufferCbMgr::getCap()
{
	return Basic | Concat;
}

int StdBufferCbMgr::getMaxNbBuffers(const FrameDim& frame_dim, 
				    int nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	FrameDim buffer_frame_dim;
	getBufferFrameDim(frame_dim, nb_concat_frames, buffer_frame_dim);
	int max_nb_buffers = m_alloc_mgr->getMaxNbBuffers(buffer_frame_dim);
	DEB_RETURN() << DEB_VAR1(max_nb_buffers);
	return max_nb_buffers;
}

void StdBufferCbMgr::allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR3(nb_buffers, nb_concat_frames, frame_dim);

	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0) {
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_dim);
	} else if (nb_concat_frames < 1) {
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(nb_concat_frames);
	}

	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if ((nb_buffers == curr_nb_buffers) && (frame_dim == m_frame_dim) && 
	    (nb_concat_frames == m_nb_concat_frames)) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	releaseBuffers();

	try {
		FrameDim buffer_frame_dim;
		getBufferFrameDim(frame_dim, nb_concat_frames, 
				  buffer_frame_dim);

		DEB_TRACE() << "Allocating buffers";
		m_alloc_mgr->allocBuffers(nb_buffers, buffer_frame_dim);
		m_frame_dim = frame_dim;
		m_nb_concat_frames = nb_concat_frames;

		DEB_TRACE() << "Allocating frame info list";
		int nb_frames = nb_buffers * nb_concat_frames;
		m_info_list.reserve(nb_frames);
		for (int i = 0; i < nb_frames; ++i)
			m_info_list.push_back(HwFrameInfoType());
	} catch (...) {
		releaseBuffers();
		throw;
	}
}

void StdBufferCbMgr::releaseBuffers()
{
	DEB_MEMBER_FUNCT();

	m_alloc_mgr->releaseBuffers();
	m_info_list.clear();
	m_nb_concat_frames = 1;
	m_frame_dim = FrameDim();
}

void StdBufferCbMgr::setFrameCallbackActive(bool cb_active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_active);
	m_fcb_act = cb_active;
}

bool StdBufferCbMgr::newFrameReady(HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();

	Timestamp now = Timestamp::now();
	Timestamp start;
	getStartTimestamp(start);
	if (!frame_info.frame_timestamp.isSet())
		frame_info.frame_timestamp = now - start;

        int buffer_nb, concat_frame_nb;
	acqFrameNb2BufferNb(frame_info.acq_frame_nb, buffer_nb,
			    concat_frame_nb);
	void *ptr = getBufferPtr(buffer_nb, concat_frame_nb);
	if (!frame_info.frame_ptr) {
		frame_info.frame_ptr = ptr;
	} else if (frame_info.frame_ptr != ptr) {
	  	DEB_ERROR() << "Invalid " << DEB_VAR1(frame_info.frame_ptr);
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_info.frame_ptr);
	}

	const FrameDim& frame_dim = getFrameDim();
	if (!frame_info.frame_dim.isValid()) {
		frame_info.frame_dim = frame_dim;
	} else if (frame_info.frame_dim != frame_dim) {
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_info.frame_dim);
	}
	if (frame_info.valid_pixels == 0)
		frame_info.valid_pixels = Point(frame_dim.getSize()).getArea();

	int frame_nb = buffer_nb * m_nb_concat_frames + concat_frame_nb;
	m_info_list[frame_nb] = frame_info;

	if (!m_fcb_act) {
		DEB_TRACE() << "No cb registered";
		return false;
	}

	return HwFrameCallbackGen::newFrameReady(frame_info);
}

const FrameDim& StdBufferCbMgr::getFrameDim()
{
	DEB_MEMBER_FUNCT();
	DEB_RETURN() << DEB_VAR1(m_frame_dim);
	return m_frame_dim;
}

void StdBufferCbMgr::getNbBuffers(int& nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_alloc_mgr->getNbBuffers(nb_buffers);
	DEB_RETURN() << DEB_VAR1(nb_buffers);
}

void StdBufferCbMgr::getNbConcatFrames(int& nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	nb_concat_frames = m_nb_concat_frames;
	DEB_RETURN() << DEB_VAR1(nb_concat_frames);
}

void *StdBufferCbMgr::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	if (concat_frame_nb >= m_nb_concat_frames)
		throw LIMA_HW_EXC(InvalidValue, "Invalid ") 
			<< DEB_VAR2(concat_frame_nb, m_nb_concat_frames);

	char *ptr = (char *) m_alloc_mgr->getBufferPtr(buffer_nb);
	return ptr + concat_frame_nb * m_frame_dim.getMemSize();
}

void* StdBufferCbMgr::getFrameBufferPtr(int frame_nb)
{
  int buffer_nb, concat_frame_nb;
  acqFrameNb2BufferNb(frame_nb, buffer_nb,concat_frame_nb);
  return getBufferPtr(buffer_nb,concat_frame_nb);
}

void StdBufferCbMgr::clearBuffer(int buffer_nb)
{
	m_alloc_mgr->clearBuffer(buffer_nb);
}

void StdBufferCbMgr::clearAllBuffers()
{
	DEB_MEMBER_FUNCT();
	m_alloc_mgr->clearAllBuffers();
}

void StdBufferCbMgr::getFrameInfo(int acq_frame_nb, HwFrameInfo& info)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(acq_frame_nb);

	int nb_buffers;
	getNbBuffers(nb_buffers);
	int nb_frames = nb_buffers * m_nb_concat_frames;
	int frame_nb = acq_frame_nb % nb_frames;
	if (m_info_list[frame_nb].acq_frame_nb != acq_frame_nb)
		THROW_HW_ERROR(Error) << "Frame " << acq_frame_nb 
				      << " not available";

	info = m_info_list[frame_nb];
	DEB_RETURN() << DEB_VAR1(info);
}

/*******************************************************************
 * \brief BufferCtrlMgr constructor
 *******************************************************************/

BufferCtrlMgr::BufferCtrlMgr(BufferCbMgr& acq_buffer_mgr)
	: m_nb_concat_frames(1),
	  m_acq_buffer_mgr(&acq_buffer_mgr), 
	  m_frame_cb(*this),
	  m_frame_cb_act(false)
{
	DEB_CONSTRUCTOR();

	m_acq_buffer_mgr->registerFrameCallback(m_frame_cb);
}

BufferCtrlMgr::~BufferCtrlMgr()
{
	DEB_DESTRUCTOR();
}

void BufferCtrlMgr::releaseBuffers()
{
	DEB_MEMBER_FUNCT();
	m_acq_buffer_mgr->releaseBuffers();
}

void BufferCtrlMgr::setFrameDim(const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_dim, m_frame_dim);

	if (frame_dim == m_frame_dim) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	releaseBuffers();
	m_frame_dim = frame_dim;
}

void BufferCtrlMgr::getFrameDim(FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	frame_dim = m_acq_buffer_mgr->getFrameDim();
	DEB_RETURN() << DEB_VAR1(frame_dim);
}

void BufferCtrlMgr::setNbConcatFrames(int nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_concat_frames);

	bool ask_concat = (nb_concat_frames > 1);
	if ((getAcqMode() == Acc) && ask_concat)
		THROW_HW_ERROR(InvalidValue) << "Frame acc. is active";

	bool can_concat = !!(m_acq_buffer_mgr->getCap() & BufferCbMgr::Concat);
	if (ask_concat && !can_concat)
		THROW_HW_ERROR(NotSupported) << "Stripe concat. not supported";

	if (nb_concat_frames != m_nb_concat_frames)
		releaseBuffers();

	m_nb_concat_frames = nb_concat_frames;
}

void BufferCtrlMgr::getNbConcatFrames(int& nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	nb_concat_frames = m_nb_concat_frames;
	DEB_RETURN() << DEB_VAR1(nb_concat_frames);
}

void BufferCtrlMgr::setNbBuffers(int nb_buffers)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_buffers);

	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if (nb_buffers == curr_nb_buffers) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	int max_nb_buffers;
	getMaxNbBuffers(max_nb_buffers);
	if ((nb_buffers > 0) && (nb_buffers > max_nb_buffers)) {
		THROW_HW_ERROR(InvalidValue) << "Too many buffers:" 
					     << DEB_VAR1(nb_buffers);
	} else if (nb_buffers == 0)
		nb_buffers = max_nb_buffers;

	releaseBuffers();

	int acq_nb_buffers = nb_buffers;
	DEB_TRACE() << DEB_VAR1(acq_nb_buffers);

	m_acq_buffer_mgr->allocBuffers(acq_nb_buffers, m_nb_concat_frames, 
					m_frame_dim);
}

void BufferCtrlMgr::getNbBuffers(int& nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_acq_buffer_mgr->getNbBuffers(nb_buffers);
	DEB_RETURN() << DEB_VAR1(nb_buffers);
}

void BufferCtrlMgr::getMaxNbBuffers(int& max_nb_buffers)
{
	DEB_MEMBER_FUNCT();
	int concat_frames = m_nb_concat_frames;
	max_nb_buffers = m_acq_buffer_mgr->getMaxNbBuffers(m_frame_dim,
							      concat_frames);
	DEB_RETURN() << DEB_VAR1(max_nb_buffers);
}

void BufferCtrlMgr::setFrameCallbackActive(bool cb_active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_active);
	m_frame_cb_act = cb_active;
}

BufferCbMgr& BufferCtrlMgr::getAcqBufferMgr()
{
	DEB_MEMBER_FUNCT();
	return *m_acq_buffer_mgr;
}

void BufferCtrlMgr::setStartTimestamp(Timestamp start_ts)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(start_ts);

	m_acq_buffer_mgr->setStartTimestamp(start_ts);
}

void BufferCtrlMgr::getStartTimestamp(Timestamp& start_ts)
{
	DEB_MEMBER_FUNCT();
	m_acq_buffer_mgr->getStartTimestamp(start_ts);
	DEB_RETURN() << DEB_VAR1(start_ts);
}

void BufferCtrlMgr::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(acq_frame_nb);
	m_acq_buffer_mgr->getFrameInfo(acq_frame_nb, info);
}

void *BufferCtrlMgr::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(buffer_nb, concat_frame_nb);

	void *ptr;
	ptr = m_acq_buffer_mgr->getBufferPtr(buffer_nb, concat_frame_nb);
	DEB_RETURN() << DEB_VAR1(ptr);
	return ptr;
}

void *BufferCtrlMgr::getFramePtr(int acq_frame_nb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(acq_frame_nb);

	HwFrameInfoType info;
	getFrameInfo(acq_frame_nb, info);
	void *ptr = info.frame_ptr;
	DEB_RETURN() << DEB_VAR1(ptr);
	return ptr;
}

BufferCtrlMgr::AcqMode BufferCtrlMgr::getAcqMode()
{
	DEB_MEMBER_FUNCT();
	AcqMode acq_mode;
	if (m_nb_concat_frames > 1)
		acq_mode = Concat;
	else
		acq_mode = Normal;

	DEB_RETURN() << DEB_VAR1(acq_mode);
	return acq_mode;
}

bool BufferCtrlMgr::acqFrameReady(const HwFrameInfoType& acq_frame_info)
{
  DEB_MEMBER_FUNCT();
  bool aReturnFlag = true;
  if (m_frame_cb_act)
    aReturnFlag = newFrameReady(acq_frame_info);
  return aReturnFlag;
}

/*******************************************************************
 * \brief BufferCtrlMgr::AcqFrameCallback constructor
 *******************************************************************/

BufferCtrlMgr::AcqFrameCallback::AcqFrameCallback(BufferCtrlMgr& buffer_mgr)
	: m_buffer_mgr(&buffer_mgr) 
{
	DEB_CONSTRUCTOR();
}

BufferCtrlMgr::AcqFrameCallback::~AcqFrameCallback()
{
	DEB_DESTRUCTOR();
}

bool
BufferCtrlMgr::AcqFrameCallback::newFrameReady(const HwFrameInfoType& finfo)
{
	DEB_MEMBER_FUNCT();
	return m_buffer_mgr->acqFrameReady(finfo);
}

/*****************************************************************************
			  SoftBufferCtrlObj
****************************************************************************/

SoftBufferCtrlObj::SoftBufferCtrlObj() 
	: HwBufferCtrlObj(), 
	  m_buffer_cb_mgr(m_buffer_alloc_mgr), m_mgr(m_buffer_cb_mgr), 
	  m_acq_frame_nb(-1), m_buffer_callback(NULL)
{
}

void SoftBufferCtrlObj::setFrameDim(const FrameDim& frame_dim) 
{
	m_mgr.setFrameDim(frame_dim);
}

void SoftBufferCtrlObj::getFrameDim(FrameDim& frame_dim) 
{
	m_mgr.getFrameDim(frame_dim);
}

void SoftBufferCtrlObj::setNbBuffers(int  nb_buffers) 
{
	m_mgr.setNbBuffers(nb_buffers);
}

void SoftBufferCtrlObj::getNbBuffers(int& nb_buffers)
{
	m_mgr.getNbBuffers(nb_buffers);
}

void SoftBufferCtrlObj::setNbConcatFrames(int nb_concat_frames) 
{
	m_mgr.setNbConcatFrames(nb_concat_frames);
}

void SoftBufferCtrlObj::getNbConcatFrames(int& nb_concat_frames) 
{
	m_mgr.getNbConcatFrames(nb_concat_frames);
}

void SoftBufferCtrlObj::getMaxNbBuffers(int& max_nb_buffers) 
{
	m_mgr.getMaxNbBuffers(max_nb_buffers);
}

void *SoftBufferCtrlObj::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	return m_mgr.getBufferPtr(buffer_nb,concat_frame_nb);
}

void *SoftBufferCtrlObj::getFramePtr(int acq_frame_nb)
{
	return m_mgr.getFramePtr(acq_frame_nb);
}

void SoftBufferCtrlObj::getStartTimestamp(Timestamp& start_ts) 
{
	m_mgr.getStartTimestamp(start_ts);
}

void SoftBufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	m_mgr.getFrameInfo(acq_frame_nb,info);
}

void SoftBufferCtrlObj::registerFrameCallback(HwFrameCallback& frame_cb)
{
	m_mgr.registerFrameCallback(frame_cb);
}

void SoftBufferCtrlObj::unregisterFrameCallback(HwFrameCallback& frame_cb) 
{
	m_mgr.unregisterFrameCallback(frame_cb);
}

StdBufferCbMgr& SoftBufferCtrlObj::getBuffer() 
{
	return m_buffer_cb_mgr;
}

int SoftBufferCtrlObj::getNbAcquiredFrames() 
{
	return m_acq_frame_nb + 1;
}

SoftBufferCtrlObj::Sync* SoftBufferCtrlObj::getBufferSync(Cond& cond)
{
	if(!m_buffer_callback)
		m_buffer_callback = new Sync(*this, cond);
	return (SoftBufferCtrlObj::Sync*) m_buffer_callback;
}

HwBufferCtrlObj::Callback* SoftBufferCtrlObj::getBufferCallback() 
{
	return m_buffer_callback;
}

SoftBufferCtrlObj::Sync::Sync(SoftBufferCtrlObj& buffer_ctrl_obj, Cond& cond) 
	: m_cond(cond), m_buffer_ctrl_obj(buffer_ctrl_obj)
{
	DEB_CONSTRUCTOR();
}

SoftBufferCtrlObj::Sync::~Sync() 
{
	DEB_DESTRUCTOR();
}

// Important: must be called with the cond.mutex locked!
SoftBufferCtrlObj::Sync::Status 
SoftBufferCtrlObj::Sync::wait(int frame_number, double timeout)
{
	DEB_MEMBER_FUNCT();

	StdBufferCbMgr& buffer_mgr = m_buffer_ctrl_obj.getBuffer();
	void *framePtr = buffer_mgr.getFrameBufferPtr(frame_number);
	BufferList::iterator it = m_buffer_in_use.find(framePtr);
	if (it == m_buffer_in_use.end())
		return AVAILABLE;

	bool okFlag = m_cond.wait(timeout);
	DEB_TRACE() << DEB_VAR1(okFlag);

    it = m_buffer_in_use.find(framePtr);
	if (it == m_buffer_in_use.end())
		return AVAILABLE;
 	else
		return okFlag ? INTERRUPTED : TIMEOUT;
}

void SoftBufferCtrlObj::Sync::map(void *address)
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	m_buffer_in_use.insert(address);
}

void SoftBufferCtrlObj::Sync::release(void *address)
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	BufferList::iterator it = m_buffer_in_use.find(address);
	if (it == m_buffer_in_use.end())
		THROW_HW_ERROR(Error) << "Internal error: releasing buffer not in used list";

	m_buffer_in_use.erase(it);
	it = m_buffer_in_use.find(address);
	if (it == m_buffer_in_use.end())
		m_cond.broadcast();
}

void SoftBufferCtrlObj::Sync::releaseAll()
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	m_buffer_in_use.clear();
	m_cond.broadcast();
}
