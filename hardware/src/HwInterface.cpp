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
#include "lima/HwInterface.h"

using namespace lima;
using namespace std;

HwInterface::HwInterface()
{
	DEB_CONSTRUCTOR();
}

HwInterface::~HwInterface()
{
	DEB_DESTRUCTOR();
}

int HwInterface::getNbAcquiredFrames()
{
	DEB_MEMBER_FUNCT();

	int nb_acq_frames =  getNbHwAcquiredFrames();

	DEB_RETURN() << DEB_VAR1(nb_acq_frames);
	return nb_acq_frames;
}

ostream& lima::operator <<(ostream& os, 
			   const HwInterface::StatusType& status)
{
	return os << "<"
		  << "acq=" << status.acq << ", "
		  << "det=" << status.det 
		  << ">";
}

ostream& lima::operator <<(ostream& os,
			   HwInterface::ResetLevel reset_level)
{
	string name = "Unknown";
	switch (reset_level) {
	case HwInterface::SoftReset:	name = "SoftReset";	break;
	case HwInterface::HardReset:	name = "HardReset";	break;
	}
	return os << name;
}



