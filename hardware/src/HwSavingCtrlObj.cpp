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
#include "HwSavingCtrlObj.h"
#include "Exceptions.h"

using namespace lima;

HwSavingCtrlObj::HwSavingCtrlObj(int capabilities) :
  m_caps(capabilities),
  m_callback(NULL)
{
}

HwSavingCtrlObj::~HwSavingCtrlObj()
{
}

/** @brief write manualy a frame
 */
void HwSavingCtrlObj::writeFrame(int,int)
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}

/** @brief set frames' common header
 */
void HwSavingCtrlObj::setCommonHeader(const HeaderMap&)
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}

/** @brief clear common header
 */
void HwSavingCtrlObj::resetCommonHeader()
{
  DEB_MEMBER_FUNCT();
  THROW_HW_ERROR(NotSupported) << "No available for this Hardware";
}

int HwSavingCtrlObj::getCapabilities() const
{
  return m_caps;
}

void HwSavingCtrlObj::registerCallback(HwSavingCtrlObj::Callback *cbk)
{
  DEB_MEMBER_FUNCT();

  if(m_callback)
    THROW_HW_ERROR(Error) << "Callback is already registered";
  m_callback = cbk;
}

void HwSavingCtrlObj::unregisterCallback(HwSavingCtrlObj::Callback *cbk)
{
  DEB_MEMBER_FUNCT();

  if(m_callback != cbk)
    THROW_HW_ERROR(Error) << "Try the unregister wrong callback object";
  m_callback = NULL;
}

bool HwSavingCtrlObj::newFrameWrite(int frame_id)
{
  bool continueFalg = false;
  if(m_callback)
    continueFalg = m_callback->newFrameWrite(frame_id);
  return continueFalg;
}
