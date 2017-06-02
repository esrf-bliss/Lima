//Copyright (C) Rayonix, LLC
#ifndef FRAMESTATUSCB_H
#define FRAMESTATUSCB_H

#include "craydl.h"

namespace lima {
namespace RayonixHs {

class Camera;

class FrameStatusCb : public virtual craydl::VirtualFrameCallback {
    public:
        FrameStatusCb(Camera *cam, volatile bool &acquiring);

        ~FrameStatusCb();

        void RawFrameReady(int frame_number, craydl::RxFrame *rx_frame);

        void BackgroundFrameReady(craydl::RxFrame *frame_p);

        void FrameReady(int frame_number, craydl::RxFrame *rx_frame);

        void FrameAborted(int frame_number);

        void FrameError(int frame_number, int error_code, const std::string& error_string);

        void SequenceStarted();

        void SequenceEnded();
	
	void ExposureStarted(int);
	void ExposureEnded(int);
	void ReadoutStarted(int);
	void ReadoutEnded(int);
	void FrameCompleted(int);

	void resetFrameCounts();

	int frameCountRaw() const;
	int frameCountBackground() const;
	int frameCountCorrected() const;
	
    private:
    	Camera *m_cam;
    	volatile int mRawFramesRcvd;
	volatile int mBgFramesRcvd;
	volatile int mCorrFramesRcvd;
	
	volatile bool &m_acquiring;
};

} //namespace RayonxHs
} //namespace lima

#endif //FRAMESTATUSCB_H
