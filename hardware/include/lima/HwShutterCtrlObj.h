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
#ifndef HWSHUTTERCTRLOBJ_H
#define HWSHUTTERCTRLOBJ_H

#include "lima/LimaCompatibility.h"
#include "lima/Constants.h"
#include "lima/Debug.h"
#include <vector>

namespace lima
{

class LIMACORE_API HwShutterCtrlObj
{
	DEB_CLASS(DebModHardware, "HwShutterCtrlObj");

public:
	HwShutterCtrlObj();
	virtual ~HwShutterCtrlObj();

	virtual bool checkMode(ShutterMode shut_mode) const = 0;
	virtual void getModeList(ShutterModeList&  mode_list) const = 0;
	virtual void setMode(ShutterMode  shut_mode) = 0;
	virtual void getMode(ShutterMode& shut_mode) const = 0;

	virtual void setState(bool  shut_open) = 0;
	virtual void getState(bool& shut_open) const = 0;

	virtual void setOpenTime (double  shut_open_time)  = 0;
	virtual void getOpenTime (double& shut_open_time) const = 0;
	virtual void setCloseTime(double  shut_close_time) = 0;
	virtual void getCloseTime(double& shut_close_time) const = 0;
};



} // namespace lima

#endif // HWSHUTTERCTRLOBJ_H
