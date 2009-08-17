#include <string>
#include <sstream>

#include "CtControl.h"
#include "CtSaving.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtBuffer.h"
#include "CtDebug.h"

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
  m_autosave(false)
{
  m_ct_debug= new CtDebug("CtControl");
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
  delete m_ct_saving;
  delete m_ct_acq;
  delete m_ct_debug;
  delete m_ct_image;
  delete m_ct_buffer;
  delete m_op_int;
  delete m_op_ext;
}

void CtControl::getDebug(short& level) const
{
  m_ct_debug->getLevel(level);
}

void CtControl::setDebug(short level)
{
  m_ct_debug->setLevel(level);
}

void CtControl::setApplyPolicy(ApplyPolicy policy)
{
  m_policy= policy;
}

void CtControl::getApplyPolicy(ApplyPolicy &policy) const
{
  policy= m_policy;
}

void CtControl::prepareAcq()
{
  m_img_status.reset();
  m_ct_debug->trace("prepareAcq", "Apply Acquisition Parameters");
  m_ct_acq->apply(m_policy);
  m_ct_debug->trace("prepareAcq", "Apply hardware bin/roi");
  m_ct_image->applyHard();
  m_ct_debug->trace("prepareAcq", "Setup Acquisition Buffers");
  m_ct_buffer->setup(this);
  m_ct_debug->trace("prepareAcq", "Prepare Hardware for Acquisition");
  m_hw->prepareAcq();

  m_ct_debug->trace("prepareAcq", "Apply software bin/roi");
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
	m_ct_debug->trace("prepareAcq", "No auto save activated");
  m_ready= true;
}

void CtControl::startAcq()
{
  if (!m_ready)
	throw LIMA_CTL_EXC(Error, "Run prepareAcq before starting acquisition");
  m_ready = false;
  m_hw->startAcq();
  m_ct_debug->trace("startAcq", "Hardware Acquisition started");
}
 
void CtControl::stopAcq()
{
  m_hw->stopAcq();
  m_ct_debug->trace("stopAcq", "Hardware Acquisition Stopped");
}

void CtControl::getAcqStatus(HwInterface::AcqStatus& status) const
{
  HwInterface::StatusType hw_status;

  m_hw->getStatus(hw_status);
  status= hw_status.acq;
}

void CtControl::getImageStatus(ImageStatus& status) const
{
  AutoMutex aLock(m_cond.mutex());
  status= m_img_status;
}

void CtControl::ReadImage(Data &aReturnData,long frameNumber)
{
  ReadBaseImage(aReturnData,frameNumber); // todo change when external op activated
}

void CtControl::ReadBaseImage(Data &aReturnData,long frameNumber)
{
  AutoMutex aLock(m_cond.mutex());
  if(frameNumber < 0)
    frameNumber = m_img_status.LastBaseImageReady;
  else if(frameNumber > m_img_status.LastBaseImageReady)
    throw LIMA_CTL_EXC(Error, "Frame not available yet");
  aLock.unlock();
  m_ct_buffer->getFrame(aReturnData,frameNumber);
}

void CtControl::reset()
{
}

void CtControl::newFrameReady(Data& fdata)
{
  std::stringstream str;
  str << "Frame acq.nb " << fdata.frameNumber << " received";
  m_ct_debug->trace("newFrameReady", str.str());

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
}

void CtControl::newBaseImageReady(Data &aData)
{
  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_img_status.LastBaseImageReady + 1;
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
    }
  else
    m_base_images_ready.insert(aData);
  
  if(m_autosave && !m_op_ext_link_task_active)
    {
      aLock.unlock();
      newFrameToSave(aData);
    }
}

void CtControl::newImageReady(Data &aData)
{
  AutoMutex aLock(m_cond.mutex());
  long expectedImageReady = m_img_status.LastImageReady + 1;
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
    }
  else
    m_images_ready.insert(aData);

  if(m_autosave)
    {
      aLock.unlock();
      newFrameToSave(aData);
    }
}

void CtControl::newCounterReady(Data&)
{
  //@todo
}

/** @brief inc the save counter.
 *  @warning due to sequential saving, no check with image number!!!
 *  @see newImageReady
*/
void CtControl::newImageSaved(Data&)
{
  AutoMutex aLock(m_cond.mutex());
  ++m_img_status.LastImageSaved;
}

void CtControl::newFrameToSave(Data& fdata)
{
  std::stringstream str;
  str << "Add frame acq.nr " << fdata.frameNumber << " to saving";
  m_ct_debug->trace("newFrameToSave", str.str());
  m_ct_saving->frameReady(fdata);
}

// ----------------------------------------------------------------------------
// Struct ImageStatus
// ----------------------------------------------------------------------------
CtControl::ImageStatus::ImageStatus()
{
  reset();
}

void CtControl::ImageStatus::reset()
{
  LastImageAcquired	= -1;
  LastBaseImageReady	= -1;
  LastImageReady	= -1;
  LastImageSaved	= -1;
  LastCounterReady	= -1;
}
