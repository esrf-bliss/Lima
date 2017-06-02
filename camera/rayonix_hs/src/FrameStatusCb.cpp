//Copyright (C) Rayonix, LLC
#include "RayonixHsCamera.h"

#include "FrameStatusCb.h"

using namespace lima;
using namespace lima::RayonixHs;

FrameStatusCb::FrameStatusCb(Camera *cam, volatile bool &acquiring)
  : m_cam(cam),
    mRawFramesRcvd(0),
    mBgFramesRcvd(0),
    mCorrFramesRcvd(0),
    m_acquiring(acquiring) {
    //acquisitionComplete(NULL) {

}

//void registerCallbackAcqComplete(void(*func)()) {
//   acquistionComplete = func;
//}

FrameStatusCb::~FrameStatusCb() {}

void FrameStatusCb::RawFrameReady(int frame_number, craydl::RxFrame *rx_frame) { ++mRawFramesRcvd; std::cout << "Have received " << mRawFramesRcvd << " raw frames." << std::endl; }

void FrameStatusCb::BackgroundFrameReady(craydl::RxFrame *frame_p) { ++mBgFramesRcvd; std::cout << "Have received " << mBgFramesRcvd << " background frames." << std::endl; }

void FrameStatusCb::FrameReady(int frame_number, craydl::RxFrame *rx_frame) { ++mCorrFramesRcvd; std::cout << "Have received " << mCorrFramesRcvd << " corrected frames." << std::endl; }

void FrameStatusCb::FrameAborted(int frame_number) { std::cout << "FrameStatusCb: Frame #" << frame_number << " aborted!" << std::endl; }

void FrameStatusCb::FrameError(int frame_number, int error_code, const std::string& error_string) { std::cout << "FrameStatusCb: Frame error with frame #" << frame_number << "!" << std::endl; }

void FrameStatusCb::SequenceStarted() { m_acquiring = true; std::cout << "FrameStatusCb: Sequence started." << std::endl; }

void FrameStatusCb::SequenceEnded() { m_acquiring = false; std::cout << "FrameStatusCb: Sequence ended." << std::endl; }

void FrameStatusCb::ExposureStarted(int frame) { std::cout << "FrameStatusCb: Exposure started." << std::endl; }
void FrameStatusCb::ExposureEnded(int frame) { std::cout << "FrameStatusCb: Exposure ended." << std::endl; }

void FrameStatusCb::ReadoutStarted(int frame) {}
void FrameStatusCb::ReadoutEnded(int frame) {}

void FrameStatusCb::FrameCompleted(int frame) {}

int FrameStatusCb::frameCountRaw() const { return mRawFramesRcvd; }
int FrameStatusCb::frameCountBackground() const { return mBgFramesRcvd; }
int FrameStatusCb::frameCountCorrected() const { return mCorrFramesRcvd; }

void FrameStatusCb::resetFrameCounts() { mRawFramesRcvd = mBgFramesRcvd = mCorrFramesRcvd = 0; }
