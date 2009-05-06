#ifndef HWFRAMECALLBACK_H
#define HWFRAMECALLBACK_H

namespace lima
{

class HwBufferCtrlObj;

class HwFrameCallback
{
 public:
	typedef struct Info {
		int acq_frame_nb;
		void *frame_ptr;
		FrameDim frame_dim;
		double frame_time_stamp;
	} InfoType;

};

} // namespace lima

#endif // HWFRAMECALLBACK_H
