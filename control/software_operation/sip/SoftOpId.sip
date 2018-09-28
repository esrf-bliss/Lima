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
%MappedType std::list<SoftOpRoiCounter::RoiNameAndResults>
{
%TypeHeaderCode
#include <list>
#include "processlib/RoiCounter.h"
using namespace lima;
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
 bool aReturnFlag = PyList_Check(sipPy);
 for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
 {
  PyObject *p = PyList_GET_ITEM(sipPy, i);
  aReturnFlag = PySequence_Check(p) && 
                PySequence_Size(p) == 2 &&
    	          sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                PyList_Check(PySequence_Fast_GET_ITEM(p,1));
  if(aReturnFlag)
    {
      PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
      for(int k = 0;aReturnFlag && k < PyList_Size(aTmpList);++k)
	aReturnFlag = !!sipCanConvertToType(PyList_GET_ITEM(aTmpList, k),
					  sipFindType("Tasks::RoiCounterResult"), SIP_NOT_NONE);
    }
 }
 return aReturnFlag;
}

std::list<SoftOpRoiCounter::RoiNameAndResults> *roiNameNResultPt =
	new std::list<SoftOpRoiCounter::RoiNameAndResults>();

for(int i = 0;i < PyList_Size(sipPy);++i)
{
 PyObject *p = PyList_GET_ITEM(sipPy, i);
 PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
 std::string name = sipConvertFromPyStr(aTmpName);
 PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
 
 roiNameNResultPt->push_back(SoftOpRoiCounter::RoiNameAndResults(name,
								 std::list<Tasks::RoiCounterResult>()));
 for(int k = 0;k < PyList_Size(aTmpList);++k)
   {
     int state;
     Tasks::RoiCounterResult *aRoiCounterResult = 
       reinterpret_cast<Tasks::RoiCounterResult*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								   sipFindType("Tasks::RoiCounterResult"), 0,	
								   SIP_NOT_NONE,
								   &state, sipIsErr));
     // Deal with any errors.
     if (*sipIsErr)
       {
	 sipReleaseType(aRoiCounterResult, sipFindType("Tasks::RoiCounterResult"), state);
	 
	 // Tidy up.
	 delete roiNameNResultPt;

	 // There is no temporary instance.
	 return 0;
       }
     
     roiNameNResultPt->back().second.push_back(*aRoiCounterResult);
     sipReleaseType(aRoiCounterResult, sipFindType("Tasks::RoiCounterResult"), state);
   }
}
*sipCppPtr = roiNameNResultPt;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
 PyObject *l;

 // Create the Python list of the correct length.
 if (!(l = PyList_New(sipCpp->size())))
  return NULL;

 sipTransferObj = NULL;
 int i = 0;
 for (std::list<SoftOpRoiCounter::RoiNameAndResults>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
  {
    int m = 0;
    std::list<Tasks::RoiCounterResult> &aRoiCounterResultList = j->second;
    PyObject *PyRoiResultList = PyList_New(aRoiCounterResultList.size());
    for(std::list<Tasks::RoiCounterResult>::iterator k = aRoiCounterResultList.begin();
        k != aRoiCounterResultList.end();++k,++m)
    {
     PyObject *wobj;

     // Get the Python wrapper for the RoiCounterResult instance, creating a new
     // one if necessary, and handle any ownership transfer.
     Tasks::RoiCounterResult *aResultPt = new Tasks::RoiCounterResult(*k);
     if (!(wobj = sipConvertFromNewType(aResultPt, sipFindType("Tasks::RoiCounterResult"), sipTransferObj)))
     {
      delete aResultPt;
      // There was an error so garbage collect the Python lists.
      Py_DECREF(PyRoiResultList);
      Py_DECREF(l);
      return NULL;
     }
     PyList_SET_ITEM(PyRoiResultList,m,wobj);
    }

   PyObject *aNamePy = sipConvertToPyStr(j->first);
   PyList_SET_ITEM(l, i, PyTuple_Pack(2,aNamePy,PyRoiResultList));
   Py_DECREF(aNamePy);Py_DECREF(PyRoiResultList);
  }

 // Return the Python list.
 return l;
%End
};

%MappedType std::list<SoftOpRoiCounter::RoiNameAndRoi>
{
%TypeHeaderCode
#include <list>
#include "processlib/RoiCounter.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      sipCanConvertToType(PySequence_Fast_GET_ITEM(p,1),
					  sipType_Roi,SIP_NOT_NONE);
      }
    return aReturnFlag;
  }
  std::list<SoftOpRoiCounter::RoiNameAndRoi> *named_roisPt =
    new std::list<SoftOpRoiCounter::RoiNameAndRoi>();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int state;
      lima::Roi *roi =
	reinterpret_cast<lima::Roi*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
						      sipType_Roi,0,
						      SIP_NOT_NONE,
						      &state,sipIsErr));
      if(*sipIsErr)
	{
	  sipReleaseType(roi,sipType_Roi,state);
	  delete named_roisPt;
	  return 0;
	}
      named_roisPt->push_back(SoftOpRoiCounter::RoiNameAndRoi(name,*roi));
      sipReleaseType(roi,sipType_Roi,state);
    }
  *sipCppPtr = named_roisPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(std::list<SoftOpRoiCounter::RoiNameAndRoi>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *wobj;
      lima::Roi *roi = new lima::Roi(j->second);
      if(!(wobj = sipConvertFromNewType(roi,sipType_Roi,sipTransferObj)))
	{
	  delete roi;
	  Py_DECREF(l);
	  return NULL;
	}
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoiCounter::RoiNameAndArcRoi>
{
%TypeHeaderCode
#include <list>
#include "processlib/RoiCounter.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      sipCanConvertToType(PySequence_Fast_GET_ITEM(p,1),
					  sipType_ArcRoi,SIP_NOT_NONE);
      }
    return aReturnFlag;
  }
  std::list<SoftOpRoiCounter::RoiNameAndArcRoi> *named_roisPt = 
    new std::list<SoftOpRoiCounter::RoiNameAndArcRoi>();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int state;
      lima::ArcRoi *roi = 
	reinterpret_cast<lima::ArcRoi*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
						      sipType_ArcRoi,0,
						      SIP_NOT_NONE,
						      &state,sipIsErr));
      if(*sipIsErr)
	{
	  sipReleaseType(roi,sipType_ArcRoi,state);
	  delete named_roisPt;
	  return 0;
	}
      named_roisPt->push_back(SoftOpRoiCounter::RoiNameAndArcRoi(name,*roi));
      sipReleaseType(roi,sipType_ArcRoi,state);
    }
  *sipCppPtr = named_roisPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(std::list<SoftOpRoiCounter::RoiNameAndArcRoi>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *wobj;
      lima::ArcRoi *roi = new lima::ArcRoi(j->second);
      if(!(wobj = sipConvertFromNewType(roi,sipType_ArcRoi,sipTransferObj)))
	{
	  delete roi;
	  Py_DECREF(l);
	  return NULL;
	}
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoiCounter::RoiNameAndType>
{
%TypeHeaderCode
#include <list>
#include "processlib/RoiCounter.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      PyInt_Check(PySequence_Fast_GET_ITEM(p,1));
      }
    return aReturnFlag;
  }
  std::list<SoftOpRoiCounter::RoiNameAndType> *names_typesPt = new std::list<SoftOpRoiCounter::RoiNameAndType>();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int roi_type = (int) PyInt_AsLong(PySequence_Fast_GET_ITEM(p,1));
      names_typesPt->push_back(SoftOpRoiCounter::RoiNameAndType(name,roi_type));
    }
  *sipCppPtr = names_typesPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(std::list<SoftOpRoiCounter::RoiNameAndType>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyObject *aTypePy = PyInt_FromLong(j->second);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,aTypePy));
      Py_DECREF(aNamePy);Py_DECREF(aTypePy);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoiCounter::RoiNameAndTask>
{
%TypeHeaderCode
#include <list>
#include "processlib/RoiCounter.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      sipCanConvertToType(PySequence_Fast_GET_ITEM(p,1),
					  sipFindType("Tasks::RoiCounterTask"),SIP_NOT_NONE);
      }
    return aReturnFlag;
  }
  SoftOpRoiCounter::RoiNameAndTaskList *named_tasksPt = new SoftOpRoiCounter::RoiNameAndTaskList();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int state;
      Tasks::RoiCounterTask *task =
	reinterpret_cast<Tasks::RoiCounterTask*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								  sipFindType("Tasks::RoiCounterTask"),0,
								  SIP_NOT_NONE,
								  &state,sipIsErr));
      if(*sipIsErr)
	{
	  sipReleaseType(task,sipFindType("Tasks::RoiCounterTask"),state);
	  delete named_tasksPt;
	  return 0;
	}
      named_tasksPt->push_back(SoftOpRoiCounter::RoiNameAndTask(name,task));
      sipReleaseType(task,sipFindType("Tasks::RoiCounterTask"),state);
    }
  *sipCppPtr = named_tasksPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(SoftOpRoiCounter::RoiNameAndTaskList::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *wobj;
      Tasks::RoiCounterTask *task = new Tasks::RoiCounterTask(*(j->second));
      if(!(wobj = sipConvertFromNewType(task,sipFindType("Tasks::RoiCounterTask"),sipTransferObj)))
	{
	  delete task;
	  Py_DECREF(l);
	  return NULL;
	}
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoi2Spectrum::RoiNameAndResults>
{
%TypeHeaderCode
#include <list>
#include "processlib/Roi2Spectrum.h"
using namespace lima;
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
 bool aReturnFlag = PyList_Check(sipPy);
 for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
 {
  PyObject *p = PyList_GET_ITEM(sipPy, i);
  aReturnFlag = PySequence_Check(p) && 
                PySequence_Size(p) == 2 &&
                sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                PyList_Check(PySequence_Fast_GET_ITEM(p,1));
  if(aReturnFlag)
    {
      PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
      for(int k = 0;aReturnFlag && k < PyList_Size(aTmpList);++k)
	aReturnFlag = !!sipCanConvertToType(PyList_GET_ITEM(aTmpList, k),
					  sipFindType("Tasks::Roi2SpectrumResult"), SIP_NOT_NONE);
    }
 }
 return aReturnFlag;
}

std::list<SoftOpRoi2Spectrum::RoiNameAndResults> *roiNameNResultPt =
	new std::list<SoftOpRoi2Spectrum::RoiNameAndResults>();

for(int i = 0;i < PyList_Size(sipPy);++i)
{
 PyObject *p = PyList_GET_ITEM(sipPy, i);
 PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
 std::string name = sipConvertFromPyStr(aTmpName);
 PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
 
 roiNameNResultPt->push_back(SoftOpRoi2Spectrum::RoiNameAndResults(name,
								 std::list<Tasks::Roi2SpectrumResult>()));
 for(int k = 0;k < PyList_Size(aTmpList);++k)
   {
     int state;
     Tasks::Roi2SpectrumResult *aRoi2SpectrumResult = 
       reinterpret_cast<Tasks::Roi2SpectrumResult*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								   sipFindType("Tasks::Roi2SpectrumResult"), 0,	
								   SIP_NOT_NONE,
								   &state, sipIsErr));
     // Deal with any errors.
     if (*sipIsErr)
       {
	 sipReleaseType(aRoi2SpectrumResult, sipFindType("Tasks::Roi2SpectrumResult"), state);
	 
	 // Tidy up.
	 delete roiNameNResultPt;

	 // There is no temporary instance.
	 return 0;
       }
     
     roiNameNResultPt->back().second.push_back(*aRoi2SpectrumResult);
     sipReleaseType(aRoi2SpectrumResult, sipFindType("Tasks::Roi2SpectrumResult"), state);
   }
}
*sipCppPtr = roiNameNResultPt;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
 PyObject *l;

 // Create the Python list of the correct length.
 if (!(l = PyList_New(sipCpp->size())))
  return NULL;

 sipTransferObj = NULL;
 int i = 0;
 for (std::list<SoftOpRoi2Spectrum::RoiNameAndResults>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
  {
    int m = 0;
    std::list<Tasks::Roi2SpectrumResult> &aRoi2SpectrumResultList = (*j).second;
    PyObject *PyRoiResultList = PyList_New(aRoi2SpectrumResultList.size());
    for(std::list<Tasks::Roi2SpectrumResult>::iterator k = aRoi2SpectrumResultList.begin();
        k != aRoi2SpectrumResultList.end();++k,++m)
    {
     PyObject *wobj;

     // Get the Python wrapper for the Roi2SpectrumResult instance, creating a new
     // one if necessary, and handle any ownership transfer.
     Tasks::Roi2SpectrumResult *aResultPt = new Tasks::Roi2SpectrumResult(*k);
     if (!(wobj = sipConvertFromNewType(aResultPt, sipFindType("Tasks::Roi2SpectrumResult"), sipTransferObj)))
     {
      delete aResultPt;
      // There was an error so garbage collect the Python lists.
      Py_DECREF(PyRoiResultList);
      Py_DECREF(l);
      return NULL;
     }
     PyList_SET_ITEM(PyRoiResultList,m,wobj);
    }

    PyObject *aNamePy = sipConvertToPyStr(j->first);
   PyList_SET_ITEM(l, i, PyTuple_Pack(2,aNamePy,PyRoiResultList));
   Py_DECREF(aNamePy);Py_DECREF(PyRoiResultList);
  }

 // Return the Python list.
 return l;
%End
};

%MappedType std::list<SoftOpRoi2Spectrum::RoiNameAndRoi>
{
%TypeHeaderCode
#include <list>
#include "processlib/Roi2Spectrum.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      sipCanConvertToType(PySequence_Fast_GET_ITEM(p,1),
					  sipType_Roi,SIP_NOT_NONE);
      }
    return aReturnFlag;
  }
  std::list<SoftOpRoi2Spectrum::RoiNameAndRoi> *named_roisPt =
    new std::list<SoftOpRoi2Spectrum::RoiNameAndRoi>();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int state;
      lima::Roi *roi =
	reinterpret_cast<lima::Roi*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
						      sipType_Roi,0,
						      SIP_NOT_NONE,
						      &state,sipIsErr));
      if(*sipIsErr)
	{
	  sipReleaseType(roi,sipType_Roi,state);
	  delete named_roisPt;
	  return 0;
	}
      named_roisPt->push_back(SoftOpRoi2Spectrum::RoiNameAndRoi(name,*roi));
      sipReleaseType(roi,sipType_Roi,state);
    }
  *sipCppPtr = named_roisPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(std::list<SoftOpRoi2Spectrum::RoiNameAndRoi>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *wobj;
      lima::Roi *roi = new lima::Roi(j->second);
      if(!(wobj = sipConvertFromNewType(roi,sipType_Roi,sipTransferObj)))
	{
	  delete roi;
	  Py_DECREF(l);
	  return NULL;
	}
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoi2Spectrum::RoiNameAndTask>
{
%TypeHeaderCode
#include <list>
#include "processlib/Roi2Spectrum.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      sipCanConvertToType(PySequence_Fast_GET_ITEM(p,1),sipFindType("Tasks::Roi2SpectrumTask"),SIP_NOT_NONE);
      }
    return aReturnFlag;
  }
  SoftOpRoi2Spectrum::RoiNameAndTaskList *named_tasksPt = new SoftOpRoi2Spectrum::RoiNameAndTaskList();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int state;
      Tasks::Roi2SpectrumTask *task =
	reinterpret_cast<Tasks::Roi2SpectrumTask*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								  sipFindType("Tasks::Roi2SpectrumTask"),0,
								  SIP_NOT_NONE,
								  &state,sipIsErr));
      if(*sipIsErr)
	{
	  sipReleaseType(task,sipFindType("Tasks::Roi2SpectrumTask"),state);
	  delete named_tasksPt;
	  return 0;
	}
      named_tasksPt->push_back(SoftOpRoi2Spectrum::RoiNameAndTask(name,task));
      sipReleaseType(task,sipFindType("Tasks::Roi2SpectrumTask"),state);
    }
  *sipCppPtr = named_tasksPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(SoftOpRoi2Spectrum::RoiNameAndTaskList::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *wobj;
      Tasks::Roi2SpectrumTask *task = new Tasks::Roi2SpectrumTask(*(j->second));
      if(!(wobj = sipConvertFromNewType(task,sipFindType("Tasks::Roi2SpectrumTask"),sipTransferObj)))
	{
	  delete task;
	  Py_DECREF(l);
	  return NULL;
	}
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoi2Spectrum::RoiNameAndMode>
{
%TypeHeaderCode
#include <list>
#include "processlib/Roi2Spectrum.h"
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
  {
    bool aReturnFlag = PyList_Check(sipPy);
    for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
      {
	PyObject *p = PyList_GET_ITEM(sipPy,i);
	aReturnFlag = PySequence_Check(p) &&
                      PySequence_Size(p) == 2 &&
                      sipCheckConvertFromPyStr(PySequence_Fast_GET_ITEM(p,0)) &&
                      PyInt_Check(PySequence_Fast_GET_ITEM(p,1));
      }
    return aReturnFlag;
  }
  std::list<SoftOpRoi2Spectrum::RoiNameAndMode> *names_modesPt = new std::list<SoftOpRoi2Spectrum::RoiNameAndMode>();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      PyObject *aTmpName = PySequence_Fast_GET_ITEM(p,0);
      std::string name = sipConvertFromPyStr(aTmpName);
      int roi_mode = (int) PyInt_AsLong(PySequence_Fast_GET_ITEM(p,1));
      names_modesPt->push_back(SoftOpRoi2Spectrum::RoiNameAndMode(name,roi_mode));
    }
  *sipCppPtr = names_modesPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
   PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  sipTransferObj = NULL;
  int i = 0;
  for(std::list<SoftOpRoi2Spectrum::RoiNameAndMode>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *aNamePy = sipConvertToPyStr(j->first);
      PyObject *aModePy = PyInt_FromLong(j->second);
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,aModePy));
      Py_DECREF(aNamePy);Py_DECREF(aModePy);
    }
  return l;
%End
};

enum SoftOpId
{
 UNDEF,
 BACKGROUNDSUBSTRACTION,
 BINNING,
 BPM,
 FLATFIELDCORRECTION,
 FLIP,
 MASK,
 ROICOUNTERS,
 ROI2SPECTRUM,
 SOFTROI,
 USER_LINK_TASK,
 USER_SINK_TASK,
 PEAKFINDER,
};

struct SoftOpKey
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
%End
 SoftOpKey();
 SoftOpKey(SoftOpId id,const char *name);
 
 SoftOpId m_id;
 const char *m_name;
};

class SoftOpBackgroundSubstraction
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
 public:
  SoftOpBackgroundSubstraction();
  ~SoftOpBackgroundSubstraction();
  void setOffset(int value);
  void getOffset(int& value /Out/) const;

  void setBackgroundImage(Data &aData);
};



%MappedType std::list<Tasks::PeakFinderResult>
{
%TypeHeaderCode
#include <list>
#include "processlib/PeakFinder.h"
%End

%ConvertFromTypeCode
   PyObject *l;

   // Create the Python list of the correct length.
   if ((l = PyList_New(sipCpp -> size())) == NULL)
       return NULL;

   // Go through each element in the C++ instance and convert it to a
   // wrapped TYPE.
   
   int i=0;
   for (std::list<Tasks::PeakFinderResult>::iterator iter = sipCpp->begin(); iter != sipCpp->end(); iter++)
   {
       Tasks::PeakFinderResult *cpp = new Tasks::PeakFinderResult(*iter);
       PyObject *pobj;

       // Get the Python wrapper for the Type instance, creating a new
       if ((pobj = sipConvertFromNewType(cpp, sipType_Tasks_PeakFinderResult, sipTransferObj)) == NULL)
       // one if necessary, and handle any ownership transfer.
       {
           // There was an error so garbage collect the Python list.
           Py_DECREF(l);
           return NULL;
       }

       // Add the wrapper to the list.
       PyList_SET_ITEM(l, i++, pobj);
   }

   // Return the Python list.
   return l;
%End

%ConvertToTypeCode
   // Check if type is compatible
   if (sipIsErr == NULL)
   {
       // Must be any iterable
       PyObject *i = PyObject_GetIter(sipPy);
       bool iterable = (i != NULL);
       Py_XDECREF(i);
       return iterable;
   }

   // Iterate over the object
   PyObject *iterator = PyObject_GetIter(sipPy);
   PyObject *item;

   std::list<Tasks::PeakFinderResult> *l = new std::list<Tasks::PeakFinderResult>;

   while ((item = PyIter_Next(iterator)))
   {
       if (!sipCanConvertToType(item, sipType_Tasks_PeakFinderResult, SIP_NOT_NONE))
       {
           PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to Tasks::PeakFinderResult");
           *sipIsErr = 1;
           break;
       }

       int state;
       Tasks::PeakFinderResult* p = reinterpret_cast<Tasks::PeakFinderResult*>(
            sipConvertToType(item, sipType_Tasks_PeakFinderResult, 0, SIP_NOT_NONE, &state, sipIsErr));

       if (!*sipIsErr)
           l->push_back(*p);

       sipReleaseType(p, sipType_Tasks_PeakFinderResult, state);
       Py_DECREF(item);
   }

   Py_DECREF(iterator);

   if (*sipIsErr)
   {
       delete l;
       return 0;
   }

   *sipCppPtr = l;
   return sipGetState(sipTransferObj);
%End
};

class SoftOpPeakFinder
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
 public:

  enum ComputingMode {MAXIMUM,CM};
  SoftOpPeakFinder();
  ~SoftOpPeakFinder();

  void clearCounterStatus();
  int  getCounterStatus() const;
  
  void setBufferSize(int size);
  void getBufferSize(int &size /Out/) const;
   
  void setMask(Data &aMask); 
  void readPeaks(std::list<Tasks::PeakFinderResult> &result /Out/) const;
  void setComputingMode( SoftOpPeakFinder::ComputingMode );
  void getComputingMode( SoftOpPeakFinder::ComputingMode& /Out/) const;	   	   
};

class SoftOpBinning
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftOpBinning();
  virtual ~SoftOpBinning();
    
  void setBinning(int x,int y);
};

class SoftOpBpm
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftOpBpm();
  virtual ~SoftOpBpm();
%If(!WIN64_PLATFORM)
  Tasks::BpmTask* 	getTask();
  Tasks::BpmManager* 	getManager();
%End
};

class SoftOpFlatfieldCorrection
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftOpFlatfieldCorrection();
  virtual ~SoftOpFlatfieldCorrection();
    
  void setFlatFieldImage(Data &aData,bool normalize = true);
};

class SoftOpFlip
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftOpFlip();
  virtual ~SoftOpFlip();
    
  void setFlip(bool x,bool y);
};

class SoftOpMask
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  enum Type {STANDARD,DUMMY};
  SoftOpMask();
  virtual ~SoftOpMask();
    
  void setMaskImage(Data &aData);
  void getType(SoftOpMask::Type& /Out/) const;
  void setType(SoftOpMask::Type);
};

class SoftOpRoiCounter
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
 public:
  typedef std::pair<std::string,std::list<Tasks::RoiCounterResult> > RoiNameAndResults;
  typedef std::pair<std::string,lima::Roi> RoiNameAndRoi;
  typedef std::pair<std::string,lima::ArcRoi> RoiNameAndArcRoi;
  typedef std::pair<std::string,Tasks::RoiCounterTask*> RoiNameAndTask;
  typedef std::pair<std::string,int> RoiNameAndType;

  SoftOpRoiCounter();
  virtual ~SoftOpRoiCounter();

  void updateRois(const std::list<SoftOpRoiCounter::RoiNameAndRoi>&);
  void updateArcRois(const std::list<SoftOpRoiCounter::RoiNameAndArcRoi>&);

  void getRois(std::list<SoftOpRoiCounter::RoiNameAndRoi>& /Out/) const;
  void getArcRois(std::list<SoftOpRoiCounter::RoiNameAndArcRoi>& /Out/) const;

  void setLut(const std::string& name,
	      const Point& origin,Data &lut);
  void setLutMask(const std::string& name,
		  const Point& origin,Data &lut);

  void getNames(std::list<std::string>& roi_names /Out/) const;
  void getTypes(std::list<SoftOpRoiCounter::RoiNameAndType>& /Out/) const;
  void getTasks(std::list<SoftOpRoiCounter::RoiNameAndTask>& /Out/);

  void readCounters(int from,std::list<SoftOpRoiCounter::RoiNameAndResults> &result /Out/) const;

  void removeRois(const std::list<std::string>& names);
  void clearAllRois();

  void clearCounterStatus();
  int  getCounterStatus() const;
    
  void setMask(Data &aMask);

  void setBufferSize(int size);
  void getBufferSize(int &size /Out/) const;
};

class SoftOpRoi2Spectrum
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  typedef std::pair<std::string,std::list<Tasks::Roi2SpectrumResult> > RoiNameAndResults;
  typedef std::pair<std::string,lima::Roi> RoiNameAndRoi;
  typedef std::pair<std::string,Tasks::Roi2SpectrumTask*> RoiNameAndTask;
  typedef std::pair<std::string,int> RoiNameAndMode;

  SoftOpRoi2Spectrum();
  virtual ~SoftOpRoi2Spectrum();

  void updateRois(const std::list<SoftOpRoi2Spectrum::RoiNameAndRoi>&);
  void getRois(std::list<SoftOpRoi2Spectrum::RoiNameAndRoi>& /Out/) const;

  void setRoiModes(const std::list<SoftOpRoi2Spectrum::RoiNameAndMode>&);
  void getRoiModes(std::list<SoftOpRoi2Spectrum::RoiNameAndMode>& /Out/) const;

  void getNames(std::list<std::string>& roi_names /Out/) const;
  void getTasks(std::list<SoftOpRoi2Spectrum::RoiNameAndTask>& /Out/);

  void readCounters(int from,std::list<SoftOpRoi2Spectrum::RoiNameAndResults> &result /Out/) const;
  void createImage(std::string,int &from /In,Out/,Data &aData /Out/) const;

  void removeRois(const std::list<std::string>& names);
  void clearAllRois();

  void clearCounterStatus();
  int  getCounterStatus() const;

  void setBufferSize(int size);
  void getBufferSize(int &size /Out/) const;
    
  // probably needed in future
  //void setMask(Data &aMask);
};

class SoftOpSoftRoi
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftOpSoftRoi();
  virtual ~SoftOpSoftRoi();
    
  void setRoi(int x,int y,int width,int height);
};

class SoftCallback
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftCallback();
  virtual ~SoftCallback();
protected:
  virtual void prepare() = 0;
};

class SoftUserLinkTask
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftUserLinkTask();
  virtual ~SoftUserLinkTask();

  void setLinkTask(LinkTask*);

  void registerCallback(SoftCallback&);
  void unregisterCallback(SoftCallback&);
};

class SoftUserSinkTask
{
%TypeHeaderCode
#include "lima/SoftOpId.h"
using namespace lima;
%End
public:
  SoftUserSinkTask();
  virtual ~SoftUserSinkTask();

  void setSinkTask(SinkTaskBase*);

  void registerCallback(SoftCallback&);
  void unregisterCallback(SoftCallback&);
};
