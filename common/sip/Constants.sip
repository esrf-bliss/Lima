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
enum AlignDir {
	Floor, Ceil,
};

enum ImageType {
  Bpp8, Bpp8S, Bpp10, Bpp10S, Bpp12, Bpp12S, Bpp14, Bpp14S, Bpp16, Bpp16S, Bpp32, Bpp32S,
  Bpp32F
};

enum AcqMode {
	Single, Accumulation, Concatenation,
};

enum TrigMode {
	IntTrig, IntTrigMult,
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop, ExtTrigReadout,
};

%MappedType TrigModeList
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
	aReturnFlag = sipCanConvertToEnum(PySequence_Fast_GET_ITEM(sipPy,i),
					  sipType_TrigMode);
      return aReturnFlag;
    }
  std::vector<TrigMode> *modeListPt = new std::vector<TrigMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<TrigMode>(SIPLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::vector<TrigMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_TrigMode);
      if(!tobj)
        {
	  Py_DECREF(l);
	  return NULL;
        }
      PyList_SET_ITEM(l, listPos, tobj);
    }
  return l;
%End
};

enum BufferMode {
	Linear, Circular,
};

enum ShutterMode {
  ShutterManual, ShutterAutoFrame, ShutterAutoSequence
};

%MappedType ShutterModeList
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
	aReturnFlag = sipCanConvertToEnum(PySequence_Fast_GET_ITEM(sipPy,i),
					  sipType_ShutterMode);
      return aReturnFlag;
    }
  std::vector<ShutterMode> *modeListPt = new std::vector<ShutterMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<ShutterMode>(SIPLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::vector<ShutterMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_ShutterMode);
      if(!tobj)
        {
	  Py_DECREF(l);
	  return NULL;
        }
      PyList_SET_ITEM(l, listPos, tobj);
    }
  return l;
%End
};

enum AcqStatus {
	AcqReady, AcqRunning, AcqFault, AcqConfig
};

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

enum VideoMode {Y8,Y16,Y32,Y64,
		RGB555,RGB565,
		RGB24,RGB32,
		BGR24,BGR32,
		BAYER_RG8,BAYER_RG16,
		BAYER_BG8,BAYER_BG16,
		I420,YUV411,YUV422,YUV444};

%MappedType std::list<VideoMode>
{
%TypeHeaderCode
#include <list>
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
        aReturnFlag = sipCanConvertToEnum(PySequence_Fast_GET_ITEM(sipPy,i),
                                          sipType_VideoMode);
      return aReturnFlag;
    }
  std::list<VideoMode> *modeListPt = new std::list<VideoMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<VideoMode>(SIPLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::list<VideoMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_VideoMode);
      if(!tobj)
        {
          Py_DECREF(l);
          return NULL;
        }
      PyList_SET_ITEM(l, listPos, tobj);
    }
  return l;
%End
};

enum RotationMode {
  Rotation_0,
  Rotation_90,
  Rotation_180,
  Rotation_270
};

%MappedType RotationModeList
{
%TypeHeaderCode
#include <list>
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
        aReturnFlag = sipCanConvertToEnum(PySequence_Fast_GET_ITEM(sipPy,i),
                                          sipType_RotationMode);
      return aReturnFlag;
    }
  std::list<RotationMode> *modeListPt = new std::list<RotationMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<RotationMode>(SIPLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::list<RotationMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_RotationMode);
      if(!tobj)
        {
          Py_DECREF(l);
          return NULL;
        }
      PyList_SET_ITEM(l, listPos, tobj);
    }
  return l;
%End
};
