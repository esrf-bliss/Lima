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
#include "HwCap.h"

namespace lima
{


HwCap::Type HwCap::getTypeFromCtrlObj(HwDetInfoCtrlObj *p)
{
	return DetInfo;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwBufferCtrlObj *p)
{
	return Buffer;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwSyncCtrlObj *p)
{
	return Sync;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwBinCtrlObj *p)
{
	return Bin;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwRoiCtrlObj *p)
{
	return Roi;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwFlipCtrlObj *p)
{
	return Flip;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwShutterCtrlObj *p)
{
	return Shutter;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwVideoCtrlObj *p)
{
	return Video;
}


} // namespace lima
