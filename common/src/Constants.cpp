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
#include "Constants.h"

using namespace lima;
using namespace std;

ostream& lima::operator <<(ostream& os, AlignDir align_dir)
{
	const char *name = "Unknown";
	switch (align_dir) {
	case Floor:		name = "Floor";		break;
	case Ceil:		name = "Ceil";		break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, ImageType image_type)
{
	const char *name = "Unknown";
	switch (image_type) {
	case Bpp8:		name = "Bpp8";		break;
	case Bpp8S:		name = "Bpp8S";		break;
	case Bpp10:		name = "Bpp10";		break;
	case Bpp10S:		name = "Bpp10S";	break;
	case Bpp12:		name = "Bpp12";		break;
	case Bpp12S:		name = "Bpp12S";	break;
	case Bpp14:		name = "Bpp14";		break;
	case Bpp14S:		name = "Bpp14S";	break;
	case Bpp16:		name = "Bpp16";		break;
	case Bpp16S:		name = "Bpp16S";	break;
	case Bpp32:		name = "Bpp32";		break;
	case Bpp32S:		name = "Bpp32S";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, AcqMode acq_mode)
{
	const char *name = "Unknown";
	switch (acq_mode) {
	case Single:		name = "Single";	break;
	case Accumulation:	name = "Accumulation";	break;
	case Concatenation:	name = "Concatenation";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, TrigMode trig_mode)
{
	const char *name = "Unknown";
	switch (trig_mode) {
	case IntTrig:		name = "IntTrig";	break;
	case ExtTrigSingle:	name = "ExtTrigSingle";	break;
	case ExtTrigMult:	name = "ExtTrigMult";	break;
	case ExtGate:		name = "ExtGate";	break;
	case ExtStartStop:	name = "ExtStartStop";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, BufferMode buffer_mode)
{
	const char *name = "Unknown";
	switch (buffer_mode) {
	case Linear:		name = "Linear";	break;
	case Circular:		name = "Circular";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os,ShutterMode shutter_mode)
{
  const char *name;
  switch(shutter_mode)
    {
    case ShutterManual: 	name = "Manual";	break;
    case ShutterAutoFrame: 	name = "Auto frame";	break;
    case ShutterAutoSequence: 	name = "Auto sequence";	break;
    default: 			name = "Unknown";	break;
    }
  return os << name;
}

ostream& lima::operator <<(ostream& os, AcqStatus acq_status)
{
	string name = "Unknown";
	switch (acq_status) {
	case AcqReady:   name = "AcqReady";	break;
	case AcqRunning: name = "AcqRunning";	break;
	case AcqFault:   name = "AcqFault";	break;
	}
	return os << name;
}

void AddToken(string& str, const string& token, const string& sep)
{
	if (str.length() > 0)
		str += sep;
	str += token;
}

ostream& lima::operator <<(ostream& os, DetStatus det_status)
{
	if (det_status == DetIdle)
		return os << "Idle";

	string name, sep = "+";
	if (det_status & DetFault)
		AddToken(name, "Fault", sep);
	if (det_status & DetWaitForTrigger)
		AddToken(name, "WaitForTrigger", sep);
	if (det_status & DetShutterOpen)
		AddToken(name, "ShutterOpen", sep);
	if (det_status & DetExposure)
		AddToken(name, "Exposure", sep);
	if (det_status & DetShutterClose)
		AddToken(name, "ShutterClose", sep);
	if (det_status & DetChargeShift)
		AddToken(name, "ChargeShift", sep);
	if (det_status & DetReadout)
		AddToken(name, "Readout", sep);
	if (det_status & DetLatency)
		AddToken(name, "Latency", sep);
	return os << name;
}

DetStatus lima::operator |(DetStatus s1, DetStatus s2)
{
	return DetStatus(int(s1) | int(s2));
}

DetStatus& lima::operator |=(DetStatus& s1, DetStatus  s2)
{
	return s1 = s1 | s2;
}

