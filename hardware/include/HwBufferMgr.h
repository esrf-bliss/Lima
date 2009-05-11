#ifndef HWBUFFERMGR_H
#define HWBUFFERMGR_H

#include "HwFrameCallback.h"
#include "SizeUtils.h"

#include <vector>

namespace lima
{

class BaseBufferMgr : HwFrameCallbackGen
{
 public:
	virtual int getMaxNbBuffers(const FrameDim& frame_dim) = 0;
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual int getNbBuffers() = 0;
	virtual void releaseBuffers() = 0;
	
	virtual void *getBufferPtr(int buffer_nb) = 0;
	virtual double getBufferTimestamp(int buffer_nb) = 0;
};


class SoftBufferMgr : public BaseBufferMgr
{
 public:
	SoftBufferMgr();
	virtual ~SoftBufferMgr();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim);
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim() const;
	virtual int getNbBuffers() const;
	virtual void releaseBuffers();
	
	virtual void *getBufferPtr(int buffer_nb) const;
	virtual double getBufferTimestamp(int buffer_nb);

 private:
	typedef std::vector<char *> BufferList;
	typedef BufferList::const_iterator BufferListCIt;

	FrameDim m_frame_dim;
	BufferList m_buffer_list;
};

inline const FrameDim& SoftBufferMgr::getFrameDim() const
{
	return m_frame_dim;
}

inline int SoftBufferMgr::getNbBuffers() const
{
	return m_buffer_list.size();
}

inline void *SoftBufferMgr::getBufferPtr(int buffer_nb) const
{
	return m_buffer_list[buffer_nb];
}

inline double SoftBufferMgr::getBufferTimestamp(int buffer_nb)
{
	return 0;
}


class ConcatBufferMgr : public BaseBufferMgr
{



};


class AccBufferMgr : public BaseBufferMgr
{



};


} // namespace lima

#endif // HWBUFFERMGR_H
