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
	DEB_PARAM_VAR1(dev.getDevNb());

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
	DEB_PARAM_VAR2(frame_dim, nb_concat_frames);

	FrameDim buffer_frame_dim;
	getBufferFrameDim(frame_dim, nb_concat_frames, buffer_frame_dim);
	DEB_TRACE_VAR1(buffer_frame_dim);

	int max_nb_buffers = GetDefMaxNbBuffers(buffer_frame_dim);
	DEB_RETURN_VAR1(max_nb_buffers);
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
	DEB_PARAM_VAR1(cb_active);

	if (cb_active)
		m_acq.registerFrameCallback(m_frame_cb);
	else
		m_acq.unregisterFrameCallback(m_frame_cb);
}
	
