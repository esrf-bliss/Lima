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
#ifndef HWBINCTRLOBJ_H
#define HWBINCTRLOBJ_H

#include "SizeUtils.h"
#include "Debug.h"

namespace lima
{

class HwBinCtrlObj
{
	DEB_CLASS(DebModHardware, "HwBinCtrlObj");

 public:
	HwBinCtrlObj();
	virtual ~HwBinCtrlObj();

	virtual void setBin(const Bin& bin) = 0;
	virtual void getBin(Bin& bin) = 0;
	virtual void checkBin(Bin& bin) = 0;

};


} // namespace lima

#endif // HWBINCTRLOBJ_H
