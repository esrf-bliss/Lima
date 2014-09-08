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
#include "CtEvent.h"
#ifdef WITH_CONFIG
#include "CtConfig.h"
#endif
#include "SoftOpInternalMgr.h"
#include "SoftOpExternalMgr.h"

#include "HwReconstructionCtrlObj.h"

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

class CtControl::SoftOpErrorHandler : public TaskMgr::EventCallback
{
public:
  SoftOpErrorHandler(CtControl &ctr) : m_ct(ctr) {}

  virtual void error(Data&,const char *errmsg)
  {
    Event *anEvent = new Event(Control,Event::Error,Event::Processing,
			       Event::Default,errmsg);
    CtEvent *eventMgr = m_ct.event();
    eventMgr->reportEvent(anEvent);
  }
private:
  CtControl&	m_ct;
};

class CtControl::_ReconstructionChangeCallback : public HwReconstructionCtrlObj::Callback
{
public:
  _ReconstructionChangeCallback(CtControl& ctrl) : m_ct(ctrl) {}

  virtual ~_ReconstructionChangeCallback() {}
  virtual void change(LinkTask* aNewLinkTaskPt)
  {
    m_ct.setReconstructionTask(aNewLinkTaskPt);
  }
private:
  CtControl& m_ct;
};


class CtControl::ImageStatusThread : public Thread
{
  DEB_CLASS_NAMESPC(DebModControl, "ImageStatusThread", "CtControl");

public:
  ImageStatusThread(Cond& cond, ImageStatusCallback *cb);
  ~ImageStatusThread();

  ImageStatusCallback *cb()
  { 
    return m_cb; 
  }

  void imageStatusChanged(const ImageStatus& status, bool force=false,
			  bool wait=false);

protected:
  virtual void threadFunction();
  
private:
  struct ChangeEvent {
    ImageStatus status;
    bool force;
    bool *finished;
  };
  
  Cond& m_cond;
  ImageStatusCallback *m_cb;
  ImageStatus m_last_status;
  std::list<ChangeEvent *> m_event_list;
};

CtControl::ImageStatusThread::ImageStatusThread(Cond& cond, 
						ImageStatusCallback *cb)
  : m_cond(cond), m_cb(cb)
{
  DEB_CONSTRUCTOR();
  AutoMutex lock(m_cond.mutex());

  start();

  // wait thread is ready
  m_cond.wait();
}

CtControl::ImageStatusThread::~ImageStatusThread()
{
  DEB_DESTRUCTOR();
  AutoMutex lock(m_cond.mutex());

  // signal quit
  m_event_list.push_front(NULL);
  m_cond.broadcast();

  while (!m_event_list.empty())
    m_cond.wait();
}
  
void CtControl::ImageStatusThread::imageStatusChanged(const ImageStatus& status, 
						      bool force, bool wait)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR3(status, force, wait);

  if (wait && !force)
    THROW_CTL_ERROR(InvalidValue) << "Cannot wait for event without forcing";

  AutoMutex lock(m_cond.mutex());

  int cb_rate_policy;
  m_cb->getRatePolicy(cb_rate_policy);
  if (cb_rate_policy == ImageStatusCallback::RateAllFrames)
    force = true;
  if ((status == m_last_status) || ((status < m_last_status) && !force)) {
    DEB_TRACE() << "Skipping";
    return;
  }

  volatile bool finished = false;
  ChangeEvent *event = new ChangeEvent;
  event->status = status;
  event->force = force;
  event->finished = wait ? (bool *) &finished : NULL;

  // skip previous non-forced event
  if (!m_event_list.empty()) {
    ChangeEvent *prev = m_event_list.front();
    if (!prev->force) {
      DEB_TRACE() << "Deleting previous event: " 
		  << DEB_VAR2(prev->status, prev->force);
      m_event_list.pop_front();
      delete prev;
    }
  }

  m_event_list.push_front(event);
  m_last_status = status;
  m_cond.broadcast();

  while (wait && !finished)
    m_cond.wait();
}

void CtControl::ImageStatusThread::threadFunction()
{
  DEB_STATIC_FUNCT();

  AutoMutex lock(m_cond.mutex());

  // notify we're ready
  m_cond.signal();

  while (true) {
    while (m_event_list.empty())
      m_cond.wait();

    ChangeEvent *event = m_event_list.back();
    m_event_list.pop_back();

    if (!event)
      break;

    lock.unlock();
    DEB_TRACE() << "Calling callback: " << DEB_VAR1(event->status);
    m_cb->imageStatusChanged(event->status);
    lock.lock();

    if (event->finished) {
      *event->finished = true;
      m_cond.broadcast();
    }

    delete event;
  }

  m_cond.broadcast();
}


// --- helper


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
  m_img_status_thread(NULL),
  m_reconstruction_cbk(NULL)
{
  DEB_CONSTRUCTOR();

  m_ct_acq= new CtAcquisition(hw);
  m_ct_image= new CtImage(hw,*this);
  m_ct_buffer= new CtBuffer(hw);
  m_ct_shutter = new CtShutter(hw);
  m_ct_accumulation = new CtAccumulation(*this);
  m_ct_video = new CtVideo(*this);
  m_ct_event = new CtEvent(*this);

  //Saving
  m_ct_saving= new CtSaving(*this);
  _LastImageSavedCallback *aSaveCbkPt = new _LastImageSavedCallback(*this);
  m_ct_saving->setEndCallback(aSaveCbkPt);
  aSaveCbkPt->unref();

#ifdef WITH_SPS_IMAGE
  //Sps image
  m_ct_sps_image = new CtSpsImage();
#endif

#ifdef WITH_CONFIG
  m_ct_config = new CtConfig(*this);

#define REGISTER_CONFIG_MODULE(control)					\
    modulePt = control->_getConfigHandler(); \
    if(modulePt)							\
      {									\
	m_ct_config->registerModule(modulePt);				\
	modulePt->unref();						\
      }


  //Add all core callback config
  CtConfig::ModuleTypeCallback* modulePt;
  REGISTER_CONFIG_MODULE(m_ct_acq);
  REGISTER_CONFIG_MODULE(m_ct_saving);
  REGISTER_CONFIG_MODULE(m_ct_image);
  REGISTER_CONFIG_MODULE(m_ct_shutter);
  REGISTER_CONFIG_MODULE(m_ct_accumulation);
  REGISTER_CONFIG_MODULE(m_ct_video);
#endif

  m_op_int = new SoftOpInternalMgr();
  m_op_ext = new SoftOpExternalMgr();

  m_soft_op_error_handler = new SoftOpErrorHandler(*this);

  HwReconstructionCtrlObj* reconstruction_obj;
  if(hw->getHwCtrlObj(reconstruction_obj))
    {
      m_reconstruction_cbk = new _ReconstructionChangeCallback(*this);
      reconstruction_obj->registerReconstructionChangeCallback(*m_reconstruction_cbk);
      LinkTask* rec_task = reconstruction_obj->getReconstructionTask();
      setReconstructionTask(rec_task);
    }
}

CtControl::~CtControl()
{
  DEB_DESTRUCTOR();

  DEB_TRACE() << "Waiting for all threads to finish their tasks";
  PoolThreadMgr& pool_thread_mgr = PoolThreadMgr::get();
  pool_thread_mgr.wait();

  if (m_img_status_thread)
    unregisterImageStatusCallback(*m_img_status_thread->cb());
  
  if(m_reconstruction_cbk)
    {
      HwReconstructionCtrlObj* reconstruction_obj;
      m_hw->getHwCtrlObj(reconstruction_obj);
      reconstruction_obj->unregisterReconstructionChangeCallback(*m_reconstruction_cbk);
      delete m_reconstruction_cbk;
    }

  delete m_ct_saving;
#ifdef WITH_SPS_IMAGE
  delete m_ct_sps_image;
#endif
#ifdef WITH_CONFIG
  delete m_ct_config;
#endif
  delete m_ct_acq;
  delete m_ct_image;
  delete m_ct_buffer;
  delete m_ct_shutter;
  delete m_ct_accumulation;
  delete m_ct_video;

  delete m_op_int;
  delete m_op_ext;

  delete m_soft_op_error_handler;
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
    THROW_CTL_ERROR(Error) << "Acquisition not finished";

  if(aStatus.AcquisitionStatus == AcqConfig)
    THROW_CTL_ERROR(Error) << "Configuration not finished";

  resetStatus(false);
  
  //Clear common header
  m_ct_saving->resetInternalCommonHeader();

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

  DEB_TRACE() << "Prepare Saving if needed";
  m_ct_saving->_prepare(*this);
  m_autosave= m_ct_saving->hasAutoSaveMode();
  m_ready= true;

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
  m_ct_event->_prepareAcq();

  //Check that no software operation is done if Hardware saving is activated
  CtSaving::ManagedMode savingManagedMode;
  m_ct_saving->getManagedMode(savingManagedMode);
  if(savingManagedMode == CtSaving::Hardware &&
     (m_op_int_active || 
      m_op_ext_link_task_active ||
      m_op_ext_sink_task_active ||
#ifdef WITH_SPS_IMAGE
      m_display_active_flag ||
#endif
      m_ct_video->isActive()))
    THROW_CTL_ERROR(Error) << "Can't have any software operation if Hardware saving is active";
}

void CtControl::startAcq()
{
  DEB_MEMBER_FUNCT();

  if (!m_ready)
	THROW_CTL_ERROR(Error) << "Run prepareAcq before starting acquisition";
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
	THROW_CTL_ERROR(Error) << "Try to restart before detector is ready";

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
  m_ct_saving->_stop(*this);
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
    status.AcquisitionStatus = aHwStatus.acq;
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
	 anImageCnt.LastImageAcquired == anImageCnt.LastImageReady && // processing has finished
	 (!m_op_ext_sink_task_active || 
	  anImageCnt.LastCounterReady == anImageCnt.LastImageAcquired)) // ext counters
	{
	  if(m_autosave)
	    {
	      // Saving is finished
	      if(anImageCnt.LastImageAcquired == anImageCnt.LastImageSaved)
		m_status.AcquisitionStatus = AcqReady;
	    }
	  else
	    m_status.AcquisitionStatus = AcqReady;
	  DEB_TRACE() << DEB_VAR1(m_status);
	}

      if (m_img_status_thread && (m_status.AcquisitionStatus != AcqRunning)) {
	aLock.unlock();
	m_img_status_thread->imageStatusChanged(m_status.ImageCounters, 1);
	return;
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
  readBlock(aReturnData, frameNumber, readBlockLen, false);
}

void CtControl::ReadBaseImage(Data &aReturnData,long frameNumber,
			      long readBlockLen)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(frameNumber, readBlockLen);
  readBlock(aReturnData, frameNumber, readBlockLen, true);
}

void CtControl::readBlock(Data &aReturnData,long frameNumber,long readBlockLen,
			  bool baseImage)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR3(frameNumber, readBlockLen, baseImage);

  FrameDim imgDim;
  m_ct_image->getImageDim(imgDim);

  int concatNbFrames = 1;

  AutoMutex aLock(m_cond.mutex());
  ImageStatus &imgStatus = m_status.ImageCounters;
  long lastFrame;
  if (m_op_ext_link_task_active && !baseImage) {
    lastFrame = imgStatus.LastImageReady;
  } else {
    CtSaving::ManagedMode savingManagedMode;
    m_ct_saving->getManagedMode(savingManagedMode);
    if ((savingManagedMode == CtSaving::Hardware) && !baseImage) {
      lastFrame = imgStatus.LastImageSaved;
    } else {
      lastFrame = imgStatus.LastBaseImageReady;

      AcqMode acqMode;
      m_ct_acq->getAcqMode(acqMode);
      //Authorize to read the current frame in Accumulation Mode
      int acq_nb_frames;
      m_ct_acq->getAcqNbFrames(acq_nb_frames);
      if (acqMode == Accumulation && lastFrame < acq_nb_frames - 1)
	++lastFrame;

      // Only read multiple frames in one block if not software ROI
      if (acqMode == Concatenation) {
	FrameDim hwImgDim;
	m_ct_image->getHwImageDim(hwImgDim);
	if (hwImgDim == imgDim)
	  m_ct_acq->getConcatNbFrames(concatNbFrames);
      }
    }
  }

  if (frameNumber < 0) {
    frameNumber = lastFrame - (readBlockLen - 1);
    if (frameNumber < 0)
      THROW_CTL_ERROR(Error) << "Frame(s) not available yet";
  } else if (frameNumber + readBlockLen - 1 > lastFrame)
    THROW_CTL_ERROR(Error) << "Frame(s) not available yet";
  aLock.unlock();

  bool one_block = (readBlockLen == 1);
  if (concatNbFrames > 1) {
    long firstBuffer = frameNumber / concatNbFrames;
    long lastBuffer = (frameNumber + readBlockLen - 1) / concatNbFrames;
    one_block = (firstBuffer == lastBuffer);
  }

  char *p = NULL;
  long framesRead = 0; 
  while (framesRead < readBlockLen) {
    int nbFrames = 1;
    if ((readBlockLen > 1) && (concatNbFrames > 1)) {
      long lastFrame = frameNumber + (readBlockLen - framesRead);
      long nextBuffer = frameNumber / concatNbFrames + 1;
      if (lastFrame > nextBuffer * concatNbFrames)
	lastFrame = nextBuffer * concatNbFrames;
      nbFrames = lastFrame - frameNumber;
    }

    Data auxData;
    readOneImageBuffer(auxData, frameNumber, nbFrames, baseImage);

    int imageSize = imgDim.getMemSize();
    if (imageSize * nbFrames > auxData.size())
      THROW_CTL_ERROR(Error) << "Roi dim (" << imgDim << ") * "
			     << "nbFrames (" << nbFrames << ") > "
			     << "HwBuffer dim (" << auxData.size() << "): "
			     << DEB_VAR1(auxData);

    if (one_block) {
      aReturnData = auxData;
    } else {
      if (p == NULL) {
	aReturnData = auxData;
	Buffer *buffer = new Buffer(imageSize * readBlockLen);
	aReturnData.setBuffer(buffer);
	if (readBlockLen > 1) {
	  if (aReturnData.dimensions.size() == 2)
	    aReturnData.dimensions.push_back(readBlockLen);
	  else
	    aReturnData.dimensions[2] = readBlockLen;
	}
	p = (char *) aReturnData.data();
      }
      memcpy(p, auxData.data(), imageSize * nbFrames);
      p += imageSize * nbFrames;
    }

    framesRead += nbFrames;
  }

  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtControl::readOneImageBuffer(Data &aReturnData,long frameNumber, 
				   long readBlockLen, bool baseImage)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR3(frameNumber, readBlockLen, baseImage);

  AutoMutex aLock(m_cond.mutex());
  if(m_op_ext_link_task_active && !baseImage)
    {
      std::map<int,Data>::iterator i = m_images_buffer.find(frameNumber);
      if(i != m_images_buffer.end())
	aReturnData = i->second;
      else
	{
	  if(frameNumber < m_status.ImageCounters.LastImageReady - m_images_buffer_size)
	    THROW_CTL_ERROR(Error) << "Frame no more available";
	  else
	    THROW_CTL_ERROR(Error) << "Frame not available yet";
	}
    }
  else
    {
      aLock.unlock();
      CtSaving::ManagedMode savingManagedMode;
      m_ct_saving->getManagedMode(savingManagedMode);
      if((savingManagedMode == CtSaving::Hardware) && !baseImage) {
	m_ct_saving->_ReadImage(aReturnData,frameNumber);
      } else {
	m_ct_buffer->getFrame(aReturnData,frameNumber,readBlockLen);
	FrameDim imgDim;
	m_ct_image->getImageDim(imgDim);
	aReturnData.dimensions[0] = imgDim.getSize().getWidth();
	aReturnData.dimensions[1] = imgDim.getSize().getHeight();
      }
    }

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
  if (only_acq_status) {
    m_status.AcquisitionStatus = AcqReady;
  } else {
    m_status.reset();
    if (m_img_status_thread)
      m_img_status_thread->imageStatusChanged(m_status.ImageCounters, 1);
  }
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
      mgr->setEventCallback(m_soft_op_error_handler);
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

      if (m_img_status_thread)
	m_img_status_thread->imageStatusChanged(m_status.ImageCounters);
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

  if(m_img_status_thread)
    m_img_status_thread->imageStatusChanged(m_status.ImageCounters);

  _calcAcqStatus();
}

void CtControl::newImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_status.ImageCounters.LastImageReady + 1;
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

  if (m_img_status_thread)
    m_img_status_thread->imageStatusChanged(m_status.ImageCounters);
  _calcAcqStatus();
}

void CtControl::newCounterReady(Data&)
{
  DEB_MEMBER_FUNCT();
  AutoMutex aLock(m_cond.mutex());
  ++m_status.ImageCounters.LastCounterReady;
  aLock.unlock();
  _calcAcqStatus();
}

/** @brief inc the save counter.
 *  @warning due to sequential saving, no check with image number!!!
 *  @see newImageReady
*/
void CtControl::newImageSaved(Data&)
{
  DEB_MEMBER_FUNCT();
  CtSaving::ManagedMode savingManagedMode;
  m_ct_saving->getManagedMode(savingManagedMode);
  AutoMutex aLock(m_cond.mutex());
  ++m_status.ImageCounters.LastImageSaved;
  if(savingManagedMode == CtSaving::Hardware)
    {
      m_status.ImageCounters.LastImageAcquired = m_status.ImageCounters.LastImageSaved;
      m_status.ImageCounters.LastBaseImageReady = m_status.ImageCounters.LastImageSaved;
      m_status.ImageCounters.LastImageReady = m_status.ImageCounters.LastImageSaved;
    }
  aLock.unlock();

  if (m_img_status_thread)
    m_img_status_thread->imageStatusChanged(m_status.ImageCounters);
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
  DEB_PARAM() << DEB_VAR2(&cb, m_img_status_thread);

  if (m_img_status_thread)
    THROW_CTL_ERROR(InvalidValue) << "ImageStatusCallback already registered";

  cb.setImageStatusCallbackGen(this);
  m_img_status_thread = new ImageStatusThread(m_cond, &cb);
}

void CtControl::unregisterImageStatusCallback(ImageStatusCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb, m_img_status_thread);

  if (!m_img_status_thread || (m_img_status_thread->cb() != &cb))
    THROW_CTL_ERROR(InvalidValue) << "ImageStatusCallback not registered";
  
  delete m_img_status_thread;
  m_img_status_thread = NULL;
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
  : m_cb_gen(NULL), m_rate_policy(RateAsFastAsPossible)
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

void
CtControl::ImageStatusCallback::setRatePolicy(int rate_policy)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(rate_policy);
  m_rate_policy = rate_policy;
}

void
CtControl::ImageStatusCallback::getRatePolicy(int& rate_policy)
{
  DEB_MEMBER_FUNCT();
  rate_policy = m_rate_policy;
  DEB_RETURN() << DEB_VAR1(rate_policy);
}

#ifdef WIN32
CtAcquisition* CtControl::acquisition() 		{ return m_ct_acq; }
CtSaving* 		CtControl::saving() 		{ return m_ct_saving; }
#ifdef WITH_SPS_IMAGE
CtSpsImage* 	CtControl::display() 			{ return m_ct_sps_image; }
#endif
CtImage* 		CtControl::image() 		{ return m_ct_image; }
CtBuffer* 		CtControl::buffer() 		{ return m_ct_buffer; }
CtAccumulation* 	CtControl::accumulation() 	{ return m_ct_accumulation; }
CtVideo*		CtControl::video()		{ return m_ct_video;}
CtShutter* 		CtControl::shutter() 		{ return m_ct_shutter; }
CtEvent* 		CtControl::event()		{ return m_ct_event; }
#ifdef WITH_CONFIG
CtConfig*		CtControl::config()		{ return m_ct_config; }
#endif

SoftOpExternalMgr* 	CtControl::externalOperation() 	{return m_op_ext;}

HwInterface* 	CtControl::hwInterface() 		{return m_hw;}
#endif
