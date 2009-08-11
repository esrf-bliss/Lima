#include "CtControl.h"
#include "CtSaving.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtBuffer.h"
#include "CtDebug.h"

using namespace lima;

CtControl::CtControl(HwInterface *hw) :
  m_hw(hw), m_ct_saving(NULL), m_policy(All), m_ready(false)
{
  m_ct_debug= new CtDebug("CtControl");
  m_ct_acq= new CtAcquisition(hw);
  m_ct_image= new CtImage(hw);
  m_ct_buffer= new CtBuffer(hw);
}

CtControl::~CtControl()
{
  delete m_hw;
  delete m_ct_saving;
  delete m_ct_acq;
  delete m_ct_image;
  delete m_ct_buffer;
  delete m_ct_debug;
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

void CtControl::getApplyPolicy(ApplyPolicy &policy)
{
  policy= m_policy;
}

void CtControl::prepareAcq()
{
  FrameDim hw_fdim;
  m_img_status.reset();
  m_ct_debug->trace("prepareAcq", "Apply Acquisition Parameters");
  m_ct_acq->apply(m_policy);
  m_ct_debug->trace("prepareAcq", "Setup Acquisition Buffers");
  m_ct_image->getHwImageDim(hw_fdim);
  m_ct_buffer->setup(m_ct_acq, hw_fdim);
  m_ct_debug->trace("prepareAcq", "Prepare Hardware for Acquisition");
  m_hw->prepareAcq();
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

//Struct ImageStatus
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
