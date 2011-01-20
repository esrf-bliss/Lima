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

bool CtShutter::hasCapability() const
{
  DEB_MEMBER_FUNCT();
  return m_has_shutter;
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

