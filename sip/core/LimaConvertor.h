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
#define SIP_CONVERT_CTRLOBJECT_TO_SIPOBJECT(cppObject) \
{\
\
switch(cppObject->getType())\
{\
case HwCap::DetInfo:\
{\
  HwDetInfoCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwDetInfoCtrlObj,NULL); \
  break;\
}\
case HwCap::Buffer:\
{\
  HwBufferCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwBufferCtrlObj,NULL); \
  break;\
}\
case HwCap::Sync:\
{\
  HwSyncCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwSyncCtrlObj,NULL); \
  break;\
}\
case HwCap::Bin:\
{\
  HwBinCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwBinCtrlObj,NULL); \
  break;\
}\
case HwCap::Roi:\
{\
  HwRoiCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwRoiCtrlObj,NULL); \
  break;\
}\
case HwCap::Flip:\
{\
  HwFlipCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwFlipCtrlObj,NULL); \
  break;\
}\
case HwCap::Shutter:\
{\
  HwShutterCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwShutterCtrlObj,NULL); \
  break;\
}\
default:\
std::cerr << "HwCap getCtrlObj : sip wrapping Type -> " << cppObject->getType() << " not yet managed" << std::endl;break;\
}\
}
