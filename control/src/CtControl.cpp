#include "CtControl.h"
#include "CtSaving.h"

using namespace lima;

CtControl::CtControl(HwInterface *hw) :
  m_hw(hw),m_ct_saving(NULL)
{
}
CtControl::~CtControl()
{
  delete m_hw;
  delete m_ct_saving;
}


void CtControl::prepareAcq()
{
}

void CtControl::startAcq()
{
}
 
void CtControl::stopAcq()
{
}

void CtControl::getAcqStatus()
{
}

void CtControl::getImageStatus(ImageStatus& status) const
{
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
