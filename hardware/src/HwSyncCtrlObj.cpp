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
#include <algorithm>
#include "lima/HwSyncCtrlObj.h"

using namespace lima;

HwSyncCtrlObj::ValidRangesCallback::ValidRangesCallback()
  : m_hw_sync(NULL)
{
}

HwSyncCtrlObj::ValidRangesCallback::~ValidRangesCallback()
{
  if (m_hw_sync)
    m_hw_sync->unregisterValidRangesCallback(this);
}

HwSyncCtrlObj::HwSyncCtrlObj()
  : m_access_mode(Master),
    m_acq_mode(Single),
    m_valid_ranges_cb(NULL)
{
  DEB_CONSTRUCTOR();
}

HwSyncCtrlObj::~HwSyncCtrlObj()
{
  DEB_DESTRUCTOR();
  if (m_valid_ranges_cb)
    unregisterValidRangesCallback(m_valid_ranges_cb);
}

bool HwSyncCtrlObj::checkAutoExposureMode(AutoExposureMode mode) const
{
  DEB_MEMBER_FUNCT();
  bool checkFlag = mode == OFF;
  DEB_RETURN() << DEB_VAR1(checkFlag);
  return checkFlag;
}
void HwSyncCtrlObj::setAutoExposureMode(AutoExposureMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);
  if(!checkAutoExposureMode(mode))
    THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
  setHwAutoExposureMode(mode);
  m_auto_exposure_mode = mode;
}

/** @brief this method should be redefined in the subclass if 
 *  the camera can managed auto exposure
 */
void HwSyncCtrlObj::setHwAutoExposureMode(AutoExposureMode)
{
}

void HwSyncCtrlObj::getAutoExposureMode(AutoExposureMode& mode) const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_auto_exposure_mode);
  mode = m_auto_exposure_mode;
}
void HwSyncCtrlObj::setNbFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);

	setNbHwFrames(nb_frames);
}

void HwSyncCtrlObj::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	getNbHwFrames(nb_frames);
	DEB_RETURN() << DEB_VAR1(nb_frames);
}

void HwSyncCtrlObj::registerValidRangesCallback(ValidRangesCallback *cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(cb, m_valid_ranges_cb);

  if(m_valid_ranges_cb)
    {
      DEB_ERROR() << "ValidRangesCallback already registered";
      throw LIMA_CTL_EXC(InvalidValue,"ValidRangesCallback already registered");
    }

  cb->m_hw_sync = this;
  m_valid_ranges_cb = cb;
}

void HwSyncCtrlObj::unregisterValidRangesCallback(ValidRangesCallback *cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(cb, m_valid_ranges_cb);

  if(m_valid_ranges_cb != cb)
    {
      DEB_ERROR() << "ValidRangesCallback not registered";
      throw LIMA_CTL_EXC(InvalidValue,"ValidRangesCallback not registered");
    }

  m_valid_ranges_cb = NULL;
  cb->m_hw_sync = NULL;
}

std::ostream& lima::operator<<(std::ostream& os,const HwSyncCtrlObj::ValidRangesType &range)
{
  return os << "<" 
	    << "min_exp_time=" << range.min_exp_time << ","
	    << "max_exp_time=" << range.max_exp_time << ","
	    << "min_lat_time=" << range.min_lat_time << ","
	    << "max_lat_time=" << range.max_lat_time << ">";
}

const char* lima::convert_2_string(HwSyncCtrlObj::AutoExposureMode mode)
{
  const char* aHumanReadablePt;
  switch(mode)
    {
    case HwSyncCtrlObj::OFF:		aHumanReadablePt = "OFF";	break;
    case HwSyncCtrlObj::ON:		aHumanReadablePt = "ON";	break;
    default:
      aHumanReadablePt = "UNKNOWN";
      break;
    }
  return aHumanReadablePt;
}

void lima::convert_from_string(const std::string& val,
			       HwSyncCtrlObj::AutoExposureMode& mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);

  if(buffer == "off") mode = HwSyncCtrlObj::OFF;
  else if(buffer == "on") mode = HwSyncCtrlObj::ON;
  else
    {
      std::ostringstream msg;
      msg << "AutoExposureMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }

}
std::ostream& lima::operator<<(std::ostream& os,const HwSyncCtrlObj::AutoExposureMode& mode)
{
  return os << convert_2_string(mode);
}
