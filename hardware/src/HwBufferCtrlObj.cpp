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
#include "lima/HwBufferCtrlObj.h"

using namespace lima;

HwBufferCtrlObj::HwBufferCtrlObj()
{
	DEB_CONSTRUCTOR();
}

HwBufferCtrlObj::~HwBufferCtrlObj()
{
	DEB_DESTRUCTOR();
}

HwBufferCtrlObj::Callback *HwBufferCtrlObj::getBufferCallback()
{
	return NULL;
}

HwBufferCtrlObj::Callback::~Callback()
{
}

void HwBufferCtrlObj::setAllocParameters(const AllocParameters& alloc_params)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(alloc_params);
}

void HwBufferCtrlObj::getAllocParameters(AllocParameters& alloc_params)
{
	DEB_MEMBER_FUNCT();
	alloc_params = AllocParameters();
	DEB_RETURN() << DEB_VAR1(alloc_params);
}

void HwBufferCtrlObj::prepareAlloc(int nb_buffers)
{
	DEB_MEMBER_FUNCT();
}
