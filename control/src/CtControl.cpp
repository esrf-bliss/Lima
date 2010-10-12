#include <string>
#include <sstream>

#include "CtControl.h"
#include "CtSaving.h"
#include "CtSpsImage.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtBuffer.h"
#include "CtShutter.h"
#include "CtAccumulation.h"

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



CtControl::CtControl(HwInterface *hw) :
  m_hw(hw),
  m_op_int_active(false),
  m_op_ext_link_task_active(false),
  m_op_ext_sink_task_active(false),
  m_base_images_ready(CtControl::ltData()),
  m_images_ready(CtControl::ltData()),
  m_policy(All), m_ready(false),
  m_autosave(false), m_started(false),
  m_img_status_cb(NULL)
{
  DEB_CONSTRUCTOR();

  m_ct_acq= new CtAcquisition(hw);
  m_ct_image= new CtImage(hw,*this);
  m_ct_buffer= new CtBuffer(hw);
  m_ct_shutter = new CtShutter(hw);
  m_ct_accumulation = new CtAccumulation(*this);

  //Saving
  m_ct_saving= new CtSaving(*this);
  _LastImageSavedCallback *aSaveCbkPt = new _LastImageSavedCallback(*this);
  m_ct_saving->setEndCallback(aSaveCbkPt);
  aSaveCbkPt->unref();

  //Sps image
  m_ct_sps_image = new CtSpsImage();
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
  delete m_ct_sps_image;
  delete m_ct_acq;
  delete m_ct_image;
  delete m_ct_buffer;
  delete m_ct_accumulation;
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

  resetStatus(false);

  DEB_TRACE() << "Apply hardware bin/roi";
  m_ct_image->applyHard();

  DEB_TRACE() << "Setup Acquisition Buffers";
  m_ct_buffer->setup(this);

  DEB_TRACE() << "Apply Acquisition Parameters";
  m_ct_acq->apply(m_policy);

  DEB_TRACE() << "Prepare Accumulation if needed";
  m_ct_accumulation->prepare();

  DEB_TRACE() << "Prepare Hardware for Acquisition";
  m_hw->prepareAcq();

  DEB_TRACE() << "Apply software bin/roi";
  m_op_int_active= m_ct_image->applySoft(m_op_int);
  if (m_op_int->hasReconstructionTask())
    m_op_int_active= true;
  if(m_op_int_active)
    {
      _LastBaseImageReadyCallback *aCbkPt = new _LastBaseImageReadyCallback(*this);
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

  m_autosave= m_ct_saving->hasAutoSaveMode();
  if (m_autosave)
    m_ct_saving->resetLastFrameNb();
  else
    DEB_TRACE() << "No auto save activated";
  m_ready= true;

  m_display_active_flag = m_ct_sps_image->isActive();
  if(m_display_active_flag)
  {
    FrameDim dim;
    m_ct_image->getImageDim(dim);
    
    m_ct_sps_image->prepare(dim);
  }

  m_images_ready.clear();
  m_base_images_ready.clear();
}

void CtControl::startAcq()
{
  DEB_MEMBER_FUNCT();

  if (!m_ready)
	throw LIMA_CTL_EXC(Error, "Run prepareAcq before starting acquisition");

  m_ready = false;
  m_hw->startAcq();
  m_started = true;

  DEB_TRACE() << "Hardware Acquisition started";
}
 
void CtControl::stopAcq()
{
  DEB_MEMBER_FUNCT();

  m_hw->stopAcq();
  m_started = false;

  DEB_TRACE() << "Hardware Acquisition Stopped";
}

void CtControl::getStatus(Status& status) const
{
  DEB_MEMBER_FUNCT();

  AutoMutex aLock(m_cond.mutex());

  DEB_TRACE() << DEB_VAR2(m_status.AcquisitionStatus, m_started);
  bool fault = (m_status.AcquisitionStatus == AcqFault);
  if(!fault && m_started)
    {
      const ImageStatus &anImageCnt = m_status.ImageCounters;

      // Check if save has finnished
      CtSaving::SavingMode aSavemode;
      m_ct_saving->getSavingMode(aSavemode);
      
      bool aSavingIdleFlag = true;
      if(aSavemode != CtSaving::Manual)
	aSavingIdleFlag = anImageCnt.LastImageAcquired == anImageCnt.LastImageSaved;
      // End Check Save

      // Check if processing has finnished
      bool aProcessingIdle = anImageCnt.LastImageAcquired == anImageCnt.LastImageReady;

      if(aSavingIdleFlag && aProcessingIdle)
	{
	  HwInterface::Status aHwStatus;
	  m_hw->getStatus(aHwStatus);
	  DEB_TRACE() << DEB_VAR1(aHwStatus);
	  // set the status to hw acquisition status
	  m_status.AcquisitionStatus = aHwStatus.acq;
	  AcqMode anAcqMode;
	  m_ct_acq->getAcqMode(anAcqMode);

	  if(anAcqMode != Accumulation)
	    {
	      int last_hw_frame = m_hw->getNbAcquiredFrames() - 1;
	      bool aFalseIdle = ((aHwStatus.acq == AcqReady) && 
				 (anImageCnt.LastImageAcquired != last_hw_frame));
	      if (aFalseIdle)
		m_status.AcquisitionStatus = AcqRunning;
	    }
	}
      else
	m_status.AcquisitionStatus = AcqRunning;
    }
  else if (!fault)
    m_status.AcquisitionStatus = AcqReady;
  
  status= m_status;
  
  DEB_RETURN() << DEB_VAR1(status);
}

void CtControl::getImageStatus(ImageStatus& status) const
{
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  status= m_status.ImageCounters;
  
  DEB_RETURN() << DEB_VAR1(status);
}

void CtControl::ReadImage(Data &aReturnData,long frameNumber)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frameNumber);

  ReadBaseImage(aReturnData,frameNumber); // todo change when external op activated

  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtControl::ReadBaseImage(Data &aReturnData,long frameNumber)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(frameNumber);

  AutoMutex aLock(m_cond.mutex());
  ImageStatus &imgStatus = m_status.ImageCounters;
  if(frameNumber < 0)
    frameNumber = imgStatus.LastBaseImageReady;
  else if(frameNumber > imgStatus.LastBaseImageReady)
    throw LIMA_CTL_EXC(Error, "Frame not available yet");
  aLock.unlock();
  m_ct_buffer->getFrame(aReturnData,frameNumber);
  
  FrameDim img_dim;
  m_ct_image->getImageDim(img_dim);
  aReturnData.width = img_dim.getSize().getWidth();
  aReturnData.height = img_dim.getSize().getHeight();

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

  DEB_TRACE() << "Reseting display";
  m_ct_sps_image->reset();
  resetStatus(false);
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

      int internal_stage;
      m_op_int->addTo(*mgr, internal_stage);
  
      int last_link,last_sink;
      m_op_ext->addTo(*mgr, internal_stage, last_link, last_sink);

      if (internal_stage || last_link || last_sink)
	PoolThreadMgr::get().addProcess(mgr);
      else
	delete mgr;
      if (!internal_stage && !last_link)
	newBaseImageReady(fdata);

      if (m_img_status_cb)
	m_img_status_cb->imageStatusChanged(m_status.ImageCounters);
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
  
  if(m_autosave && !m_op_ext_link_task_active)
    {
      aLock.unlock();
      newFrameToSave(aData);
      aLock.lock();
    }

  if(m_display_active_flag)
    {
      aLock.unlock();
      m_ct_sps_image->frameReady(aData);
    }
  else
    aLock.unlock();

  if (img_status_changed && m_img_status_cb)
    m_img_status_cb->imageStatusChanged(m_status.ImageCounters);

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

  if(m_autosave)
    {
      aLock.unlock();
      newFrameToSave(aData);
    }
  else
    aLock.unlock();

  if (m_img_status_cb && img_status_changed)
    m_img_status_cb->imageStatusChanged(m_status.ImageCounters);

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
bool CtControl::_checkOverrun(Data &aData) const
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
  m_ct_saving->getSavingMode(mode) ;

  bool overrunFlag = false;
  if(imageToProcess >= nb_buffers) // Process overrun
    {
      overrunFlag = true;

      m_status.AcquisitionStatus = AcqFault;
      m_status.Error = ProcessingOverun;
      
      DEB_ERROR() << DEB_VAR1(m_status);
    }
  else if(mode != CtSaving::Manual && imageToSave >= nb_buffers) // Save overrun
    {
      overrunFlag = true;
      
      m_status.AcquisitionStatus = AcqFault;
      m_status.Error = SaveOverun;

      DEB_ERROR() << DEB_VAR1(m_status);
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
