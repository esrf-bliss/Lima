#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

BufferCtrlObj::BufferCtrlObj(Camera *cam) :
  m_handle(cam->getHandle())
{
  m_acq_frame_nb = 0;
  m_frame[0].Context[0] = this;
  m_frame[1].Context[0] = this;
}

void BufferCtrlObj::startAcq()
{
  m_acq_frame_nb = 0;
  int buffer_nb,concat_frame_nb;
  m_buffer_cb_mgr.acqFrameNb2BufferNb(0,buffer_nb,concat_frame_nb);
  tPvFrame& frame = m_frame[0];
  frame.ImageBuffer = (char*) m_buffer_cb_mgr.getBufferPtr(buffer_nb,
							   concat_frame_nb);
  tPvErr error = PvCaptureQueueFrame(m_handle,&frame,_newFrame);
}

void BufferCtrlObj::_newFrame(tPvFrame* aFrame)
{
  BufferCtrlObj *bufferPt = (BufferCtrlObj*)aFrame->Context[0];
  int requested_nb_frames;
  bufferPt->m_sync->getNbFrames(requested_nb_frames);
  
  ++bufferPt->m_acq_frame_nb;
  
  bool stopAcq = false;
  if(!requested_nb_frames || 
     bufferPt->m_acq_frame_nb < requested_nb_frames)
    {
      int buffer_nb, concat_frame_nb;
      bufferPt->m_buffer_cb_mgr.acqFrameNb2BufferNb(bufferPt->m_acq_frame_nb,
						    buffer_nb,
						    concat_frame_nb);
      tPvFrame& frame = bufferPt->m_frame[bufferPt->m_acq_frame_nb & 0x1];
      frame.ImageBuffer = (char*)bufferPt->m_buffer_cb_mgr.getBufferPtr(buffer_nb,
									concat_frame_nb);
      tPvErr error = PvCaptureQueueFrame(bufferPt->m_handle,&frame,_newFrame);
    }
  else
    stopAcq = true;
  
  HwFrameInfoType frame_info;
  frame_info.acq_frame_nb = bufferPt->m_acq_frame_nb;
  bufferPt->m_buffer_cb_mgr.newFrameReady(frame_info);
  
  if(stopAcq)
   bufferPt->m_sync->stopAcq();
}
