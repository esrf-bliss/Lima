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

#include "Debug.h"
#include "Exceptions.h"
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

const char* lima::convert_2_string(ImageType image_type)
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

	case Bpp1:		name = "Bpp1";		break;
	case Bpp4:		name = "Bpp4";		break;
	case Bpp6:		name = "Bpp6";		break;
	case Bpp24:		name = "Bpp24";		break;
	case Bpp24S:	name = "Bpp24S";	break;
	}
	return name;
}
void lima::convert_from_string(const std::string& val,
			       ImageType& image_type)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);
  
  if(buffer == "bpp8") 		image_type = Bpp8;
  else if(buffer == "bpp8s") 	image_type = Bpp8S;
  else if(buffer == "bpp10") 	image_type = Bpp10;
  else if(buffer == "bpp10s") 	image_type = Bpp10S;
  else if(buffer == "bpp12") 	image_type = Bpp12;
  else if(buffer == "bpp12s") 	image_type = Bpp12S;
  else if(buffer == "bpp14") 	image_type = Bpp14;
  else if(buffer == "bpp14s") 	image_type = Bpp14S;
  else if(buffer == "bpp16") 	image_type = Bpp16;
  else if(buffer == "bpp16s") 	image_type = Bpp16S;
  else if(buffer == "bpp32") 	image_type = Bpp32;
  else if(buffer == "bpp32s") 	image_type = Bpp32S;

  else if(buffer == "bpp1") 	image_type = Bpp1;
  else if(buffer == "bpp4") 	image_type = Bpp4;
  else if(buffer == "bpp6") 	image_type = Bpp6;
  else if(buffer == "bpp24") 	image_type = Bpp24;
  else if(buffer == "bpp24s") 	image_type = Bpp24S;
  else
    {
      std::ostringstream msg;
      msg << "ImageType can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }
}

ostream& lima::operator <<(ostream& os, ImageType image_type)
{
	return os << convert_2_string(image_type);
}

const char* lima::convert_2_string(AcqMode acq_mode)
{
  	const char *name = "Unknown";
	switch (acq_mode) {
	case Single:		name = "Single";	break;
	case Accumulation:	name = "Accumulation";	break;
	case Concatenation:	name = "Concatenation";	break;
	}
	return name;
}
void lima::convert_from_string(const std::string& val,
			       AcqMode& mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);

  if(buffer == "single") 		mode = Single;
  else if(buffer == "accumulation") 	mode = Accumulation;
  else if(buffer == "concatenation") 	mode = Concatenation;
  else
    {
      std::ostringstream msg;
      msg << "AcqMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }

}
ostream& lima::operator <<(ostream& os, AcqMode acq_mode)
{
	return os << convert_2_string(acq_mode);
}

const char* lima::convert_2_string(TrigMode trig_mode)
{
	const char *name = "Unknown";
	switch (trig_mode) {
	case IntTrig:		name = "IntTrig";	break;
	case IntTrigMult:	name = "IntTrigMult";	break;
	case ExtTrigSingle:	name = "ExtTrigSingle";	break;
	case ExtTrigMult:	name = "ExtTrigMult";	break;
	case ExtGate:		name = "ExtGate";	break;
	case ExtStartStop:	name = "ExtStartStop";	break;
	case ExtTrigReadout:	name = "ExtTrigReadout";break;
	}
	return name;
}
void lima::convert_from_string(const std::string& val,
			       TrigMode& trig_mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);

  if(buffer == "inttrig") 		trig_mode = IntTrig;
  else if(buffer == "inttrigmult") 	trig_mode = IntTrigMult;
  else if(buffer == "exttrigsingle") 	trig_mode = ExtTrigSingle;
  else if(buffer == "exttrigmult") 	trig_mode = ExtTrigMult;
  else if(buffer == "extgate") 		trig_mode = ExtGate;
  else if(buffer == "extstartstop") 	trig_mode = ExtStartStop;
  else if(buffer == "exttrigreadout") 	trig_mode = ExtTrigReadout;
  else
    {
      std::ostringstream msg;
      msg << "TrigMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }

}
ostream& lima::operator <<(ostream& os, TrigMode trig_mode)
{
	return os << convert_2_string(trig_mode);
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

const char* lima::convert_2_string(ShutterMode shutter_mode)
{
  const char *name;
  switch(shutter_mode)
    {
    case ShutterManual: 	name = "Manual";	break;
    case ShutterAutoFrame: 	name = "Auto frame";	break;
    case ShutterAutoSequence: 	name = "Auto sequence";	break;
    default: 			name = "Unknown";	break;
    }
  return name;
}
void lima::convert_from_string(const std::string& val,
			       ShutterMode& shutter_mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);
  if(buffer == "manual") 		shutter_mode = ShutterManual;
  else if(buffer == "auto frame") 	shutter_mode = ShutterAutoFrame;
  else if(buffer == "auto sequence") 	shutter_mode = ShutterAutoSequence;
  else
    {
      std::ostringstream msg;
      msg << "TrigMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }
}
ostream& lima::operator <<(ostream& os,ShutterMode shutter_mode)
{
  return os << convert_2_string(shutter_mode);
}

ostream& lima::operator <<(ostream& os, AcqStatus acq_status)
{
	const char* name = "Unknown";
	switch (acq_status) {
	case AcqReady:   name = "AcqReady";	break;
	case AcqRunning: name = "AcqRunning";	break;
	case AcqFault:   name = "AcqFault";	break;
	case AcqConfig:   name = "AcqConfig";	break;
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
#ifdef __unix
	return os << name;
#else  // Fucking Window can't stream insertion of string ?!??!!!
	return os << name.c_str();
#endif
}

DetStatus lima::operator |(DetStatus s1, DetStatus s2)
{
	return DetStatus(int(s1) | int(s2));
}

DetStatus& lima::operator |=(DetStatus& s1, DetStatus  s2)
{
	return s1 = s1 | s2;
}

const char* lima::convert_2_string(VideoMode aVideoMode)
{
  const char *aHumanReadablePt;
  switch(aVideoMode)
    {
    case Y8: 		aHumanReadablePt = "Y8";		break;
    case Y16: 		aHumanReadablePt = "Y16";		break;
    case Y32: 		aHumanReadablePt = "Y32";		break;
    case Y64: 		aHumanReadablePt = "Y64";		break;
    case RGB555: 	aHumanReadablePt = "RGB555";		break;
    case RGB565: 	aHumanReadablePt = "RGB565";		break;
    case RGB24: 	aHumanReadablePt = "RGB24";		break;
    case RGB32: 	aHumanReadablePt = "RGB32";		break;
    case BGR24: 	aHumanReadablePt = "BGR24";		break;
    case BGR32: 	aHumanReadablePt = "BGR32";		break;
    case BAYER_RG8: 	aHumanReadablePt = "BAYER_RG8";		break;
    case BAYER_RG16: 	aHumanReadablePt = "BAYER_RG16";	break;
    case BAYER_BG8: 	aHumanReadablePt = "BAYER_BG8";		break;
    case BAYER_BG16: 	aHumanReadablePt = "BAYER_BG16";	break;
    case I420: 		aHumanReadablePt = "I420";		break;
    case YUV411: 	aHumanReadablePt = "YUV411";		break;
    case YUV422: 	aHumanReadablePt = "YUV422";		break;
    case YUV444: 	aHumanReadablePt = "YUV444";		break;
    default: 		aHumanReadablePt = "Unknown";		break;
    }
  return aHumanReadablePt;
}
void lima::convert_from_string(const std::string& val,
			       VideoMode& video_mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);

  if(buffer == "y8") 			video_mode = Y8;
  else if(buffer == "y16") 		video_mode = Y16;
  else if(buffer == "y32") 		video_mode = Y32;
  else if(buffer == "y64") 		video_mode = Y64;
  else if(buffer == "rgb555") 		video_mode = RGB555;
  else if(buffer == "rgb565") 		video_mode = RGB565;
  else if(buffer == "rgb24") 		video_mode = RGB24;
  else if(buffer == "rgb32") 		video_mode = RGB32;
  else if(buffer == "bgr24") 		video_mode = BGR24;
  else if(buffer == "bgr32") 		video_mode = BGR32;
  else if(buffer == "bayer_rg8") 	video_mode = BAYER_RG8;
  else if(buffer == "bayer_rg16") 	video_mode = BAYER_RG16;
  else if(buffer == "i420") 		video_mode = I420;
  else if(buffer == "yuv411") 		video_mode = YUV411;
  else if(buffer == "yuv422") 		video_mode = YUV422;
  else if(buffer == "yuv444") 		video_mode = YUV444;
  else
    {
      std::ostringstream msg;
      msg << "VideoMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }
}
ostream& lima::operator <<(ostream& os, VideoMode aVideoMode)
{
  return os << convert_2_string(aVideoMode);
}
const char* lima::convert_2_string(RotationMode rotationMode)
{
  const char *aHumanReadablePt;
  switch(rotationMode)
    {
    case Rotation_0: 	aHumanReadablePt = "Rotation_0";	break;
    case Rotation_90: 	aHumanReadablePt = "Rotation_90";	break;
    case Rotation_180: 	aHumanReadablePt = "Rotation_180";	break;
    case Rotation_270: 	aHumanReadablePt = "Rotation_270";	break;
    default: 		aHumanReadablePt = "Unknown";		break;
    }
  return aHumanReadablePt;
}
void lima::convert_from_string(const std::string& val,
			       RotationMode& rotationMode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);

  if(buffer == "rotation_0") 		rotationMode = Rotation_0;
  else if(buffer == "rotation_90") 	rotationMode = Rotation_90;
  else if(buffer == "rotation_180") 	rotationMode = Rotation_180;
  else if(buffer == "rotation_270") 	rotationMode = Rotation_270;
  else
    {
      std::ostringstream msg;
      msg << "RotationMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }

}
ostream& lima::operator <<(ostream& os,RotationMode rotationMode)
{
  return os << convert_2_string(rotationMode);
}
