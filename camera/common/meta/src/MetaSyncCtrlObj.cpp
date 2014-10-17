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
#include <sstream>
#include "MetaSyncCtrlObj.h"
#include "MetaInterface.h"

using namespace lima;
using namespace lima::Meta;

#define CHECK_ALL(fct)							\
  bool check_flag = !m_interface.m_tiles.empty();			\
									\
  for(Interface::TileCnt::iterator i = m_interface.m_tiles.begin();	\
      check_flag && i != m_interface.m_tiles.end();++i)			\
    {									\
  HwSyncCtrlObj* local_sync;						\
  if(!i->second->getHwCtrlObj(local_sync))				\
    THROW_HW_ERROR(Error) << "Cannot get hardware sync";		\
									\
  check_flag = local_sync->fct;						\
    }									\
									\
  DEB_RETURN() << DEB_VAR1(check_flag);

#define SET_ALL(fct)							\
  for(Interface::TileCnt::iterator i = m_interface.m_tiles.begin();	\
      i != m_interface.m_tiles.end();++i)				\
    {									\
  HwSyncCtrlObj* local_sync;						\
  if(!i->second->getHwCtrlObj(local_sync))				\
    THROW_HW_ERROR(Error) << "Cannot get hardware sync";		\
									\
  local_sync->fct;							\
    }									

#define GET_ONE(fct)						\
Interface::TileCnt::iterator i = m_interface.m_tiles.begin();\
 HwSyncCtrlObj* local_sync;				     \
 if(!i->second->getHwCtrlObj(local_sync))		     \
   THROW_HW_ERROR(Error) << "Cannot get hardware sync";	     \
 local_sync->fct;

SyncCtrlObj::SyncCtrlObj(Interface& i) :
  m_interface(i)
{
}

SyncCtrlObj::~SyncCtrlObj()
{
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  CHECK_ALL(checkTrigMode(trig_mode));

  return check_flag;
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  SET_ALL(setTrigMode(trig_mode));
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(trig_mode);

  GET_ONE(getTrigMode(trig_mode));
}

void SyncCtrlObj::setExpTime(double exp_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(exp_time);

  SET_ALL(setExpTime(exp_time));
}

void SyncCtrlObj::getExpTime(double &exp_time)
{
  DEB_MEMBER_FUNCT();
  
  GET_ONE(getExpTime(exp_time));

  DEB_RETURN() << DEB_VAR1(exp_time);
}

void SyncCtrlObj::setLatTime(double  lat_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(lat_time);

  SET_ALL(setLatTime(lat_time));
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
  DEB_MEMBER_FUNCT();
  
  GET_ONE(getLatTime(lat_time));

  DEB_RETURN() << DEB_VAR1(lat_time);
}

void SyncCtrlObj::setNbHwFrames(int  nb_frames)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nb_frames);
  
  SET_ALL(setNbHwFrames(nb_frames));
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
  DEB_MEMBER_FUNCT();
  
  GET_ONE(getNbHwFrames(nb_frames));

  DEB_RETURN() << DEB_VAR1(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
  DEB_MEMBER_FUNCT();

  Interface::TileCnt::iterator i = m_interface.m_tiles.begin();
  HwSyncCtrlObj* local_sync;
  if(!i->second->getHwCtrlObj(local_sync))
    THROW_HW_ERROR(Error) << "Cannot get hardware sync";
  local_sync->getValidRanges(valid_ranges);

  for(++i;i != m_interface.m_tiles.end();++i)
    {
      ValidRangesType local_ranges;
      if(!i->second->getHwCtrlObj(local_sync))
	THROW_HW_ERROR(Error) << "Cannot get hardware sync";
      local_sync->getValidRanges(local_ranges);
      
      if(local_ranges.min_exp_time > valid_ranges.min_exp_time)
	valid_ranges.min_exp_time = local_ranges.min_exp_time;
      if(local_ranges.max_exp_time < valid_ranges.max_exp_time)
	valid_ranges.max_exp_time = local_ranges.max_exp_time;

      if(local_ranges.min_lat_time > valid_ranges.min_lat_time)
	valid_ranges.min_lat_time = local_ranges.min_lat_time;
      if(local_ranges.max_lat_time < valid_ranges.max_lat_time)
	valid_ranges.max_lat_time = local_ranges.max_lat_time;
    }

  DEB_RETURN() << DEB_VAR1(valid_ranges);
}

bool SyncCtrlObj::checkAutoExposureMode(HwSyncCtrlObj::AutoExposureMode mode) const
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  CHECK_ALL(checkAutoExposureMode(mode));

  return check_flag;
}

void SyncCtrlObj::setHwAutoExposureMode(AutoExposureMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  SET_ALL(setHwAutoExposureMode(mode));
}
