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
#ifndef HWCAP_H
#define HWCAP_H

#include "LimaCompatibility.h"
#include "Constants.h"

namespace lima
{
class HwDetInfoCtrlObj;
class HwBufferCtrlObj;
class HwSyncCtrlObj;
class HwBinCtrlObj;
class HwRoiCtrlObj;
class HwFlipCtrlObj;
class HwShutterCtrlObj;
class HwVideoCtrlObj;
class HwEventCtrlObj;
class HwSavingCtrlObj;
class HwConfigCtrlObj;
class HwReconstructionCtrlObj;

class LIMACORE_API HwCap
{
public:
	enum Type {
	        Undef,
		DetInfo, Buffer, Sync, 	// Standard: these are mandatory
		Roi, Bin, Flip,		// Image operations
		Kinetics, FrameTransfer,// Acquisition modes
		Timing,			// Read only detector timmings
		Shutter,		// Shutter control
		SerialLine, 		// Generic detector serial line
		Video,			// Video capable usualy color camera 
		Event,			// Asynch. event (error) generator
		Saving,			// Saving capable
		Config,			// Config management capable
		Reconstruction,		// Image reconstruction capable
	};
	HwCap() : m_type(Undef),m_ctrl_obj(NULL) {}
	
	template <class CtrlObj>
	HwCap(CtrlObj *ctrl_obj) : m_ctrl_obj(ctrl_obj)
	{ m_type = getTypeFromCtrlObj(ctrl_obj); }

	HwCap(const HwCap &cap) : m_type(cap.m_type),m_ctrl_obj(cap.m_ctrl_obj) {}

	Type getType() const
	{ return m_type; }

	static Type getTypeFromCtrlObj(HwDetInfoCtrlObj*);
	static Type getTypeFromCtrlObj(HwBufferCtrlObj*);
	static Type getTypeFromCtrlObj(HwSyncCtrlObj*);
	static Type getTypeFromCtrlObj(HwBinCtrlObj*);
	static Type getTypeFromCtrlObj(HwRoiCtrlObj*);
	static Type getTypeFromCtrlObj(HwFlipCtrlObj*);
	static Type getTypeFromCtrlObj(HwShutterCtrlObj*);
	static Type getTypeFromCtrlObj(HwVideoCtrlObj*);
	static Type getTypeFromCtrlObj(HwEventCtrlObj*);
	static Type getTypeFromCtrlObj(HwSavingCtrlObj*);
	static Type getTypeFromCtrlObj(HwConfigCtrlObj*);
	static Type getTypeFromCtrlObj(HwReconstructionCtrlObj*);

	template <class CtrlObj>
	bool getCtrlObj(CtrlObj *& ctrl_obj) const
	{ 
		bool ok = (m_type == getTypeFromCtrlObj(ctrl_obj));
		ctrl_obj = ok ? (CtrlObj *) m_ctrl_obj : NULL;
		return ok;
	}
 private:
	Type m_type;
	void *m_ctrl_obj;
};
 
} // namespace lima

#include "HwDetInfoCtrlObj.h"
#include "HwBufferCtrlObj.h"
#include "HwSyncCtrlObj.h"
#include "HwBinCtrlObj.h"
#include "HwRoiCtrlObj.h"
#include "HwFlipCtrlObj.h"
#include "HwShutterCtrlObj.h"
#include "HwVideoCtrlObj.h"
#include "HwEventCtrlObj.h"
#include "HwSavingCtrlObj.h"

#endif // HWCAP_H
