#include <sstream>
#include "ProsilicaSyncCtrlObj.h"
#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaCamera.h"

using namespace lima;
using namespace lima::Prosilica;

SyncCtrlObj::SyncCtrlObj(Camera *cam,BufferCtrlObj *buffer) :
  m_cam(cam),
  m_handle(cam->getHandle()),
  m_trig_mode(IntTrig),
  m_buffer(buffer),
  m_nb_frames(1),
  m_started(false)
{
  DEB_CONSTRUCTOR();
}

SyncCtrlObj::~SyncCtrlObj()
{
  DEB_DESTRUCTOR();
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  switch(trig_mode)
    {
    case IntTrig:
    case IntTrigMult:
    case ExtTrigMult:
      return true;
    default:
      return false;
    }
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  tPvErr error;
  if(checkTrigMode(trig_mode))
    {
      switch(trig_mode)
	{
	case ExtTrigMult:
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerMode", "SyncIn1");
	  if(error)
	    throw LIMA_HW_EXC(Error,"Can't set trigger input");
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerEvent", "EdgeRising");
	  if(error)
	    throw LIMA_HW_EXC(Error,"Can't change Trigger start to a rising edge");
	  break;
	default:		// Software
	  error = PvAttrEnumSet(m_handle, "FrameStartTriggerMode", "FixedRate");
	  if(error)
	    {
	      std::stringstream message;
	      message << "could not set trigger mode to FixedRate " << error;
	      throw LIMA_HW_EXC(Error,message.str().c_str());
	    }
	  break;
	}
      m_trig_mode = trig_mode;
    }
  else
    throw LIMA_HW_EXC(NotSupported,"Trigger type not supported");
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
  trig_mode = m_trig_mode;
}

void SyncCtrlObj::setExpTime(double exp_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(exp_time);

  tPvErr error = PvAttrEnumSet(m_handle, "ExposureMode", "Manual");
  if(error != ePvErrSuccess)
    throw LIMA_HW_EXC(Error,"Can't set manual exposure");

  tPvUint32 exposure_value = tPvUint32(exp_time * 1e6);
  error = PvAttrUint32Set(m_handle,"ExposureValue",exposure_value);
  if(error != ePvErrSuccess)
    throw LIMA_HW_EXC(Error,"Can't set exposure time failed"); 
}

void SyncCtrlObj::getExpTime(double &exp_time)
{
  DEB_MEMBER_FUNCT();

  tPvUint32 exposure_value;
  PvAttrUint32Get(m_handle, "ExposureValue", &exposure_value);
  exp_time = exposure_value / 1e6;

  DEB_RETURN() << DEB_VAR1(exp_time);
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
  //No latency managed
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
  lat_time = 0.;		// Don't know
}

void SyncCtrlObj::setNbFrames(int  nb_frames)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nb_frames);

  m_nb_frames = nb_frames;
}

void SyncCtrlObj::getNbFrames(int& nb_frames)
{
  nb_frames = m_nb_frames;
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
  setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
  getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
  valid_ranges.min_exp_time = 1e-6; // Don't know
  valid_ranges.max_exp_time = 60.; // Don't know
  valid_ranges.min_lat_time = 0.; // Don't know
  valid_ranges.max_lat_time = 0.; // Don't know
}

void SyncCtrlObj::startAcq()
{
  DEB_MEMBER_FUNCT();
  if(!m_started)
    {
      tPvErr error = PvCaptureStart(m_handle);
      if(error)
	throw LIMA_HW_EXC(Error,"Can't start acquisition capture");

      error = PvCommandRun(m_handle, "AcquisitionStart");
      if(error)
	throw LIMA_HW_EXC(Error,"Can't start acquisition");
  
      if(m_buffer)
	m_buffer->startAcq();
      else
	m_cam->startAcq();
    }
  m_started = true;
}

void SyncCtrlObj::stopAcq()
{
  DEB_MEMBER_FUNCT();
  if(m_started)
    {
      DEB_TRACE() << "Try to stop Acq";
      tPvErr error = PvCommandRun(m_handle,"AcquisitionStop");
      if(error)
	{
	  DEB_ERROR() << "Failed to stop acquisition";
	  throw LIMA_HW_EXC(Error,"Failed to stop acquisition");
	}

      DEB_TRACE() << "Try to stop Capture";
      error = PvCaptureEnd(m_handle);
      if(error)
	{
	  DEB_ERROR() << "Failed to stop acquisition";
	  throw LIMA_HW_EXC(Error,"Failed to stop acquisition");
	}

//       DEB_TRACE() << "Try to clear queue";
//       error = PvCaptureQueueClear(m_handle);
//       if(error)
// 	{
// 	  DEB_ERROR() << "Failed to stop acquisition";
// 	  throw LIMA_HW_EXC(Error,"Failed to stop acquisition");
// 	}
    }
  m_started = false;
}

void SyncCtrlObj::getStatus(HwInterface::StatusType& status)
{
  DEB_MEMBER_FUNCT();
  if(m_started)
    {
      tPvErr error = ePvErrSuccess;
      if(m_buffer)
	{
	  bool exposing;
	  m_buffer->getStatus(error,exposing);
	  if(error)
	    {
	      status.acq = AcqFault;
	      status.det = DetFault;
	    }
	  else
	    {
	      status.acq = AcqRunning;
	      status.det = exposing ? DetExposure : DetIdle;
	    }
	}
      else			// video mode, don't need to be precise
	{
	  status.acq = AcqRunning;
	  status.det = DetExposure;
	}
    }
  else
    {
      status.acq = AcqReady;
      status.det = DetIdle;
    }
  DEB_RETURN() << DEB_VAR1(status);
}
