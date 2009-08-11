#include "EspiaBufferMgr.h"
#include "MemUtils.h"

using namespace lima;
using namespace lima::Espia;
using namespace std;


BufferMgr::BufferMgr(Acq& acq) 
	: m_acq(acq), m_frame_cb(*this)
{
}

BufferMgr::~BufferMgr() 
{
}

BufferMgr::Cap BufferMgr::getCap()
{
	return Basic | Concat;
}

int BufferMgr::getMaxNbBuffers(const FrameDim& frame_dim,
			       int nb_concat_frames)
{
	FrameDim buffer_frame_dim;
	getBufferFrameDim(frame_dim, nb_concat_frames, buffer_frame_dim);
	return GetDefMaxNbBuffers(buffer_frame_dim);
}

void BufferMgr::allocBuffers(int nb_buffers, int nb_concat_frames, 
			     const FrameDim& frame_dim)
{
	m_acq.bufferAlloc(nb_buffers, nb_concat_frames, frame_dim);
}

const FrameDim& BufferMgr::getFrameDim()
{
	return m_acq.getFrameDim();
}

void BufferMgr::getNbBuffers(int& nb_buffers)
{
	m_acq.getNbBuffers(nb_buffers);
}

void BufferMgr::getNbConcatFrames(int& nb_concat_frames)
{
	m_acq.getNbBufferFrames(nb_concat_frames);
}

void BufferMgr::releaseBuffers()
{
	m_acq.bufferFree();
}

void *BufferMgr::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	return m_acq.getBufferFramePtr(buffer_nb, concat_frame_nb);
}

void BufferMgr::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	m_acq.getFrameInfo(acq_frame_nb, info);
}

void BufferMgr::setStartTimestamp(Timestamp  start_ts)
{
}

void BufferMgr::getStartTimestamp(Timestamp& start_ts)
{
	m_acq.getStartTimestamp(start_ts);
}

void BufferMgr::setFrameCallbackActive(bool cb_active)
{
	if (cb_active)
		m_acq.registerFrameCallback(m_frame_cb);
	else
		m_acq.unregisterFrameCallback(m_frame_cb);
}
	
