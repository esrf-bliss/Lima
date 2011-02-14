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

#define LIMA_HWCAP_TO_SIP_CTRLOBJ_HEAD(cppHwCap)			\
  switch(cppHwCap->getType())						\
  {

#define LIMA_HWCAP_TO_SIP_CTRLOBJ_CASE(cppHwCap, aCap, cppClass, sipClass) \
  case HwCap::aCap:							\
  {									\
    cppClass *aCtrlObjPtr = NULL;					\
    if (cppHwCap->getCtrlObj(aCtrlObjPtr))				\
      sipRes = sipConvertFromInstance(aCtrlObjPtr, sipClass, NULL);	\
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
  LIMA_HWCAP_TO_SIP_CTRLOBJ_CASE(cppHwCap, aCap, Hw##aCap##CtrlObj,	\
				 sipClass_Hw##aCap##CtrlObj)

#define LIMA_HWCAP_TO_SIP_FRELON_CTRLOBJ_CASE(cppHwCap, aCap)		\
  LIMA_HWCAP_TO_SIP_CTRLOBJ_CASE(cppHwCap, aCap, Frelon::aCap##CtrlObj,	\
				 sipClass_Frelon_##aCap##CtrlObj)

#define LIMA_CONVERT_HWCAP_TO_SIP_CTRLOBJ(cppHwCap, macDomain)		\
{									\
  LIMA_HWCAP_TO_SIP_CTRLOBJ_HEAD(cppHwCap)				\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, DetInfo)	\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Buffer)	\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Sync)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Bin)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Roi)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Flip)		\
  LIMA_HWCAP_TO_SIP_##macDomain##_CTRLOBJ_CASE(cppHwCap, Shutter)	\
  LIMA_HWCAP_TO_SIP_CTRLOBJ_TAIL(cppHwCap)				\
}

