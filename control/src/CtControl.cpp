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

#include "lima/CtControl.h"
#include "lima/CtSaving.h"
#ifdef WITH_SPS_IMAGE
#include "lima/CtSpsImage.h"
#endif
#include "lima/CtAcquisition.h"
#include "lima/CtImage.h"
#include "lima/CtBuffer.h"
#include "lima/CtShutter.h"
#include "lima/CtAccumulation.h"
#include "lima/CtVideo.h"
#include "lima/CtEvent.h"
#ifdef WITH_CONFIG
#include "lima/CtConfig.h"
#endif
#include "lima/SoftOpInternalMgr.h"
#include "lima/SoftOpExternalMgr.h"

#include "lima/HwReconstructionCtrlObj.h"

#include "processlib/PoolThreadMgr.h"

using namespace lima;

static const int ABORT_TASK_PRIORITY = 10;

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
  
  virtual void finished(Data &/*aData*/)
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

  bool waitIdle(double timeout=-1.0);

protected:
  virtual void threadFunction();
  
private:
  struct ChangeEvent {

    ChangeEvent() : force(false), finished(NULL) {}
    ImageStatus status;
    bool force;
    bool *finished;
  };
  
  Cond& m_cond;
  ImageStatusCallback *m_cb;
  ImageStatus m_last_status;
  std::list<ChangeEvent *> m_event_list;
  bool m_waiting;
};

CtControl::ImageStatusThread::ImageStatusThread(Cond& cond, 
						ImageStatusCallback *cb)
  : m_cond(cond), m_cb(cb), m_waiting(false)
{
  DEB_CONSTRUCTOR();
  start();
  waitIdle();
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

  ImageStatusCallback::RatePolicy cb_rate_policy;
  m_cb->getRatePolicy(cb_rate_policy);
  if (cb_rate_policy == ImageStatusCallback::RateAllFrames)
    force = true;
  if ((status == m_last_status) || ((status < m_last_status) && !force)) {
    DEB_TRACE() << "Skipping";
    return;
  }

  bool finished = false;
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

  while (true) {
    while (m_event_list.empty()) {
      // only broadcast if m_waiting changed
      if (!m_waiting) {
	m_waiting = true;
	m_cond.broadcast();
      }
      m_cond.wait();
    }

    m_waiting = false;

    ChangeEvent *event = m_event_list.back();
    m_event_list.pop_back();

    if (!event)
      break;

    {
      AutoMutexUnlock u(lock);
      DEB_TRACE() << "Calling callback: " << DEB_VAR1(event->status);
      m_cb->imageStatusChanged(event->status);
    }

    if (event->finished) {
      *event->finished = true;
      m_cond.broadcast();
    }

    delete event;
  }

  m_cond.broadcast();
}

bool CtControl::ImageStatusThread::waitIdle(double timeout)
{
  DEB_MEMBER_FUNCT();

  AutoMutex lock(m_cond.mutex());

  while (!m_waiting)
    if (!m_cond.wait(timeout))
      break;

  return m_waiting;
}


// --- helper


CtControl::CtControl(HwInterface *hw) :
  m_hw(hw),
  m_op_int_active(false),
  m_op_ext_link_task_active(false),
  m_op_ext_sink_task_active(false),
  m_base_images_ready(CtControl::ltData()),
  m_images_ready(CtControl::ltData()),
  m_images_saved(CtControl::ltData()),
  m_images_buffer_size(16),
  m_policy(All), m_ready(false),
  m_autosave(false), m_running(false),
  m_reconstruction_cbk(NULL),
  m_prepare_timeout(2)
{
  DEB_CONSTRUCTOR();

  // Forces initialisation of PoolThreadMgr (pthread_atfork)
  PoolThreadMgr::get();

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
  m_op_int->setFirstProcessingInPlace(hw->firstProcessingInPlace());
}

CtControl::~CtControl()
{
  DEB_DESTRUCTOR();
  
  Status aStatus;
  getStatus(aStatus);
  if(aStatus.AcquisitionStatus == AcqRunning)
  {
    DEB_WARNING() << "Acquisition is still running, stopping acquisition";
    stopAcq();
  }

  DEB_TRACE() << "Waiting for all threads to finish their tasks";
  PoolThreadMgr& pool_thread_mgr = PoolThreadMgr::get();
  pool_thread_mgr.wait();

  {
    ReadWriteLock::WriteGuard guard(m_img_status_thread_list_lock);
    for(ImageStatusThreadList::iterator i = m_img_status_thread_list.begin();
	i != m_img_status_thread_list.end();++i)
      {
	(*i)->cb()->setImageStatusCallbackGen(NULL);
	delete *i;
      }
  }

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
  delete m_ct_event;

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

  {
    ReadWriteLock::ReadGuard guard(m_img_status_thread_list_lock);
    ImageStatusThreadList::iterator i, end = m_img_status_thread_list.end();
    for (i = m_img_status_thread_list.begin(); i != end; ++i)
      if (!(*i)->waitIdle(m_prepare_timeout))
	THROW_CTL_ERROR(Error) << "ImageStatusCallback still active after "
			       << m_prepare_timeout << " sec";
  }

  Status aStatus;
  getStatus(aStatus);

  if(aStatus.AcquisitionStatus == AcqRunning)
    THROW_CTL_ERROR(Error) << "Acquisition not finished";

  if(aStatus.AcquisitionStatus == AcqConfig)
    THROW_CTL_ERROR(Error) << "Configuration not finished";

  {
    AutoMutex aLock(m_cond.mutex());
    m_ready= false; // prevent calling startAcq before full preparation
  }

  //Abort previous acquisition tasks
  PoolThreadMgr::get().abort();
  m_ct_saving->_resetReadyFlag();

  // reset acq status without notifying callbacks
  resetStatus(true);
  
  //Clear all re-ordered image counters
  m_images_ready.clear();
  m_base_images_ready.clear();
  m_images_buffer.clear();
  m_images_saved.clear();

  //Clear common header
  m_ct_saving->resetInternalCommonHeader();

  DEB_TRACE() << "Apply hardware bin/roi";
  m_ct_image->applyHard();

  DEB_TRACE() << "Setup Acquisition Buffers";
  m_ct_buffer->setup(this);

  DEB_TRACE() << "Apply Acquisition Parameters";
  m_ct_acq->apply(m_policy, this);

  DEB_TRACE() << "Apply Shutter Parameters";
  m_ct_shutter->apply();

  DEB_TRACE() << "Prepare Accumulation if needed";
  m_ct_accumulation->prepare();

  DEB_TRACE() << "Prepare Saving if needed";
  m_ct_saving->_prepare();
  m_autosave= m_ct_saving->hasAutoSaveMode();

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

  // reset status and notify callbacks
  resetStatus(false);
  
  AutoMutex aLock(m_cond.mutex());
  m_ready= true;
}

void CtControl::startAcq()
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());

  if (!m_ready)
    THROW_CTL_ERROR(Error) << "Run prepareAcq before starting acquisition";

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

  bool was_running = m_running;
  m_running = true;
  m_status.AcquisitionStatus = AcqRunning;

  if (!was_running)
    m_ct_video->_startAcqTime();

  aLock.unlock();

  try {
    m_hw->startAcq();
    DEB_TRACE() << "Hardware Acquisition started";
  } catch (...) {
    DEB_ERROR() << "HwInterface::startAcq failed!";
    _stopAcq(true);
    throw;
  }
}
 
void CtControl::stopAcq()
{
  DEB_MEMBER_FUNCT();

  try {
    m_hw->stopAcq();
    DEB_TRACE() << "Hardware Acquisition stopped";
  } catch (...) {
    DEB_ERROR() << "HwInterface::stopAcq failed!";
    _stopAcq(true);
    throw;
  }

  _stopAcq(false);
}

void CtControl::_stopAcq(bool faulty_acq)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(faulty_acq);

  {
    AutoMutex aLock(m_cond.mutex());
    m_ready = false;
    m_running = false;
    if (faulty_acq)
      m_status.AcquisitionStatus = AcqFault;
  }

  _calcAcqStatus();
  m_ct_saving->_stop();
}

/** @brief stop an acquisition and purge all pending tasks.
 */
void CtControl::abortAcq()
{
  DEB_MEMBER_FUNCT();

  stopAcq();
  PoolThreadMgr::get().abort();
  
  m_ct_saving->_resetReadyFlag();

  AutoMutex aLock(m_cond.mutex());
  m_status.AcquisitionStatus = AcqReady;
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
    
  DEB_RETURN() << DEB_VAR1(status);
}

/** @brief This function is DEPRECATED. Use stopAcqAsync instead
 */

void CtControl::abortAcq(AcqStatus acq_status, ErrorCode error_code, 
			 Data& data, bool ctrl_mutex_locked)
{
  DEB_MEMBER_FUNCT();
  DEB_WARNING() << "This function is deprecated! Use stopAcqAsync instead";
  if (ctrl_mutex_locked)
    THROW_CTL_ERROR(InvalidValue) << DEB_VAR1(ctrl_mutex_locked);
  stopAcqAsync(acq_status, error_code, data);
}

/** @brief aborts an acquisiton from a callback thread: it's safe to call 
 *  from a HW thread. Creates a dummy task that calls stopAcq() and waits
 *  for all buffers to be released
 */

void CtControl::stopAcqAsync(AcqStatus acq_status, ErrorCode error_code,
			     Data& data)
{
  DEB_MEMBER_FUNCT();

  {
    AutoMutex lock(m_cond.mutex());
    bool status_change = (m_status.AcquisitionStatus != acq_status);
    if (status_change) {
      m_status.AcquisitionStatus = acq_status;
      m_status.Error = error_code;
    }
  }

  typedef SinkTaskBase AbortAcqTask;
  AbortAcqTask *abort_task = new AbortAcqTask();
  _AbortAcqCallback *abort_cb = new _AbortAcqCallback(*this);
  abort_task->setEventCallback(abort_cb);
  abort_cb->unref();

  TaskMgr *mgr = new TaskMgr(ABORT_TASK_PRIORITY);
  mgr->setInputData(data);
  mgr->addSinkTask(0, abort_task);
  abort_task->unref();
  
  PoolThreadMgr::get().addProcess(mgr);
}


/** @brief notify ImageStatusThreads of new ImageCounters.
 *  If force is true, the event is not ignored
 */
void CtControl::_updateImageStatusThreads(bool force)
{
  DEB_MEMBER_FUNCT();
    
  ReadWriteLock::ReadGuard guard(m_img_status_thread_list_lock);
  for(ImageStatusThreadList::iterator i = m_img_status_thread_list.begin();
      i != m_img_status_thread_list.end();++i)
    (*i)->imageStatusChanged(m_status.ImageCounters, force);
}


/** @brief an event arrived, calc the new status
 */
void CtControl::_calcAcqStatus()
{
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());

  AcqStatus& acq_status = m_status.AcquisitionStatus;
  DEB_TRACE() << DEB_VAR1(acq_status);
  if((acq_status != AcqRunning) && (acq_status != AcqFault))
    return;

  const ImageStatus& img_cntrs = m_status.ImageCounters;
  int acq_nb_frames;
  m_ct_acq->getAcqNbFrames(acq_nb_frames);
  
  long last_frame = ((m_running && (acq_nb_frames > 0)) ? (acq_nb_frames - 1) :
				 img_cntrs.LastImageAcquired);

  DEB_TRACE() << DEB_VAR2(last_frame, img_cntrs);

  bool hw_acq_end = (img_cntrs.LastImageAcquired == last_frame);
  bool img_op_end = (img_cntrs.LastImageReady == last_frame);
  bool cnt_op_end = (!m_op_ext_sink_task_active ||
		     (img_cntrs.LastCounterReady == last_frame));
  bool save_end = (!m_autosave || (img_cntrs.LastImageSaved) == last_frame);
  bool acq_end = (hw_acq_end && img_op_end && cnt_op_end && save_end);

  DEB_TRACE() << DEB_VAR5(hw_acq_end, img_op_end, cnt_op_end, save_end, acq_end);
  
  if(!acq_end)
    return;
  
  if(acq_status == AcqRunning)
    acq_status = AcqReady;
  DEB_TRACE() << DEB_VAR1(m_status);

  aLock.unlock();

  _updateImageStatusThreads(true);
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
    frameNumber += nbFrames;
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
      if(i == m_images_buffer.end())
	{
	  if(frameNumber < m_status.ImageCounters.LastImageReady - m_images_buffer_size)
	    THROW_CTL_ERROR(Error) << "Frame no more available";
	  else
	    THROW_CTL_ERROR(Error) << "Frame not available yet";
	}
      aReturnData = i->second;
      DEB_RETURN() << DEB_VAR1(aReturnData);
      return;
    }
  aLock.unlock();

  CtSaving::ManagedMode savingManagedMode;
  m_ct_saving->getManagedMode(savingManagedMode);
  if((savingManagedMode == CtSaving::Hardware) && !baseImage) {
    m_ct_saving->_ReadImage(aReturnData,frameNumber);
  } else {
    m_ct_buffer->getFrame(aReturnData,frameNumber,readBlockLen);
    // if the processing is not in place, the processed buffer is lost.
    // need to re-do the internal processing. Except for the accumulation
    // mode: accumulated images are already processed
    AcqMode acqMode;
    m_ct_acq->getAcqMode(acqMode);
    if(!m_hw->firstProcessingInPlace() && (acqMode != Accumulation))
      {
	TaskMgr mgr;
	mgr.setInputData(aReturnData);
	int stage = 0;
	m_op_int->addTo(mgr,stage,false);
	if(stage)
	  aReturnData = mgr.syncProcess();
      }
    FrameDim imgDim;
    m_ct_image->getImageDim(imgDim);
    aReturnData.dimensions[0] = imgDim.getSize().getWidth();
    aReturnData.dimensions[1] = imgDim.getSize().getHeight();
  }

  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtControl::setReconstructionTask(LinkTask *task)
{
  m_op_int->setReconstructionTask(task);
}

void CtControl::setPrepareTimeout(double timeout)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(timeout);
  if ((timeout < 0) && (timeout != -1))
    THROW_CTL_ERROR(InvalidValue) << "Invalid timeout: " << timeout;
  m_prepare_timeout = timeout;
}

void CtControl::getPrepareTimeout(double& timeout) const
{
  DEB_MEMBER_FUNCT();
  timeout = m_prepare_timeout;
  DEB_RETURN() << DEB_VAR1(timeout);
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
}

void CtControl::resetStatus(bool only_acq_status)
{
  DEB_MEMBER_FUNCT();
  DEB_TRACE() << "Reseting the status";
  AutoMutex aLock(m_cond.mutex());
  if (only_acq_status) {
    m_status.AcquisitionStatus = AcqReady;
    return;
  }
  m_status.reset();
  aLock.unlock();
  _updateImageStatusThreads(true);
}

bool CtControl::newFrameReady(Data& fdata)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(fdata);

  DEB_TRACE() << "Frame acq.nb " << fdata.frameNumber << " received";

  {
    AutoMutex aLock(m_cond.mutex());
    if(_checkOverrun(fdata, aLock))
      return false;// Stop Acquisition on overun

    m_status.ImageCounters.LastImageAcquired= fdata.frameNumber;
  }

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

  _updateImageStatusThreads(false);
  _calcAcqStatus();

  return true;
}

void CtControl::newBaseImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  ImageStatus &imgStatus = m_status.ImageCounters;
  imgStatus.LastBaseImageReady = _increment_image_cnt(aData,imgStatus.LastBaseImageReady,
						      m_base_images_ready);
  if(!m_op_ext_link_task_active)
    imgStatus.LastImageReady = imgStatus.LastBaseImageReady;

  aLock.unlock();

  if(m_autosave && !m_op_ext_link_task_active)
    newFrameToSave(aData);

#ifdef WITH_SPS_IMAGE
  if(m_display_active_flag)
    m_ct_sps_image->frameReady(aData);
#endif
  CtVideo::VideoSource source;m_ct_video->getVideoSource(source);
  if(source == CtVideo::BASE_IMAGE ||
        (source == CtVideo::LAST_IMAGE && !m_op_ext_link_task_active))
    m_ct_video->frameReady(aData);

  _updateImageStatusThreads(false);
  _calcAcqStatus();
}

void CtControl::newImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());

  ImageStatus &imgStatus = m_status.ImageCounters;
  imgStatus.LastImageReady = _increment_image_cnt(aData,imgStatus.LastImageReady,
						  m_images_ready);

  m_images_buffer.insert(std::pair<int,Data>(aData.frameNumber,aData));
  //pop out the oldest Data
  if(int(m_images_buffer.size()) > m_images_buffer_size)
    m_images_buffer.erase(m_images_buffer.begin());

  aLock.unlock();

  if(m_autosave)
    newFrameToSave(aData);

  CtVideo::VideoSource source;m_ct_video->getVideoSource(source);
  if(source == CtVideo::LAST_IMAGE)
    m_ct_video->frameReady(aData);

  _updateImageStatusThreads(false);
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
/** function to re-order image counters
 */

long CtControl::_increment_image_cnt(Data& aData,
				     long image_cnt,SortedDataType& cnt,
				     long step)
{
  long expectedImageCnt = image_cnt + step;
  if(aData.frameNumber != expectedImageCnt)
    {
      cnt.insert(aData);
      return image_cnt;
    }

  while(!cnt.empty())
    {
      SortedDataType::iterator i = cnt.begin();
      if(i->frameNumber != expectedImageCnt + step)
	break;
      expectedImageCnt = i->frameNumber;
      cnt.erase(i);
    }
  return expectedImageCnt;
}

/** @brief inc the save counter.
 *  @warning due to sequential saving, no check with image number!!!
 *  @see newImageReady
*/
void CtControl::newImageSaved(Data &data)
{
  DEB_MEMBER_FUNCT();

  CtSaving::ManagedMode savingManagedMode;
  m_ct_saving->getManagedMode(savingManagedMode);
  unsigned long frames_per_callback = 1;
  if(savingManagedMode == CtSaving::Hardware) {
    m_ct_saving->getFramesPerFile(frames_per_callback);
    int extra_frames = (data.frameNumber + 1) % frames_per_callback;
    if (extra_frames > 0)
      frames_per_callback = extra_frames;
  }
  AutoMutex aLock(m_cond.mutex());
  ImageStatus &imgStatus = m_status.ImageCounters;

  imgStatus.LastImageSaved = _increment_image_cnt(data,imgStatus.LastImageSaved,
						  m_images_saved,
						  frames_per_callback);
  if(savingManagedMode == CtSaving::Hardware)
    {
      imgStatus.LastImageAcquired = imgStatus.LastImageSaved;
      imgStatus.LastBaseImageReady = imgStatus.LastImageSaved;
      imgStatus.LastImageReady = imgStatus.LastImageSaved;
    }

  aLock.unlock();

  _updateImageStatusThreads(false);
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

/** registerImageStatusCallback is not thread safe!!!
 */
void CtControl::registerImageStatusCallback(ImageStatusCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(&cb);

  Status aStatus;
  getStatus(aStatus);
  if(aStatus.AcquisitionStatus == AcqRunning)
    THROW_CTL_ERROR(Error) << "Can't register callback if acquisition is running";

  ReadWriteLock::WriteGuard guard(m_img_status_thread_list_lock);
  ImageStatusThreadList::iterator i, end = m_img_status_thread_list.end();
  i = std::find_if(m_img_status_thread_list.begin(), end,
		   [&cb] (ImageStatusThread *t) { return t->cb() == &cb; });
  if(i != end)
    THROW_CTL_ERROR(InvalidValue) << "ImageStatusCallback already registered";
  AutoPtr<ImageStatusThread> thread = new ImageStatusThread(m_cond, &cb);
  cb.setImageStatusCallbackGen(this);
  m_img_status_thread_list.push_back(thread);
  thread.forget();
}
/** unregisterImageStatusCallback is not thread safe!!!
 */
void CtControl::unregisterImageStatusCallback(ImageStatusCallback& cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(&cb);

  ReadWriteLock::WriteGuard guard(m_img_status_thread_list_lock);
  ImageStatusThreadList::iterator i, end = m_img_status_thread_list.end();
  i = std::find_if(m_img_status_thread_list.begin(), end,
		   [&cb] (ImageStatusThread *t) { return t->cb() == &cb; });
  if(i == end) {
    DEB_WARNING() << "Unregistering callback while destroying CtControl";
    return;
  }

  AutoPtr<ImageStatusThread> thread = *i; 
  m_img_status_thread_list.erase(i);
  cb.setImageStatusCallbackGen(NULL);
}

/** @brief this methode check if an overrun 
 *  @warning this methode is call under lock
 */
bool CtControl::_checkOverrun(Data& aData, AutoMutex& l)
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

  if (overrunFlag) {
    AutoMutexUnlock u(l);
    DEB_ERROR() << DEB_VAR2(m_status, error_code);
    stopAcqAsync(AcqFault, error_code, aData);
  }

  DEB_RETURN() << DEB_VAR1(overrunFlag);
  return overrunFlag;
}
// ----------------------------------------------------------------------------
// Struct ImageStatus
// ----------------------------------------------------------------------------
CtControl::ImageStatus::ImageStatus() :
    LastImageAcquired(-1),
    LastBaseImageReady(-1),
    LastImageReady(-1),
    LastImageSaved(-1),
    LastCounterReady(-1)
{
  DEB_CONSTRUCTOR();
}

CtControl::ImageStatus::ImageStatus(long lastImgAcq, long lastBaseImgReady,
				    long lastImgReady, long lastImgSaved,
				    long lastCntReady):
    LastImageAcquired(lastImgAcq), LastBaseImageReady(lastBaseImgReady),
    LastImageReady(lastImgReady), LastImageSaved(lastImgSaved),
    LastCounterReady(lastCntReady)
{
  DEB_CONSTRUCTOR();
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

CtControl::Status::Status(AcqStatus acq_status, ErrorCode err,
			  CameraErrorCode cam_err,
			  const ImageStatus& img_status) :
  AcquisitionStatus(acq_status),
  Error(err),
  CameraStatus(cam_err),
  ImageCounters(img_status)
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
CtControl::ImageStatusCallback::setRatePolicy(RatePolicy rate_policy)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(rate_policy);
  m_rate_policy = rate_policy;
}

void
CtControl::ImageStatusCallback::getRatePolicy(RatePolicy& rate_policy)
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
