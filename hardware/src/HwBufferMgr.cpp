#include "HwBufferMgr.h"

using namespace lima;

SoftBufferMgr::SoftBufferMgr()
{
}

SoftBufferMgr::~SoftBufferMgr()
{
	releaseBuffers();
}

int SoftBufferMgr::getMaxNbBuffers(const FrameDim& frame_dim)
{
	return 0;
}

void SoftBufferMgr::allocBuffers(int nb_buffers, 
				 const FrameDim& frame_dim)
{
	int frame_size = frame_dim.getMemSize();
	if (frame_size <= 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid FrameDim");

	BufferList& bl = m_buffer_list;
	bl.reserve(nb_buffers);
	try {
		for (int i = 0; i < nb_buffers; ++i)
			bl.push_back(new char[frame_size]);
	} catch (...) {
		releaseBuffers();
		throw;
	}

	m_frame_dim = frame_dim;
}

void SoftBufferMgr::releaseBuffers()
{
	BufferList& bl = m_buffer_list;
	for (BufferListCIt it = bl.begin(); it != bl.end(); ++it)
		delete [] *it;
	bl.clear();
	m_frame_dim = FrameDim();
}

