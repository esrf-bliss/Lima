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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "lima/LimaCompatibility.h"
#include <ostream>
#include <vector>
#include <list>

namespace lima
{

enum AlignDir {
	Floor, Ceil,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, AlignDir align_dir);
/**@brief ImageType is the depth of detectors images
 * - Bpp8 means 8 bits unsigned
 * - Bpp8S means 8 bits signed....
 */
enum ImageType {
	Bpp8, Bpp8S, Bpp10, Bpp10S, Bpp12, Bpp12S, Bpp14, Bpp14S, 
	Bpp16, Bpp16S, Bpp32, Bpp32S, Bpp32F, Bpp1, Bpp4, Bpp6, Bpp24, Bpp24S
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, ImageType image_type);
LIMACORE_API const char* convert_2_string(ImageType image_type);
LIMACORE_API void convert_from_string(const std::string&,ImageType&);
enum AcqMode {
	Single, Concatenation, Accumulation,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, AcqMode acq_mode);
LIMACORE_API const char* convert_2_string(AcqMode mode);
LIMACORE_API void convert_from_string(const std::string&,AcqMode&);
enum TrigMode {
	IntTrig,IntTrigMult,
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop, ExtTrigReadout,
};

typedef std::vector<TrigMode> TrigModeList;

LIMACORE_API std::ostream& operator <<(std::ostream& os, TrigMode trig_mode);
LIMACORE_API const char* convert_2_string(TrigMode trigMode);
LIMACORE_API void convert_from_string(const std::string&,TrigMode&);
enum BufferMode {
	Linear, Circular,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, BufferMode buffer_mode);

enum ShutterMode {
  ShutterManual, ShutterAutoFrame, ShutterAutoSequence
};

typedef std::vector<ShutterMode> ShutterModeList;

LIMACORE_API std::ostream& operator <<(std::ostream& os, ShutterMode shutter_mode);
LIMACORE_API const char* convert_2_string(ShutterMode);
LIMACORE_API void convert_from_string(const std::string&,ShutterMode&);
enum AcqStatus {
	AcqReady, AcqRunning, AcqFault, AcqConfig
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, AcqStatus acq_status);

enum DetStatus {
	DetIdle			= 0x00,
	DetFault		= 0x01, 
	DetWaitForTrigger	= 0x02,
	DetShutterOpen		= 0x04,
	DetExposure		= 0x08,
	DetShutterClose		= 0x10,
	DetChargeShift		= 0x20,
	DetReadout		= 0x40,
	DetLatency		= 0x80,
};

LIMACORE_API DetStatus  operator | (DetStatus  s1, DetStatus  s2);
LIMACORE_API DetStatus& operator |=(DetStatus& s1, DetStatus  s2);

LIMACORE_API std::ostream& operator <<(std::ostream& os, DetStatus det_status);

enum VideoMode {Y8,Y16,Y32,Y64,
		RGB555,RGB565,
		RGB24,RGB32,
		BGR24,BGR32,
		BAYER_RG8,BAYER_RG16,
		BAYER_BG8,BAYER_BG16,
		I420,YUV411,YUV422,YUV444,
		YUV411PACKED,YUV422PACKED,YUV444PACKED,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os,VideoMode videoMode);
LIMACORE_API const char* convert_2_string(VideoMode);
LIMACORE_API void convert_from_string(const std::string&,VideoMode&);
enum RotationMode {
  Rotation_0,
  Rotation_90,
  Rotation_180,
  Rotation_270
};

typedef std::list<RotationMode> RotationModeList;

LIMACORE_API std::ostream& operator <<(std::ostream& os,RotationMode rotationMode);
LIMACORE_API const char* convert_2_string(RotationMode rotationMode);
LIMACORE_API void convert_from_string(const std::string&,RotationMode&);
} // namespace lima

#endif // CONSTANTS_H
