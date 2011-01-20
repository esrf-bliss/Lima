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
#include "HwSyncCtrlObj.h"

using namespace lima;

HwSyncCtrlObj::HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl)
  : m_buffer_ctrl(buffer_ctrl),
    m_acq_mode(Single),
    m_valid_ranges_cb(NULL)
{
	DEB_CONSTRUCTOR();
}

HwSyncCtrlObj::~HwSyncCtrlObj()
{
	DEB_DESTRUCTOR();
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
}

std::ostream& lima::operator<<(std::ostream& os,const HwSyncCtrlObj::ValidRangesType &range)
{
  return os << "<" 
	    << "min_exp_time=" << range.min_exp_time << ","
	    << "max_exp_time=" << range.max_exp_time << ","
	    << "min_lat_time=" << range.min_lat_time << ","
	    << "max_lat_time=" << range.max_lat_time << ">";
}
