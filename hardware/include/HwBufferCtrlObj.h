#ifndef HWBUFFERCTRLOBJ_H
#define HWBUFFERCTRLOBJ_H

#include "HwFrameCallback.h"

namespace lima
{

class HwBufferCtrlObj
{
public:
	HwBufferCtrlObj();
	virtual ~HwBufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim) = 0;
	virtual void getFramedim(      FrameDim& frame_dim) = 0;

	virtual void setNbBuffers(int  nb_buffers) = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;

	virtual void setNbConcatFrames(int  nb_concat_frames) = 0;
	virtual void getNbConcatFrames(int& nb_concat_frames) = 0;

	virtual void setNbAccFrames(int  nb_acc_frames) = 0;
	virtual void getNbAccFrames(int& nb_acc_frames) = 0;

	virtual void getMaxNbBuffers(int& max_nb_buffers) = 0;

	virtual void *getBufferPtr(int buffer_nb) = 0;
	virtual void *getFramePtr(int acq_frame_nb) = 0;

	virtual void getStartTimestamp(Timestamp& start_ts) = 0;
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info) = 0;

	virtual void   registerFrameCallback(HwFrameCallback *frame_cb) = 0;
	virtual void unregisterFrameCallback(HwFrameCallback *frame_cb) = 0;
};
 
} // namespace lima

#endif // HWBUFFERCTRLOBJ_H
