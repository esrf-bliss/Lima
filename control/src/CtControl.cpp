#include <string>
#include <sstream>

#include "CtControl.h"
#include "CtSaving.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtBuffer.h"

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
  m_autosave(false),
  m_img_status_cb(NULL)
{
  DEB_CONSTRUCTOR();

  m_ct_acq= new CtAcquisition(hw);
  m_ct_image= new CtImage(hw);
  m_ct_buffer= new CtBuffer(hw);

  //Saving
  m_ct_saving= new CtSaving(*this);
  _LastImageSavedCallback *aSaveCbkPt = new _LastImageSavedCallback(*this);
  m_ct_saving->setEndCallback(aSaveCbkPt);
  aSaveCbkPt->unref();

  m_op_int = new SoftOpInternalMgr();
  m_op_ext = new SoftOpExternalMgr();
}

CtControl::~CtControl()
{
  DEB_DESTRUCTOR();

  if (m_img_status_cb)
    unregisterImageStatusCallback(*m_img_status_cb);

  delete m_ct_saving;
  delete m_ct_acq;
  delete m_ct_image;
  delete m_ct_buffer;
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
  m_img_status.reset();
  DEB_TRACE() << "Apply Acquisition Parameters";
  
  m_ct_acq->apply(m_policy);
  DEB_TRACE() << "Apply hardware bin/roi";
  m_ct_image->applyHard();

  DEB_TRACE() << "Setup Acquisition Buffers";
  m_ct_buffer->setup(this);

  DEB_TRACE() << "Prepare Hardware for Acquisition";
  m_hw->prepareAcq();

  DEB_TRACE() << "Apply software bin/roi";
  m_op_int_active= m_ct_image->applySoft(m_op_int);
  if(m_op_int_active)
    {
      _LastBaseImageReadyCallback *aCbkPt = new _LastBaseImageReadyCallback(*this);
      m_op_int->setEndCallback(aCbkPt);
      aCbkPt->unref();
    }
  else
    m_op_int->setEndCallback(NULL);
  // set ext op
  m_op_ext_link_task_active = false; // @todo
  m_op_ext_sink_task_active = false; // @todo

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
  if (!m_autosave)
    DEB_TRACE() << "No auto save activated";
  m_ready= true;
}

void CtControl::startAcq()
{
  DEB_MEMBER_FUNCT();

  if (!m_ready)
	throw LIMA_CTL_EXC(Error, "Run prepareAcq before starting acquisition");
  m_ready = false;
  m_hw->startAcq();

  DEB_TRACE() << "Hardware Acquisition started";
}
 
void CtControl::stopAcq()
{
  DEB_MEMBER_FUNCT();

  m_hw->stopAcq();

  DEB_TRACE() << "Hardware Acquisition Stopped";
}

void CtControl::getAcqStatus(AcqStatus& status) const
{
  DEB_MEMBER_FUNCT();
  
  HwInterface::StatusType hw_status;

  m_hw->getStatus(hw_status);
  
  status= hw_status.acq;
  
  DEB_RETURN() << DEB_VAR1(status);
}

void CtControl::getImageStatus(ImageStatus& status) const
{
  DEB_MEMBER_FUNCT();
  
  AutoMutex aLock(m_cond.mutex());
  status= m_img_status;
  
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
  if(frameNumber < 0)
    frameNumber = m_img_status.LastBaseImageReady;
  else if(frameNumber > m_img_status.LastBaseImageReady)
    throw LIMA_CTL_EXC(Error, "Frame not available yet");
  aLock.unlock();
  m_ct_buffer->getFrame(aReturnData,frameNumber);
  
  DEB_RETURN() << DEB_VAR1(aReturnData);
}

void CtControl::reset()
{
  DEB_MEMBER_FUNCT();
}

void CtControl::newFrameReady(Data& fdata)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(fdata);

  DEB_TRACE() << "Frame acq.nb " << fdata.frameNumber << " received";

  AutoMutex aLock(m_cond.mutex());
  m_img_status.LastImageAcquired= fdata.frameNumber;
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
  if (m_autosave && (!internal_stage && !last_link))
    {
      newFrameToSave(fdata);
      AutoMutex aLock(m_cond.mutex());
      m_img_status.LastBaseImageReady = m_img_status.LastImageAcquired = fdata.frameNumber;
      
    }

  if (m_img_status_cb)
    m_img_status_cb->imageStatusChanged(m_img_status);
}

void CtControl::newBaseImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_img_status.LastBaseImageReady + 1;
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
      m_img_status.LastBaseImageReady = expectedImageReady;
      if(!m_op_ext_link_task_active)
	m_img_status.LastImageReady = expectedImageReady;
      
      img_status_changed = true;
    }
  else
    m_base_images_ready.insert(aData);
  
  if(m_autosave && !m_op_ext_link_task_active)
    {
      aLock.unlock();
      newFrameToSave(aData);
    }

  aLock.unlock();
  if (img_status_changed && m_img_status_cb)
    m_img_status_cb->imageStatusChanged(m_img_status);

}

void CtControl::newImageReady(Data &aData)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aData);

  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_img_status.LastImageReady + 1;
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
      m_img_status.LastImageReady = expectedImageReady;

      img_status_changed = true;
    }
  else
    m_images_ready.insert(aData);

  if(m_autosave)
    {
      aLock.unlock();
      newFrameToSave(aData);
    }

  aLock.unlock();
  if (m_img_status_cb && img_status_changed)
    m_img_status_cb->imageStatusChanged(m_img_status);

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
  ++m_img_status.LastImageSaved;
  aLock.unlock();

  if (m_img_status_cb)
    m_img_status_cb->imageStatusChanged(m_img_status);
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
