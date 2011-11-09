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
#include "SimulatorSyncCtrlObj.h"

using namespace lima;
using namespace lima::Simulator;
using namespace std;


/*******************************************************************
 * \brief SyncCtrlObj constructor
 *******************************************************************/

SyncCtrlObj::SyncCtrlObj(Camera& simu)
	: HwSyncCtrlObj(), m_simu(simu)
{
}

SyncCtrlObj::~SyncCtrlObj()
{
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	return (trig_mode == IntTrig);
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	if (!checkTrigMode(trig_mode))
		throw LIMA_HW_EXC(InvalidValue, "Invalid (external) trigger");
}

void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	trig_mode = IntTrig;
}

void SyncCtrlObj::setExpTime(double exp_time)
{
	m_simu.setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double& exp_time)
{
	m_simu.getExpTime(exp_time);
}

void SyncCtrlObj::setLatTime(double lat_time)
{
	m_simu.setLatTime(lat_time);
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
	m_simu.getLatTime(lat_time);
}

void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	m_simu.setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	m_simu.getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	double min_time = 10e-9;
	double max_time = 1e6;
	valid_ranges.min_exp_time = min_time;
	valid_ranges.max_exp_time = max_time;
	valid_ranges.min_lat_time = min_time;
	valid_ranges.max_lat_time = max_time;
}
