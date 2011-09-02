#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

BufferCtrlObj::BufferCtrlObj(Camera *cam) :
  m_handle(cam->getHandle()),
  m_status(ePvErrSuccess),
  m_exposing(false)
{
  DEB_CONSTRUCTOR();

  m_frame[0].Context[0] = this;
  m_frame[1].Context[0] = this;
}
void BufferCtrlObj::prepareAcq()
{
  DEB_MEMBER_FUNCT();
  FrameDim dim;
  getFrameDim(dim);
  m_frame[0].ImageBufferSize = m_frame[1].ImageBufferSize = dim.getMemSize();
  
  m_acq_frame_nb = -1;
  int buffer_nb,concat_frame_nb;
  m_buffer_cb_mgr.acqFrameNb2BufferNb(0,buffer_nb,concat_frame_nb);
  tPvFrame& frame0 = m_frame[0];
  frame0.ImageBuffer = (char*) m_buffer_cb_mgr.getBufferPtr(buffer_nb,
							    concat_frame_nb);

  m_buffer_cb_mgr.acqFrameNb2BufferNb(1,buffer_nb,concat_frame_nb);
  tPvFrame& frame1 = m_frame[1];
  frame1.ImageBuffer = (char*) m_buffer_cb_mgr.getBufferPtr(buffer_nb,
							    concat_frame_nb);
}

void BufferCtrlObj::startAcq()
{
  DEB_MEMBER_FUNCT();

  m_exposing = true;
  tPvFrame& frame = m_frame[0];
  m_status = PvCaptureQueueFrame(m_handle,&frame,_newFrame);
  
/**  int requested_nb_frames;
  m_sync->getNbFrames(requested_nb_frames);
  if(!requested_nb_frames || requested_nb_frames > 1)
    {
      tPvFrame& frame = m_frame[1];
      m_status = PvCaptureQueueFrame(m_handle,&frame,_newFrame);
    }
*/
}

void BufferCtrlObj::_newFrame(tPvFrame* aFrame)
{
  DEB_STATIC_FUNCT();
  BufferCtrlObj *bufferPt = (BufferCtrlObj*)aFrame->Context[0];

  int requested_nb_frames;
  bufferPt->m_sync->getNbFrames(requested_nb_frames);

  bufferPt->m_exposing = false;
  if(bufferPt->m_status || aFrame->Status != ePvErrSuccess) // error
    {
      // it's not really an error,continue
      if(aFrame->Status == ePvErrDataMissing)
	{
	  DEB_WARNING() << DEB_VAR1(aFrame->Status);
          PvCaptureQueueFrame(bufferPt->m_handle,aFrame,_newFrame);
	  return;
	}
      else if(aFrame->Status == ePvErrCancelled) // we stopped the acqusition so not an error
	return;
      else 
	{
	  if(!bufferPt->m_status) // Keep error status
	    bufferPt->m_status = aFrame->Status;

	  if(aFrame->Status)
	    DEB_ERROR() << DEB_VAR1(aFrame->Status);
	    
	  return;
	}
    }
  
  ++bufferPt->m_acq_frame_nb;
  
  bool stopAcq = false;
  if(!requested_nb_frames || 
     bufferPt->m_acq_frame_nb < (requested_nb_frames - 1))
    {
      int buffer_nb, concat_frame_nb;
      bufferPt->m_buffer_cb_mgr.acqFrameNb2BufferNb(bufferPt->m_acq_frame_nb,
						    buffer_nb,
						    concat_frame_nb);
      aFrame->ImageBuffer = (char*)bufferPt->m_buffer_cb_mgr.getBufferPtr(buffer_nb,
									  concat_frame_nb);
      bufferPt->m_exposing = true;
      bufferPt->m_status = PvCaptureQueueFrame(bufferPt->m_handle,aFrame,_newFrame);
    }
  else
    stopAcq = true;
  
  HwFrameInfoType frame_info;
  frame_info.acq_frame_nb = bufferPt->m_acq_frame_nb;
  bufferPt->m_buffer_cb_mgr.newFrameReady(frame_info);
  
  if(stopAcq)
    bufferPt->m_sync->stopAcq(false);
}
