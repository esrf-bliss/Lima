//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include <string>
#include <sstream>

#include "CtControl.h"
#include "CtSaving.h"
#ifdef WITH_SPS_IMAGE
#include "CtSpsImage.h"
#endif
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtBuffer.h"
#include "CtShutter.h"
#include "CtAccumulation.h"
#include "CtVideo.h"

#include "SoftOpInternalMgr.h"
#include "SoftOpExternalMgr.h"

#include "PoolThreadMgr.h"

using namespace lima;

class CtControl::_LastBaseImageReadyCallback : public TaskEventCallback
{
public:
  _LastBaseImageReadyCallback(CtControl &ctrl) : 
    TaskEventCallback(),_ctrl(ctrl) {}
						 
  virtual void finished(Data &aData)
  {
    _ctrl.newBaseImageReady(aData);
  }
private:
  CtControl &_ctrl;
};

class CtControl::_LastImageReadyCallback : public TaskEventCallback
{
public:
  _LastImageReadyCallback(CtControl &ctrl) :
    TaskEventCallback(),_ctrl(ctrl) {}
  
  virtual void finished(Data &aData)
  {
    _ctrl.newImageReady(aData);
  }
private:
  CtControl &_ctrl;
};

class CtControl::_LastImageSavedCallback : public TaskEventCallback
{
public:
  _LastImageSavedCallback(CtControl &ctrl) :
    TaskEventCallback(),_ctrl(ctrl) {}
  
  virtual void finished(Data &aData)
  {
    _ctrl.newImageSaved(aData);
  }
private:
  CtControl &_ctrl;
};

class CtControl::_LastCounterReadyCallback : public TaskEventCallback
{
public:
  _LastCounterReadyCallback(CtControl &ctrl) :
    TaskEventCallback(),_ctrl(ctrl) {}
  
  virtual void finished(Data &aData)
  {
    _ctrl.newCounterReady(aData);
  }
private:
  CtControl &_ctrl;
};

class CtControl::_AbortAcqCallback : public TaskEventCallback
{
public:
  _AbortAcqCallback(CtControl &ctrl) :
    TaskEventCallback(),_ctrl(ctrl) {}
  
  virtual void finished(Data &aData)
  {
    _ctrl.stopAcq();
  }
private:
  CtControl &_ctrl;
};



CtControl::CtControl(HwInterface *hw) :
  m_hw(hw),
  m_op_int_active(false),
  m_op_ext_link_task_active(false),
  m_op_ext_sink_task_active(false),
  m_base_images_ready(CtControl::ltData()),
  m_images_ready(CtControl::ltData()),
  m_images_buffer_size(16),
  m_policy(All), m_ready(false),
  m_autosave(false), m_running(false),
  m_img_status_cb(NULL)
{
  DEB_CONSTRUCTOR();

  m_ct_acq= new CtAcquisition(hw);
  m_ct_image= new CtImage(hw,*this);
  m_ct_buffer= new CtBuffer(hw);
  m_ct_shutter = new CtShutter(hw);
  m_ct_accumulation = new CtAccumulation(*this);
  m_ct_video = new CtVideo(*this);

  //Saving
  m_ct_saving= new CtSaving(*this);
  _LastImageSavedCallback *aSaveCbkPt = new _LastImageSavedCallback(*this);
  m_ct_saving->setEndCallback(aSaveCbkPt);
  aSaveCbkPt->unref();

#ifdef WITH_SPS_IMAGE
  //Sps image
  m_ct_sps_image = new CtSpsImage();
#endif
  m_op_int = new SoftOpInternalMgr();
  m_op_ext = new SoftOpExternalMgr();
}

CtControl::~CtControl()
{
  DEB_DESTRUCTOR();

  DEB_TRACE() << "Waiting for all threads to finish their tasks";
  PoolThreadMgr& pool_thread_mgr = PoolThreadMgr::get();
  pool_thread_mgr.wait();

  if (m_img_status_cb)
    unregisterImageStatusCallback(*m_img_status_cb);

  delete m_ct_saving;
#ifdef WITH_SPS_IMAGE
  delete m_ct_sps_image;
#endif
  delete m_ct_acq;
  delete m_ct_image;
  delete m_ct_buffer;
  delete m_ct_shutter;
  delete m_ct_accumulation;
  delete m_ct_video;

  delete m_op_int;
  delete m_op_ext;
}

void CtControl::setApplyPolicy(ApplyPolicy policy)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(policy);

  m_policy= policy;
}

void CtControl::getApplyPolicy(ApplyPolicy &policy) const
{
  DEB_MEMBER_FUNCT();

  policy= m_policy;

  DEB_RETURN() << DEB_VAR1(policy);
}

void CtControl::prepareAcq()
{
  DEB_MEMBER_FUNCT();

  Status aStatus;
  getStatus(aStatus);

  if(aStatus.AcquisitionStatus == AcqRunning)
    throw LIMA_CTL_EXC(Error,"Acquisition not finnished");

  resetStatus(false);

  DEB_TRACE() << "Apply hardware bin/roi";
  m_ct_image->applyHard();

  DEB_TRACE() << "Setup Acquisition Buffers";
  m_ct_buffer->setup(this);

  DEB_TRACE() << "Apply Acquisition Parameters";
  m_ct_acq->apply(m_policy);

  DEB_TRACE() << "Apply Shutter Parameters";
  m_ct_shutter->apply();

  DEB_TRACE() << "Prepare Accumulation if needed";
  m_ct_accumulation->prepare();

  DEB_TRACE() << "Prepare Hardware for Acquisition";
  m_hw->prepareAcq();

  DEB_TRACE() << "Apply software bin/roi";
  m_op_int_active= (m_ct_image->applySoft(m_op_int) ||
		    m_op_int->hasReconstructionTask());
  if(m_op_int_active)
    {
      TaskEventCallback *aCbkPt;
      if(m_ct_buffer->isAccumulationActive())
	aCbkPt = new CtAccumulation::_ImageReady4AccCallback(*this->m_ct_accumulation);
      else
	aCbkPt = new _LastBaseImageReadyCallback(*this);
      m_op_int->setEndCallback(aCbkPt);
      aCbkPt->unref();
    }
  else
    m_op_int->setEndCallback(NULL);
  // set ext op
  m_op_ext->prepare();
  m_op_ext->isTaskActive(m_op_ext_link_task_active,m_op_ext_sink_task_active);
  if(m_op_ext_link_task_active)
    {
      _LastImageReadyCallback *aCbkPt = new _LastImageReadyCallback(*this);
      m_op_ext->setEndLinkTaskCallback(aCbkPt);
      aCbkPt->unref();
    }
  else
    m_op_ext->setEndLinkTaskCallback(NULL);

  if(m_op_ext_sink_task_active)
    {
      _LastCounterReadyCallback *aCbkPt = new _LastCounterReadyCallback(*this);
      m_op_ext->setEndSinkTaskCallback(aCbkPt);
      aCbkPt->unref();
    }
  else
    m_op_ext->setEndSinkTaskCallback(NULL);

  m_ct_saving->_prepare();
  m_autosave= m_ct_saving->hasAutoSaveMode();
  m_ready= true;

#ifdef WITH_SPS_IMAGE
  m_display_active_flag = m_ct_sps_image->isActive();
  if(m_display_active_flag)
  {
    FrameDim dim;
    m_ct_image->getImageDim(dim);
    
    m_ct_sps_image->prepare(dim);
  }
#endif
  m_images_ready.clear();
  m_base_images_ready.clear();
  m_images_buffer.clear();
  m_ct_video->_prepareAcq();
}

void CtControl::startAcq()
{
  DEB_MEMBER_FUNCT();

  if (!m_ready)
	throw LIMA_CTL_EXC(Error, "Run prepareAcq before starting acquisition");
  m_running = true;
  TrigMode trigMode;
  m_ct_acq->getTriggerMode(trigMode);

  if(trigMode != IntTrigMult)
    m_ready = false;
  else
    {
      //First check the detector is in Idle Stat
      HwInterface::Status hwStatus;
      m_hw->getStatus(hwStatus);
      if(hwStatus.det != DetIdle)
	throw LIMA_CTL_EXC(Error, "Try to restart before detector is ready");

      //m_ready = false after the last image is triggerred
      int nbFrames4Acq;
      m_ct_acq->getAcqNbFrames(nbFrames4Acq);
      nbFrames4Acq -= 2;
      m_ready = m_status.ImageCounters.LastImageAcquired != nbFrames4Acq;
    }

  AutoMutex aLock(m_cond.mutex());

  m_hw->startAcq();
  m_status.AcquisitionStatus = AcqRunning;
  DEB_TRACE() << "Hardware Acquisition started";
}
 
void CtControl::stopAcq()
{
  DEB_MEMBER_FUNCT();

  m_hw->stopAcq();
  m_ready = false;
  m_running = false;
  DEB_TRACE() << "Hardware Acquisition Stopped";
  _calcAcqStatus();
}
void CtControl::getStatus(Status& status) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  status = m_status;
  HwInterface::Status aHwStatus;
  m_hw->getStatus(aHwStatus);
  DEB_TRACE() << DEB_VAR1(aHwStatus);
  if(aHwStatus.acq == AcqFault)
    status.AcquisitionStatus = AcqFault;
  else if(status.AcquisitionStatus == AcqReady)
    {
      HwInterface::Status aHwStatus;
      m_hw->getStatus(aHwStatus);
      DEB_TRACE() << DEB_VAR1(aHwStatus);
      status.AcquisitionStatus = aHwStatus.acq;
    }
}

/** @brief aborts an acquisiton from a callback thread: it's safe to call 
 *  from a HW thread. Creates a dummy task that calls stopAcq()
 */

void CtControl::abortAcq(AcqStatus acq_status, ErrorCode error_code, 
			 Data& data, bool ctrl_mutex_locked)
{
  DEB_MEMBER_FUNCT();

  if (!ctrl_mutex_locked)
    m_cond.mutex().lock();

  bool status_change = (m_status.AcquisitionStatus != acq_status);
  if (status_change) {
    m_status.AcquisitionStatus = acq_status;
    m_status.Error = error_code;
  }

  if (!ctrl_mutex_locked)
    m_cond.mutex().unlock();

  typedef SinkTaskBase AbortAcqTask;
  AbortAcqTask *abort_task = new AbortAcqTask();
  _AbortAcqCallback *abort_cb = new _AbortAcqCallback(*this);
  abort_task->setEventCallback(abort_cb);
  abort_cb->unref();

  TaskMgr *mgr = new TaskMgr();
  mgr->setInputData(data);
  mgr->addSinkTask(0, abort_task);
  abort_task->unref();
  
  PoolThreadMgr::get().addProcess(mgr);
}


/** @brief an event arrived, calc the new status
 */
void CtControl::_calcAcqStatus()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());
  if(m_status.AcquisitionStatus == AcqRunning)
    {
      const ImageStatus &anImageCnt = m_status.ImageCounters;
      int acq_nb_frames;
      m_ct_acq->getAcqNbFrames(acq_nb_frames);
      if((!m_running ||
	  anImageCnt.LastImageAcquired == (acq_nb_frames - 1)) && // we reach the nb frames asked
	 anImageCnt.LastImageAcquired == anImageCnt.LastImageReady) // processing has finished
	{
	  if(m_autosave)
	    {
	      // Saving is finnished
	      if(anImageCnt.LastImageAcquired == anImageCnt.LastImageSaved)
		m_status.AcquisitionStatus = AcqReady;
	    }
	  else
	    m_status.AcquisitionStatus = AcqReady;
	  DEB_TRACE() << DEB_VAR1(m_status);
	}
    }
}

void CtControl::getImageStatus(ImageStatus& status) const
{
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  status= m_status.ImageCounters;
  
  DEB_RETURN() << DEB_VAR1(status);
}

void CtControl::ReadImage(Data &aReturnData,long frameNumber, 
			  long readBlockLen)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frameNumber, readBlockLen);

  AutoMutex aLock(m_cond.mutex());
  if(m_op_ext_link_task_active)
    {
      if (readBlockLen != 1)
	throw LIMA_CTL_EXC(NotSupported, "Cannot read more than one frame "
			   "at a time with External Operations");
      if(frameNumber < 0)
	frameNumber = m_status.ImageCounters.LastImageReady;

      std::map<int,Data>::iterator i = m_images_buffer.find(frameNumber);
      if(i != m_images_buffer.end())
	aReturnData = i->second;
      else
	{
	  if(frameNumber < m_status.ImageCounters.LastImageReady - m_images_buffer_size)
	    throw LIMA_CTL_EXC(Error,"Frame no more available");
	  else
	    throw LIMA_CTL_EXC(Error,"Frame not available yet");
	}
    }
  else
    {
      aLock.unlock();
      ReadBaseImage(aReturnData,frameNumber,readBlockLen); // todo change when external op activated
    }

  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtControl::ReadBaseImage(Data &aReturnData,long frameNumber, 
			      long readBlockLen)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frameNumber, readBlockLen);

  AcqMode acqMode;
  m_ct_acq->getAcqMode(acqMode);
  int acq_nb_frames;
  m_ct_acq->getAcqNbFrames(acq_nb_frames);

  AutoMutex aLock(m_cond.mutex());
  ImageStatus &imgStatus = m_status.ImageCounters;
  long lastFrame = imgStatus.LastBaseImageReady;

  //Authorize to read the current frame in Accumulation Mode
  if(acqMode == Accumulation && lastFrame < acq_nb_frames - 1)
    ++lastFrame;

  if (frameNumber < 0) {
    frameNumber = lastFrame - (readBlockLen - 1);
    if (frameNumber < 0)
      throw LIMA_CTL_EXC(Error, "Frame(s) not available yet");
  } else if (frameNumber + readBlockLen - 1 > lastFrame)
    throw LIMA_CTL_EXC(Error, "Frame(s) not available yet");
  aLock.unlock();
  m_ct_buffer->getFrame(aReturnData,frameNumber,readBlockLen);
  
  FrameDim img_dim;
  m_ct_image->getImageDim(img_dim);
  int roiWidth = img_dim.getSize().getWidth();
  int roiHeight = img_dim.getSize().getHeight() * readBlockLen;
  if((roiWidth * roiHeight) >
     (aReturnData.dimensions[0] * aReturnData.dimensions[1]))
    throw LIMA_CTL_EXC(Error, "Roi dim > HwBuffer dim");

  aReturnData.dimensions[0] = roiWidth;
  aReturnData.dimensions[1] = roiHeight;

  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtControl::setReconstructionTask(LinkTask *task)
{
  m_op_int->setReconstructionTask(task);
}

void CtControl::reset()
{
  DEB_MEMBER_FUNCT();

  DEB_TRACE() << "Stopping acquisition";
  stopAcq();

  DEB_TRACE() << "Throwing waiting save tasks";
  m_ct_saving->clear();

  DEB_TRACE() << "Suspending task threads";
  PoolThreadMgr& pool_thread_mgr = PoolThreadMgr::get();
  pool_thread_mgr.abort();
 
  DEB_TRACE() << "Reseting hardware";
  m_hw->reset(HwInterface::SoftReset);

  DEB_TRACE() << "Reseting image";
  m_ct_image->reset();

  DEB_TRACE() << "Reseting buffer";
  m_ct_buffer->reset();

  DEB_TRACE() << "Reseting acquisition";
  m_ct_acq->reset();
  
  DEB_TRACE() << "Reseting shutter parameters";
  m_ct_shutter->reset();

#ifdef WITH_SPS_IMAGE
  DEB_TRACE() << "Reseting display";
  m_ct_sps_image->reset();
#endif
  resetStatus(false);
  m_status.AcquisitionStatus = AcqReady;
}

void CtControl::resetStatus(bool only_acq_status)
{
  DEB_MEMBER_FUNCT();
  DEB_TRACE() << "Reseting the status";
  if (only_acq_status)
    m_status.AcquisitionStatus = AcqReady;
  else
    m_status.reset();
}

bool CtControl::newFrameReady(Data& fdata)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(fdata);

  DEB_TRACE() << "Frame acq.nb " << fdata.frameNumber << " received";

  bool aContinueFlag = true;
  AutoMutex aLock(m_cond.mutex());
  if(_checkOverrun(fdata))
    aContinueFlag = false;// Stop Acquisition on overun
  else
    {
      m_status.ImageCounters.LastImageAcquired= fdata.frameNumber;
      aLock.unlock();
  
      TaskMgr *mgr = new TaskMgr();
      mgr->setInputData(fdata);

      int internal_stage = 0;
      if (!m_ct_buffer->isAccumulationActive())
	m_op_int->addTo(*mgr, internal_stage);
  
      int last_link,last_sink;
      m_op_ext->addTo(*mgr, internal_stage, last_link, last_sink);

      if (internal_stage || (last_link >= 0) || (last_sink >= 0))
	PoolThreadMgr::get().addProcess(mgr);
      else
	delete mgr;
      if (!internal_stage)
	newBaseImageReady(fdata);

      if (m_img_status_cb)
	m_img_status_cb->imageStatusChanged(m_status.ImageCounters);
      _calcAcqStatus();
    }

  return aContinueFlag;
}

void CtControl::newBaseImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_status.ImageCounters.LastBaseImageReady + 1;
  bool img_status_changed = false;
  if(aData.frameNumber == expectedImageReady)
    {
      while(!m_base_images_ready.empty())
	{
	  std::set<Data,ltData>::iterator i = m_base_images_ready.begin();
	  long nextExpectedImageReady = expectedImageReady + 1;
	  if(nextExpectedImageReady == i->frameNumber)
	    {
	      expectedImageReady = nextExpectedImageReady;
	      m_base_images_ready.erase(i);
	    }
	  else
	    break;
	}
      ImageStatus &imgStatus = m_status.ImageCounters;
      imgStatus.LastBaseImageReady = expectedImageReady;
      if(!m_op_ext_link_task_active)
	imgStatus.LastImageReady = expectedImageReady;
      
      img_status_changed = true;
    }
  else
    m_base_images_ready.insert(aData);

  aLock.unlock();

  if(m_autosave && !m_op_ext_link_task_active)
    newFrameToSave(aData);

#ifdef WITH_SPS_IMAGE
  if(m_display_active_flag)
    m_ct_sps_image->frameReady(aData);
#endif

  m_ct_video->frameReady(aData);

  if(m_img_status_cb && img_status_changed)
    m_img_status_cb->imageStatusChanged(m_status.ImageCounters);

  _calcAcqStatus();
}

void CtControl::newImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_status.ImageCounters.LastImageReady + 1;
  bool img_status_changed = false;
  if(aData.frameNumber == expectedImageReady)
    {
      while(!m_images_ready.empty())
	{
	  std::set<Data,ltData>::iterator i = m_images_ready.begin();
	  long nextExpectedImageReady = expectedImageReady + 1;
	  if(nextExpectedImageReady == i->frameNumber)
	    {
	      expectedImageReady = nextExpectedImageReady;
	      m_images_ready.erase(i);
	    }
	  else
	    break;
	}
      m_status.ImageCounters.LastImageReady = expectedImageReady;

      img_status_changed = true;
    }
  else
    m_images_ready.insert(aData);

  m_images_buffer.insert(std::pair<int,Data>(aData.frameNumber,aData));
  //pop out the oldest Data
  if(int(m_images_buffer.size()) > m_images_buffer_size)
    m_images_buffer.erase(m_images_buffer.begin());

  aLock.unlock();

  if(m_autosave)
    newFrameToSave(aData);

  if (m_img_status_cb && img_status_changed)
    m_img_status_cb->imageStatusChanged(m_status.ImageCounters);
  _calcAcqStatus();
}

void CtControl::newCounterReady(Data&)
{
  DEB_MEMBER_FUNCT();
  //@todo
}

/** @brief inc the save counter.
 *  @warning due to sequential saving, no check with image number!!!
 *  @see newImageReady
*/
void CtControl::newImageSaved(Data&)
{
  DEB_MEMBER_FUNCT();
  AutoMutex aLock(m_cond.mutex());
  ++m_status.ImageCounters.LastImageSaved;
  aLock.unlock();

  if (m_img_status_cb)
    m_img_status_cb->imageStatusChanged(m_status.ImageCounters);
  _calcAcqStatus();
}

void CtControl::newFrameToSave(Data& fdata)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(fdata);

  DEB_TRACE() << "Add frame acq.nr " << fdata.frameNumber 
	      << " to saving";
  m_ct_saving->frameReady(fdata);
}

void CtControl::registerImageStatusCallback(ImageStatusCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb, m_img_status_cb);

  if (m_img_status_cb) {
    DEB_ERROR() << "ImageStatusCallback already registered";
    throw LIMA_CTL_EXC(InvalidValue, "ImageStatusCallback already registered");
  }

  cb.setImageStatusCallbackGen(this);
  m_img_status_cb = &cb;
}

void CtControl::unregisterImageStatusCallback(ImageStatusCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb, m_img_status_cb);

  if (m_img_status_cb != &cb) {
    DEB_ERROR() << "ImageStatusCallback not registered";
    throw LIMA_CTL_EXC(InvalidValue, "ImageStatusCallback not registered");
  }
  
  m_img_status_cb = NULL;
  cb.setImageStatusCallbackGen(NULL);
}

/** @brief this methode check if an overrun 
 *  @warning this methode is call under lock
 */
bool CtControl::_checkOverrun(Data &aData)
{
  DEB_MEMBER_FUNCT();
  if(m_status.AcquisitionStatus == AcqFault) return true;

  const ImageStatus &imageStatus = m_status.ImageCounters;

  long imageToProcess = imageStatus.LastImageAcquired - 
    imageStatus.LastBaseImageReady;

  long imageToSave =	imageStatus.LastImageAcquired -
    imageStatus.LastImageSaved;

  long nb_buffers;
  m_ct_buffer->getNumber(nb_buffers);
  
  CtSaving::SavingMode mode;
  m_ct_saving->getSavingMode(mode);

  bool overrunFlag = false;
  ErrorCode error_code = NoError;
  if(imageToProcess >= nb_buffers) // Process overrun
    {
      overrunFlag = true;
      error_code = ProcessingOverun;
    }
  else if(mode != CtSaving::Manual && imageToSave >= nb_buffers) // Save overrun
    {
      overrunFlag = true;
      int first_to_save, last_to_save;
      m_ct_saving->getSaveCounters(first_to_save, last_to_save);
      DEB_ERROR() << DEB_VAR2(first_to_save, last_to_save);
      int frames_to_save = last_to_save - first_to_save + 1;
      int frames_to_compress = imageStatus.LastBaseImageReady - last_to_save;
      bool slow_processing = frames_to_compress > frames_to_save;
      DEB_ERROR() << DEB_VAR2(frames_to_compress, frames_to_save);
      error_code = slow_processing ? ProcessingOverun : SaveOverun;
    }

  DEB_PARAM() << DEB_VAR1(overrunFlag);
  if (overrunFlag) {
    DEB_ERROR() << DEB_VAR2(m_status, error_code);
    abortAcq(AcqFault, error_code, aData, true);
  }

  return overrunFlag;
}
// ----------------------------------------------------------------------------
// Struct ImageStatus
// ----------------------------------------------------------------------------
CtControl::ImageStatus::ImageStatus()
{
  DEB_CONSTRUCTOR();
  reset();
}

void CtControl::ImageStatus::reset()
{
  DEB_MEMBER_FUNCT();

  LastImageAcquired	= -1;
  LastBaseImageReady	= -1;
  LastImageReady	= -1;
  LastImageSaved	= -1;
  LastCounterReady	= -1;
  
  DEB_TRACE() << *this;
}
// ----------------------------------------------------------------------------
// Struct Status
// ----------------------------------------------------------------------------

CtControl::Status::Status() :
  AcquisitionStatus(AcqReady),
  Error(NoError),
  CameraStatus(NoCameraError),
  ImageCounters()
{
  DEB_CONSTRUCTOR();
}

void CtControl::Status::reset()
{
  DEB_MEMBER_FUNCT();

  AcquisitionStatus = AcqReady;
  Error = NoError;
  CameraStatus = NoCameraError;
  ImageCounters.reset();
}

// ----------------------------------------------------------------------------
// class ImageStatus
// ----------------------------------------------------------------------------
CtControl::ImageStatusCallback::ImageStatusCallback()
  : m_cb_gen(NULL)
{
  DEB_CONSTRUCTOR();
}

CtControl::ImageStatusCallback::~ImageStatusCallback()
{
  DEB_DESTRUCTOR();
  if (m_cb_gen)
    m_cb_gen->unregisterImageStatusCallback(*this);
}

void 
CtControl::ImageStatusCallback::setImageStatusCallbackGen(CtControl *cb_gen)
{
  DEB_MEMBER_FUNCT();
  m_cb_gen = cb_gen;
}
