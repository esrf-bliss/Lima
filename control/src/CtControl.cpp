#include <string>
#include <sstream>

#include "CtControl.h"
#include "CtSaving.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtBuffer.h"
#include "CtDebug.h"

#include "PoolThreadMgr.h"


using namespace lima;

CtControl::CtControl(HwInterface *hw) :
  m_hw(hw), m_ct_saving(NULL), m_policy(All), m_ready(false)
{
  m_ct_debug= new CtDebug("CtControl");
  m_ct_acq= new CtAcquisition(hw);
  m_ct_image= new CtImage(hw);
  m_ct_buffer= new CtBuffer(hw);
  m_ct_saving= new CtSaving(*this);
  m_op_int= new SoftOpInternalMgr();
}

CtControl::~CtControl()
{
  delete m_ct_saving;
  delete m_ct_acq;
  delete m_ct_debug;
  delete m_ct_image;
  delete m_ct_buffer;
  delete m_op_int;
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
  m_ct_debug->trace("prepareAcq", "Setup Acquisition Buffers");
  m_ct_buffer->setup(this);
  m_ct_debug->trace("prepareAcq", "Prepare Hardware for Acquisition");
  m_hw->prepareAcq();

  //m_op_int->setBin(sw_bin_roi->getBin());
  //m_op_int->setRoi(sw_bin_roi->getRoi());
 
  // set ext op

  m_autosave= m_ct_saving->hasAutoSaveMode();
  if (!m_autosave)
	m_ct_debug->trace("prepareAcq", "No auto save activated");
  m_ready= true;
}

void CtControl::startAcq()
{
  if (!m_ready)
	throw LIMA_CTL_EXC(Error, "Run prepareAcq before starting acquisition");
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
  status= m_img_status;
}

void CtControl::reset()
{
}

void CtControl::newFrameReady(Data& fdata)
{
  std::stringstream str;
  str << "Frame acq.nb " << fdata.frameNumber << " received";
  m_ct_debug->trace("newFrameReady", str.str());
  m_img_status.LastImageAcquired= fdata.frameNumber;

  TaskMgr mgr= TaskMgr();
  mgr.setInputData(fdata);

  //m_op_int->addTo(mgr, m_op_stage.internal);
  // m_ct_extop->addTo(mgr, first_stage, last_link, last_sink);

  m_op_stage.ext_link= 0;
  m_op_stage.ext_sink= 0;

  if (m_op_stage.getNb())
  	PoolThreadMgr::get().addProcess(&mgr);
  if (m_autosave && !m_op_stage.getNbLink())
	newFrameToSave(fdata);
}

void CtControl::newFrameToSave(Data& fdata)
{
  std::stringstream str;
  str << "Add frame acq.nr " << fdata.frameNumber << " to saving";
  m_ct_debug->trace("newFrameToSave", str.str());
  m_ct_saving->frameReady(fdata);
}

// ----------------------------------------------------------------------------
// Struct SoftOpStage
// ----------------------------------------------------------------------------
void CtControl::SoftOpStage::reset()
{
	internal= 0;
	ext_link= 0;
	ext_sink= 0;
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
