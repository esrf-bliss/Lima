//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2026
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

class HwCap
{
%TypeHeaderCode
#include <iostream>
#include "lima/HwCap.h"
#include "lima/HwDetInfoCtrlObj.h"
#include "lima/HwBufferCtrlObj.h"
#include "lima/HwSyncCtrlObj.h"
#include "lima/HwBinCtrlObj.h"

#define LIMA_HWCAP_TO_SIP_CTRLOBJ_HEAD(cppHwCap)			\
  switch(cppHwCap->getType())						\
  {

#define LIMA_HWCAP_TO_SIP_CTRLOBJ_CASE(cppHwCap, aCap, cppClass)        \
  case HwCap::aCap:							\
  {									\
    cppClass *aCtrlObjPtr = NULL;					\
    if (cppHwCap->getCtrlObj(aCtrlObjPtr))				\
      sipRes = sipConvertFromType(aCtrlObjPtr, sipFindType(#cppClass), NULL);	\
    break;								\
  }

#define LIMA_HWCAP_TO_SIP_CTRLOBJ_TAIL(cppHwCap)			\
  default:								\
    std::cerr << "HwCap getCtrlObj : sip wrapping Type -> "		\
              << cppHwCap->getType() << " not yet managed"		\
	      << std::endl;						\
    break;								\
  }

#define LIMA_HWCAP_TO_SIP_HW_CTRLOBJ_CASE(cppHwCap, aCap)		\
  LIMA_HWCAP_TO_SIP_CTRLOBJ_CASE(cppHwCap, aCap, Hw##aCap##CtrlObj)

#define LIMA_CONVERT_HWCAP_TO_SIP_CTRLOBJ(cppHwCap, macDomain)		\
do {									\
  LIMA_HWCAP_TO_SIP_CTRLOBJ_HEAD(cppHwCap)				\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, DetInfo)	\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Buffer)	\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Sync)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Bin)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Roi)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Flip)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Shutter)	\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Event)		\
  LIMA_HWCAP_TO_SIP_CTRLOBJ_TAIL(cppHwCap)				\
} while (0)

using namespace lima;
%End
public:
	enum Type {
		Undef,
		DetInfo, Buffer, Sync, 	// Standard: these are mandatory
		Roi, Bin, Flip,		// Image operations
		Kinetics, FrameTransfer,// Acquisition modes
		Timing,			// Read only detector timings
		Shutter,		// Shutter control
		SerialLine, 		// Generic detector serial line
		Video,			// Video capable usualy color camera
		Event,			// Asynch. event (error) generator
	};
	HwCap();
	HwCap(const HwCap&);
	HwCap(HwDetInfoCtrlObj*);
	HwCap(HwBufferCtrlObj*);
	HwCap(HwSyncCtrlObj*);
	HwCap(HwBinCtrlObj*);
	HwCap(HwRoiCtrlObj*);
	HwCap(HwFlipCtrlObj*);
	HwCap(HwShutterCtrlObj*);
//	HwCap(HwVideoCtrlObj*);
	HwCap(HwEventCtrlObj*);
	Type getType() const;
	SIP_PYOBJECT getCtrlObj();
%MethodCode
LIMA_CONVERT_HWCAP_TO_SIP_CTRLOBJ(sipCpp, HW);
%End
};
