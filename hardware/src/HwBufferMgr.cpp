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
#include "lima/HwBufferMgr.h"

#include <cstring>
#if !defined(_WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef __unix
#include <unistd.h>
#else
#include <processlib/win/unistd.h>
#endif
#include <sys/mman.h>
#endif

using namespace lima;

/*******************************************************************
 * BufferAllocMgr
 *******************************************************************/

BufferAllocMgr::BufferAllocMgr()
{
	DEB_CONSTRUCTOR();
}

BufferAllocMgr::~BufferAllocMgr()
{
	DEB_DESTRUCTOR();
}

void BufferAllocMgr::setAllocParameters(const AllocParameters& alloc_params)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(alloc_params);
	AllocParameters default_params;
	if (alloc_params != default_params)
		THROW_HW_ERROR(NotSupported) << "Setting non-default buffer "
					     << "alloc params is not supported";
}

void BufferAllocMgr::getAllocParameters(AllocParameters& alloc_params)
{
	DEB_MEMBER_FUNCT();
	alloc_params = AllocParameters();
	DEB_RETURN() << DEB_VAR1(alloc_params);
}

void BufferAllocMgr::prepareAlloc(int nb_buffers, const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
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
 * SoftBufferAllocMgr
 *******************************************************************/

class SoftBufferAllocMgr::DefAllocChangeCb :
	public AllocatorFactory::DefaultChangeCallback
{
public:
	DefAllocChangeCb(SoftBufferAllocMgr& mgr) : m_mgr(mgr)
	{
	}

	virtual void onDefaultAllocatorChange(Allocator::Ref prev_alloc,
					      Allocator::Ref new_alloc)
	{
		m_mgr.onDefaultAllocatorChange(prev_alloc, new_alloc);
	}

private:
	SoftBufferAllocMgr& m_mgr;
};

SoftBufferAllocMgr::SoftBufferAllocMgr()
	: m_def_alloc_change_cb(new DefAllocChangeCb(*this))
{
	DEB_CONSTRUCTOR();
	AllocParameters params;
	params.initMem = true;
	setAllocParameters(params);
}

SoftBufferAllocMgr::~SoftBufferAllocMgr()
{
	DEB_DESTRUCTOR();
	releaseBuffers();
}

void SoftBufferAllocMgr::setAllocParameters(const AllocParameters& params)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(params);
	if (!params.initMem)
		DEB_WARNING() << "Memory buffers won't be initialized!";
	m_buffer_helper.setParameters(params);
}

void SoftBufferAllocMgr::getAllocParameters(AllocParameters& params)
{
	DEB_MEMBER_FUNCT();
	m_buffer_helper.getParameters(params);
	DEB_RETURN() << DEB_VAR1(params);
}

void SoftBufferAllocMgr::prepareAlloc(int nb_buffers, const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	m_buffer_helper.prepareBuffers(nb_buffers, frame_dim.getMemSize());
}

void SoftBufferAllocMgr::onDefaultAllocatorChange(Allocator::Ref prev_alloc,
						  Allocator::Ref new_alloc)
{
	DEB_MEMBER_FUNCT();
	DEB_ALWAYS() << "SoftBufferAllocmgr: Default allocator changed!";
	DEB_ALWAYS() << "prev_alloc: " << prev_alloc->toString();
	DEB_ALWAYS() << "new_alloc:  " << new_alloc->toString();

#ifdef LIMA_USE_NUMA
	NumaAllocator *numa_alloc = dynamic_cast<NumaAllocator *>(new_alloc.get());
	if (numa_alloc) {
		CPUMask mask = numa_alloc->getCPUAffinityMask();
		DEB_ALWAYS() << " Numa mask: " << NumaNodeMask::fromCPUMask(mask);
	}
#endif
	AllocParameters params;
	getAllocParameters(params);
	if (!params.allocator)
		releaseBuffers();
}

int SoftBufferAllocMgr::getMaxNbBuffers(const FrameDim& frame_dim)
{
	AllocParameters params;
	getAllocParameters(params);
	return params.getDefMaxNbBuffers(frame_dim.getMemSize());
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

	if (frame_dim != m_frame_dim)
		releaseBuffers();

	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	int to_alloc = nb_buffers - curr_nb_buffers;
	if (to_alloc == 0) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	try {
		BufferList& bl = m_buffer_list;
		bl.resize(nb_buffers);
		if (to_alloc > 0) {			
			DEB_TRACE() << "Allocating " << to_alloc << " buffers";
			for (int i = curr_nb_buffers; i < nb_buffers; i++)
				bl[i] = m_buffer_helper.getBuffer(frame_size);
		} else {
			DEB_TRACE() << "Releasing " << -to_alloc << " buffers";
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
	nb_buffers = int(m_buffer_list.size());
	DEB_RETURN() << DEB_VAR1(nb_buffers);
}

void *SoftBufferAllocMgr::getBufferPtr(int buffer_nb)
{
	DEB_MEMBER_FUNCT();
	void *ptr = m_buffer_list[buffer_nb].get();
	DEB_RETURN() << DEB_VAR1(ptr);
	return ptr;
}


/*******************************************************************
 * NumaSoftBufferAllocMgr
 *******************************************************************/

#ifdef LIMA_USE_NUMA

NumaSoftBufferAllocMgr::NumaSoftBufferAllocMgr()
{
	DEB_CONSTRUCTOR();
	setCPUAffinityMask({});
}

NumaSoftBufferAllocMgr::~NumaSoftBufferAllocMgr()
{
	DEB_DESTRUCTOR();
	setAllocator(Allocator::Ref());
}

void NumaSoftBufferAllocMgr::setCPUAffinityMask(const CPUMask& mask)
{
	DEB_MEMBER_FUNCT();
	if (DEB_CHECK_ANY(DebTypeParam))
		DEB_PARAM() << mask;

	NumaAllocator *numa_allocator = getNumaAllocator();
	if (numa_allocator &&
	    (mask == numa_allocator->getCPUAffinityMask()))
		return;

	FrameDim frame_dim = getFrameDim();
	int nb_buffers;
	getNbBuffers(nb_buffers);
	if (nb_buffers > 0)
		releaseBuffers();

	Allocator::Ref allocator = std::make_shared<NumaAllocator>(mask);
	setAllocator(allocator);

	if (nb_buffers > 0)
		allocBuffers(nb_buffers, frame_dim);
}

void NumaSoftBufferAllocMgr::setAllocator(Allocator::Ref allocator)
{
	DEB_MEMBER_FUNCT();
	AllocParameters params;
	getAllocParameters(params);
	params.allocator = allocator;
	setAllocParameters(params);
}

Allocator::Ref NumaSoftBufferAllocMgr::getAllocator()
{
	DEB_MEMBER_FUNCT();
	AllocParameters params;
	getAllocParameters(params);
	return params.allocator;
}

#endif


/*******************************************************************
 * BufferCbMgr
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

	if ((nb_buffers < 1) || (nb_concat_frames < 1))
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR2(nb_buffers, 
							 nb_concat_frames);

	buffer_nb = (acq_frame_nb / nb_concat_frames) % nb_buffers;
	concat_frame_nb = acq_frame_nb % nb_concat_frames;

	DEB_RETURN() << DEB_VAR2(buffer_nb, concat_frame_nb);
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
 * StdBufferCbMgr
 *******************************************************************/

StdBufferCbMgr::StdBufferCbMgr(BufferAllocMgr& alloc_mgr)
	: m_alloc_mgr(&alloc_mgr)

{
	DEB_CONSTRUCTOR();
	m_nb_concat_frames = 1;
	m_keep_sideband_data = false;
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
	if (frame_size <= 0)
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_dim);
	else if (nb_concat_frames < 1)
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(nb_concat_frames);

	int curr_nb_buffers;
	getNbBuffers(curr_nb_buffers);
	if ((frame_dim != m_frame_dim) || 
	    (nb_concat_frames != m_nb_concat_frames)) {
		releaseBuffers();
	} else if (nb_buffers == curr_nb_buffers) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	try {
		FrameDim buffer_frame_dim;
		getBufferFrameDim(frame_dim, nb_concat_frames, 
				  buffer_frame_dim);
		DEB_TRACE() << "(Re)allocating buffers";
		m_alloc_mgr->allocBuffers(nb_buffers, buffer_frame_dim);
		m_frame_dim = frame_dim;
		m_nb_concat_frames = nb_concat_frames;

		DEB_TRACE() << "(Re)allocating frame info list";
		int nb_frames = nb_buffers * nb_concat_frames;
		m_info_list.resize(nb_frames);

		DEB_TRACE() << "(Re)allocating frame nb map";
		for (int i = 0; i < nb_frames; ++i)
			m_frame_nb_map[getFrameBufferPtr(i)] = i;
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
	m_frame_nb_map.clear();
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
	if (!frame_info.frame_ptr)
		frame_info.frame_ptr = ptr;
	else if (frame_info.frame_ptr != ptr)
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_info.frame_ptr);

	const FrameDim& frame_dim = getFrameDim();
	if (!frame_info.frame_dim.isValid())
		frame_info.frame_dim = frame_dim;
	else if (frame_info.frame_dim != frame_dim)
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(frame_info.frame_dim);

	if (frame_info.valid_pixels == 0)
		frame_info.valid_pixels = Point(frame_dim.getSize()).getArea();

	int frame_nb = buffer_nb * m_nb_concat_frames + concat_frame_nb;
	m_info_list[frame_nb] = frame_info;
	if (!frame_info.sideband_data.empty() && !m_keep_sideband_data)
		m_info_list[frame_nb].sideband_data.reset();

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
	acqFrameNb2BufferNb(frame_nb, buffer_nb, concat_frame_nb);
	return getBufferPtr(buffer_nb, concat_frame_nb);
}

int StdBufferCbMgr::getFrameBufferNb(void *ptr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ptr);

	FrameNbMap::iterator it = m_frame_nb_map.find(ptr);
	if (it == m_frame_nb_map.end())
		THROW_HW_ERROR(Error) << "Buffer " << ptr << " not found";
	
	int frame_nb = it->second;
	DEB_RETURN() << DEB_VAR1(frame_nb);
	return frame_nb;
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

void StdBufferCbMgr::setKeepSidebandData(bool keep_sideband_data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(keep_sideband_data);
	m_keep_sideband_data = keep_sideband_data;
}

void StdBufferCbMgr::getKeepSidebandData(bool& keep_sideband_data)
{
	DEB_MEMBER_FUNCT();
	keep_sideband_data = m_keep_sideband_data;
	DEB_RETURN() << DEB_VAR1(keep_sideband_data);
}

/*******************************************************************
 * BufferCtrlMgr
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

	if ((frame_dim == m_frame_dim) &&
	    (m_frame_dim.getImageType() == frame_dim.getImageType())) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	releaseBuffers();
	m_frame_dim = frame_dim;
}

void BufferCtrlMgr::getFrameDim(FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	frame_dim = m_frame_dim;
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
	if ((nb_buffers > 0) && (nb_buffers > max_nb_buffers))
		THROW_HW_ERROR(InvalidValue) << "Too many buffers:" 
					     << DEB_VAR1(nb_buffers);
	else if (nb_buffers == 0)
		nb_buffers = max_nb_buffers;

	DEB_TRACE() << DEB_VAR1(nb_buffers);
	m_acq_buffer_mgr->allocBuffers(nb_buffers, m_nb_concat_frames, 
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
 * BufferCtrlMgr::AcqFrameCallback
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

SoftBufferCtrlObj::SoftBufferCtrlObj(BufferAllocMgrPtr buffer_alloc_mgr) 
	: HwBufferCtrlObj(), 
	  m_buffer_alloc_mgr(buffer_alloc_mgr ? buffer_alloc_mgr :
						new SoftBufferAllocMgr),
	  m_buffer_cb_mgr(*m_buffer_alloc_mgr), m_mgr(m_buffer_cb_mgr),
	  m_acq_frame_nb(-1)
{
}

void SoftBufferCtrlObj::setAllocParameters(const AllocParameters& params)
{
	m_buffer_alloc_mgr->setAllocParameters(params);
}

void SoftBufferCtrlObj::getAllocParameters(AllocParameters& params)
{
	m_buffer_alloc_mgr->getAllocParameters(params);
}

void SoftBufferCtrlObj::setFrameDim(const FrameDim& frame_dim) 
{
	m_mgr.setFrameDim(frame_dim);
}

void SoftBufferCtrlObj::getFrameDim(FrameDim& frame_dim) 
{
	m_mgr.getFrameDim(frame_dim);
}

void SoftBufferCtrlObj::prepareAlloc(int nb_buffers)
{
	int nb_concat_frames;
	getNbConcatFrames(nb_concat_frames);
	FrameDim frame_dim;
	getFrameDim(frame_dim);
	Size size = frame_dim.getSize();
	size *= Point(1, nb_concat_frames);
	frame_dim.setSize(size);
	m_buffer_alloc_mgr->prepareAlloc(nb_buffers, frame_dim);
}

void SoftBufferCtrlObj::setNbBuffers(int  nb_buffers) 
{
	m_mgr.setNbBuffers(nb_buffers);
	if (m_buffer_callback)
		m_buffer_callback->realloc();
}

void SoftBufferCtrlObj::getNbBuffers(int& nb_buffers)
{
	m_mgr.getNbBuffers(nb_buffers);
}

void SoftBufferCtrlObj::setNbConcatFrames(int nb_concat_frames) 
{
	m_mgr.setNbConcatFrames(nb_concat_frames);
	if (m_buffer_callback)
		m_buffer_callback->realloc();
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

SoftBufferCtrlObj::Sync *SoftBufferCtrlObj::getBufferSync(Cond& cond)
{
	if (!m_buffer_callback)
		m_buffer_callback = new Sync(*this, cond);
	return m_buffer_callback;
}

HwBufferCtrlObj::Callback *SoftBufferCtrlObj::getBufferCallback() 
{
	return m_buffer_callback;
}

SoftBufferCtrlObj::Sync::Sync(SoftBufferCtrlObj& buffer_ctrl_obj, Cond& cond) 
	: m_cond(cond), m_buffer_ctrl_obj(buffer_ctrl_obj),
	  m_total_used_frames(0)
{
	DEB_CONSTRUCTOR();
	realloc();
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

	frame_number %= m_frame_use.size();
	if (m_frame_use[frame_number] == 0)
		return AVAILABLE;

	bool okFlag = m_cond.wait(timeout);
	DEB_TRACE() << DEB_VAR1(okFlag);

	if (m_frame_use[frame_number] == 0)
		return AVAILABLE;
 	else
		return okFlag ? INTERRUPTED : TIMEOUT;
}

void *SoftBufferCtrlObj::Sync::map(void *address)
{
	DEB_MEMBER_FUNCT();

	StdBufferCbMgr& buffer_mgr = m_buffer_ctrl_obj.getBuffer();
	int frame_nb = buffer_mgr.getFrameBufferNb(address);
	AutoMutex aLock(m_cond.mutex());
	int *frame_use_ptr = &m_frame_use[frame_nb];
	if ((*frame_use_ptr)++ == 0)
		++m_total_used_frames;
	return frame_use_ptr;
}

void SoftBufferCtrlObj::Sync::release(void *address_ref)
{
	DEB_MEMBER_FUNCT();

	int *frame_use_ptr = static_cast<int *>(address_ref);
	AutoMutex aLock(m_cond.mutex());
	if (*frame_use_ptr == 0)
		THROW_HW_ERROR(Error) << "Internal error: " 
				      << "releasing buffer not in use list";

	if (--(*frame_use_ptr) == 0) {
		--m_total_used_frames;
		m_cond.broadcast();
	}
}

void SoftBufferCtrlObj::Sync::releaseAll()
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	UseCountList::iterator it, end = m_frame_use.end();
	for (it = m_frame_use.begin(); it != end; ++it)
		*it = 0;
	if (m_total_used_frames > 0)
		DEB_WARNING() << "Buffers in use when called releaseAll";
	m_total_used_frames = 0;
	m_cond.broadcast();
}

void SoftBufferCtrlObj::Sync::realloc()
{
	DEB_MEMBER_FUNCT();

	{
		AutoMutex aLock(m_cond.mutex());
		if (m_total_used_frames > 0)
			THROW_HW_ERROR(Error) << "Buffers in use when called "
					      << "realloc";
	}

	int nb_buffers;
	m_buffer_ctrl_obj.getNbBuffers(nb_buffers);
	int nb_concat_frames;
	m_buffer_ctrl_obj.getNbConcatFrames(nb_concat_frames);

	int nb_frames = nb_buffers * nb_concat_frames;
	if (nb_frames == m_frame_use.size())
		return;

	m_frame_use.resize(nb_frames);

	UseCountList::iterator it, end = m_frame_use.end();
	for (it = m_frame_use.begin(); it != end; ++it)
		*it = 0;
}

#if !defined(_WIN32)
/*****************************************************************************
			MmapFileBufferAllocMgr
****************************************************************************/
MmapFileBufferAllocMgr::MmapFileBufferAllocMgr(const char* mapped_file):
  m_map_mem_base(NULL),
  m_map_size(0),
  m_use_size(0),
  m_frame_mem_size(0),
  m_nb_buffers(0)
{
  DEB_CONSTRUCTOR();

  int fd = ::open(mapped_file,O_RDWR);
  if(fd < 0)
    THROW_HW_ERROR(Error) << "Could not open file: "  << mapped_file;

  m_map_size = ::lseek(fd,0,SEEK_END);
  ::lseek(fd,0,SEEK_SET);
  m_map_mem_base = mmap(NULL,m_map_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  close(fd);

  if(m_map_mem_base == MAP_FAILED)
    THROW_HW_ERROR(Error) << "Could not map file: "  << mapped_file;

  m_use_size = m_map_size;
  off_t tot_mem = _calc_req_mem_size(100);
  m_alloc_params.reqMemSizePercent = 100.0 * m_use_size / tot_mem;

  DEB_TRACE() << DEB_VAR2(m_map_size, m_alloc_params);
}

MmapFileBufferAllocMgr::~MmapFileBufferAllocMgr()
{
  if(m_map_mem_base)
    munmap(m_map_mem_base,m_map_size);
}

void MmapFileBufferAllocMgr::setAllocParameters(const AllocParameters& alloc_params)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(alloc_params);
  m_alloc_params = alloc_params;
}

void MmapFileBufferAllocMgr::getAllocParameters(AllocParameters& alloc_params)
{
  DEB_MEMBER_FUNCT();
  alloc_params = m_alloc_params;
  DEB_RETURN() << DEB_VAR1(alloc_params);
}

int MmapFileBufferAllocMgr::getMaxNbBuffers(const FrameDim& frame_dim)
{
  int frame_mem_size = _calc_frame_mem_size(frame_dim);
  return m_use_size / frame_mem_size;
}

void MmapFileBufferAllocMgr::allocBuffers(int nb_buffers,const FrameDim& frame_dim)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(nb_buffers, frame_dim);

  off_t req_mem = _calc_req_mem_size(m_alloc_params.reqMemSizePercent);
  if (req_mem > m_map_size)
    THROW_HW_ERROR(InvalidValue) << "MmapFile not big enough: "
				 << DEB_VAR2(req_mem, m_map_size);
  m_use_size = req_mem;
  DEB_TRACE() << DEB_VAR2(m_map_size, m_use_size);

  m_frame_mem_size = _calc_frame_mem_size(frame_dim);
  m_frame_dim = frame_dim;
  m_nb_buffers = nb_buffers;
}

void MmapFileBufferAllocMgr::releaseBuffers()
{
  m_frame_mem_size = 0;
  m_nb_buffers = 0;
  m_frame_dim = FrameDim();
}

void* MmapFileBufferAllocMgr::getBufferPtr(int buffer_nb)
{
  DEB_MEMBER_FUNCT();

  if(m_frame_mem_size <= 0)
    THROW_HW_ERROR(Error) << "Allocation wasn't done";

  unsigned long long offset = buffer_nb;
  offset *= m_frame_mem_size;  
  char* start_map = (char*)m_map_mem_base;
  return start_map + offset;
}

void MmapFileBufferAllocMgr::clearBuffer(int buffer_nb)
{
  if(m_frame_mem_size > 0)
    {
      void *ptr = getBufferPtr(buffer_nb);
      memset(ptr, 0, m_frame_mem_size);
    }
}

void MmapFileBufferAllocMgr::clearAllBuffers()
{
  if(m_use_size > 0)
    memset(m_map_mem_base,0,m_use_size);
}

int MmapFileBufferAllocMgr::_calc_frame_mem_size(const FrameDim& frame_dim) const
{
  return  (frame_dim.getMemSize() + 31) & ~31; // 32 alignment (avx2)
}

off_t MmapFileBufferAllocMgr::_calc_req_mem_size(double req_mem_size_percent)
{
  int mem_unit = 0;
  int system_mem;
  GetSystemMem(mem_unit, system_mem);
  return req_mem_size_percent * mem_unit * system_mem / 100.0;
}
#endif // !defined(_WIN32)
