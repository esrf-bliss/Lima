class HwBufferCtrlObj
{
%TypeHeaderCode
#include <HwBufferCtrlObj.h>
%End
public:
	HwBufferCtrlObj();
	virtual ~HwBufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim) = 0;
	virtual void getFrameDim(FrameDim& frame_dim /Out/) = 0;

	virtual void setNbBuffers(int  nb_buffers) = 0;
	virtual void getNbBuffers(int& nb_buffers /Out/) = 0;

	virtual void setNbConcatFrames(int  nb_concat_frames) = 0;
	virtual void getNbConcatFrames(int& nb_concat_frames /Out/) = 0;

	virtual void getMaxNbBuffers(int& max_nb_buffers /Out/) = 0;

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0) = 0;
	virtual void *getFramePtr(int acq_frame_nb) = 0;

	virtual void getStartTimestamp(Timestamp& start_ts /Out/) = 0;
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/) = 0;

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb) = 0;
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb) = 0;
};
 
