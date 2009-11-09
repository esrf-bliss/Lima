#ifndef ESPIABUFFERMGR_H
#define ESPIABUFFERMGR_H

#include "HwBufferMgr.h"
#include "EspiaAcq.h"

namespace lima
{

namespace Espia
{

class BufferMgr : public BufferCbMgr
{
	DEB_CLASS_NAMESPC(DebModEspia, "BufferMgr", "Espia");

 public:
	BufferMgr(Acq& acq);
	virtual ~BufferMgr();

	virtual Cap getCap();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim,
				    int nb_concat_frames);
	virtual void allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers);
	virtual void getNbConcatFrames(int& nb_concat_frames);
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb);

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info);

	virtual void setStartTimestamp(Timestamp  start_ts);
	virtual void getStartTimestamp(Timestamp& start_ts);

 protected:
	class FrameCallback : public HwFrameCallback
	{
	public:
		FrameCallback(BufferMgr& buffer_mgr) 
			: m_buffer_mgr(buffer_mgr) {}

		virtual bool newFrameReady(const HwFrameInfoType& frame_info)
		{
			return m_buffer_mgr.newFrameReady(frame_info);
		}
			
	protected:
		BufferMgr& m_buffer_mgr;
	};

	virtual void setFrameCallbackActive(bool cb_active);
	
 private:
	Acq& m_acq;
	FrameCallback m_frame_cb;
};

} // namespace Espia

} // namespace lima

#endif // ESPIABUFFERMGR_H
