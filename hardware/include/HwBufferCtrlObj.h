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

	virtual void setNbBuffers(int  nb_buffers) = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;

	virtual void setNbConcatFrames(int  nb_concat_frames) = 0;
	virtual void getNbConcatFrames(int& nb_concat_frames) = 0;

	virtual void setNbAccFrames(int  nb_acc_frames) = 0;
	virtual void getNbAccFrames(int& nb_acc_frames) = 0;

	virtual void getMaxNbBuffers(int& max_nb_buffers) = 0;

	virtual void setBufferMode(BufferMode  buffer_mode) = 0;
	virtual void getBufferMode(BufferMode& buffer_mode) = 0;


	virtual void *getBufferPtr(int buffer_nb) = 0;
	virtual void *getFramePtr(int acq_frame_nb) = 0;
	virtual double getFrameTimeStamp(int acq_frame_nb) = 0;

 private:
};
 
} // namespace lima

#endif // HWBUFFERCTRLOBJ_H
