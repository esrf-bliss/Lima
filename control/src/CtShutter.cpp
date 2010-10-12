#include "CtShutter.h"

using namespace lima;

#ifdef CHECK_AVAILABILITY
#undef CHECK_AVAILABILITY
#endif
#define CHECK_AVAILABILITY(func) \
  if(m_has_shutter)\
    func;	   \
  else\
    throw LIMA_CTL_EXC(Error, "No shutter capability");

CtShutter::CtShutter(HwInterface *hw)
{
  DEB_CONSTRUCTOR();

  m_has_shutter = hw->getHwCtrlObj(m_hw_shutter);
}

CtShutter::~CtShutter()
{
  DEB_DESTRUCTOR();
}

void CtShutter::getModeList(ShutterModeList& mode_list) const
{
  DEB_MEMBER_FUNCT();
  if(m_has_shutter)
    m_hw_shutter->getModeList(mode_list);
}
    
bool CtShutter::checkMode(ShutterMode shut_mode) const
{
  DEB_MEMBER_FUNCT();
  bool returnFlag = false;
  if(m_has_shutter)
    returnFlag = m_hw_shutter->checkMode(shut_mode);
  return returnFlag;
}

void CtShutter::setMode(ShutterMode  shut_mode)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->setMode(shut_mode));
}

void CtShutter::getMode(ShutterMode& shut_mode) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->getMode(shut_mode));
}

void CtShutter::setState(bool  shut_open)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->setState(shut_open));
}

void CtShutter::getState(bool& shut_open) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->getState(shut_open));
}

void CtShutter::setOpenTime (double  shut_open_time)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->setOpenTime(shut_open_time));
 }

void CtShutter::getOpenTime (double& shut_open_time) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->getOpenTime(shut_open_time));
}

void CtShutter::setCloseTime(double  shut_close_time)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->setCloseTime(shut_close_time));
}

void CtShutter::getCloseTime(double& shut_close_time) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->getCloseTime(shut_close_time));
}

