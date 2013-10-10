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
#include <RoiCounter.h>
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
    	        PyString_Check(PySequence_Fast_GET_ITEM(p,0)) &&
                PyList_Check(PySequence_Fast_GET_ITEM(p,1));
  if(aReturnFlag)
    {
      PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
      for(int k = 0;aReturnFlag && k < PyList_Size(aTmpList);++k)
	aReturnFlag = sipCanConvertToType(PyList_GET_ITEM(aTmpList, k),
					  sipType_Tasks_RoiCounterResult, SIP_NOT_NONE);
    }
 }
 return aReturnFlag;
}

std::list<SoftOpRoiCounter::RoiNameAndResults> *roiNameNResultPt = 
	new std::list<SoftOpRoiCounter::RoiNameAndResults>();

for(int i = 0;i < PyList_Size(sipPy);++i)
{
 PyObject *p = PyList_GET_ITEM(sipPy, i);
 char* name = PyString_AsString(PySequence_Fast_GET_ITEM(p,0));
 PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
 
 roiNameNResultPt->push_back(SoftOpRoiCounter::RoiNameAndResults(name,
								 std::list<Tasks::RoiCounterResult>()));
 for(int k = 0;k < PyList_Size(aTmpList);++k)
   {
     int state;
     Tasks::RoiCounterResult *aRoiCounterResult = 
       reinterpret_cast<Tasks::RoiCounterResult*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								   sipType_Tasks_RoiCounterResult, 0,	
								   SIP_NOT_NONE,
								   &state, sipIsErr));
     // Deal with any errors.
     if (*sipIsErr)
       {
	 sipReleaseType(aRoiCounterResult, sipType_Tasks_RoiCounterResult, state);
	 
	 // Tidy up.
	 delete roiNameNResultPt;

	 // There is no temporary instance.
	 return 0;
       }
     
     roiNameNResultPt->back().second.push_back(*aRoiCounterResult);
     sipReleaseType(aRoiCounterResult, sipType_Tasks_RoiCounterResult, state);
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
    std::list<Tasks::RoiCounterResult> &aRoiCounterResultList = (*j).second;
    PyObject *PyRoiResultList = PyList_New(aRoiCounterResultList.size());
    for(std::list<Tasks::RoiCounterResult>::iterator k = aRoiCounterResultList.begin();
        k != aRoiCounterResultList.end();++k,++m)
    {
     PyObject *wobj;

     // Get the Python wrapper for the RoiCounterResult instance, creating a new
     // one if necessary, and handle any ownership transfer.
     Tasks::RoiCounterResult *aResultPt = new Tasks::RoiCounterResult(*k);
     if (!(wobj = sipConvertFromNewType(aResultPt, sipType_Tasks_RoiCounterResult, sipTransferObj)))
     {
      delete aResultPt;
      // There was an error so garbage collect the Python lists.
      Py_DECREF(PyRoiResultList);
      Py_DECREF(l);
      return NULL;
     }
     PyList_SET_ITEM(PyRoiResultList,m,wobj);
    }

   PyObject *aNamePy = PyString_FromString((*j).first.c_str());
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
#include <RoiCounter.h>
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
                      PyString_Check(PySequence_Fast_GET_ITEM(p,0)) &&
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
      char *name = PyString_AsString(PySequence_Fast_GET_ITEM(p,0));
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
      PyObject *aNamePy = PyString_FromString(j->first.c_str());
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
#include <RoiCounter.h>
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
                      PyString_Check(PySequence_Fast_GET_ITEM(p,0)) &&
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
      char *name = PyString_AsString(PySequence_Fast_GET_ITEM(p,0));
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
      PyObject *aNamePy = PyString_FromString(j->first.c_str());
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoiCounter::RoiNameAndTask>
{
%TypeHeaderCode
#include <list>
#include <RoiCounter.h>
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
                      PyString_Check(PySequence_Fast_GET_ITEM(p,0)) &&
                      sipCanConvertToType(PySequence_Fast_GET_ITEM(p,1),
					  sipType_Tasks_RoiCounterTask,SIP_NOT_NONE);
      }
    return aReturnFlag;
  }
  SoftOpRoiCounter::RoiNameAndTaskList *named_roisPt = new SoftOpRoiCounter::RoiNameAndTaskList();

  for(int i = 0;i < PyList_Size(sipPy);++i)
    {
      PyObject *p = PyList_GET_ITEM(sipPy,i);
      char *name = PyString_AsString(PySequence_Fast_GET_ITEM(p,0));
      int state;
      Tasks::RoiCounterTask *task = 
	reinterpret_cast<Tasks::RoiCounterTask*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								  sipType_Tasks_RoiCounterTask,0,
								  SIP_NOT_NONE,
								  &state,sipIsErr));
      if(*sipIsErr)
	{
	  sipReleaseType(task,sipType_Tasks_RoiCounterTask,state);
	  delete named_roisPt;
	  return 0;
	}
      named_roisPt->push_back(SoftOpRoiCounter::RoiNameAndTask(name,task));
      sipReleaseType(task,sipType_ArcRoi,state);
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
  for(SoftOpRoiCounter::RoiNameAndTaskList::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
    {
      PyObject *wobj;
      Tasks::RoiCounterTask *task = new Tasks::RoiCounterTask(*(j->second));
      if(!(wobj = sipConvertFromNewType(task,sipType_Tasks_RoiCounterTask,sipTransferObj)))
	{
	  delete task;
	  Py_DECREF(l);
	  return NULL;
	}
      PyObject *aNamePy = PyString_FromString(j->first.c_str());
      PyList_SET_ITEM(l,i,PyTuple_Pack(2,aNamePy,wobj));
      Py_DECREF(aNamePy);Py_DECREF(wobj);
    }
  return l;
%End
};

%MappedType std::list<SoftOpRoi2Spectrum::RoiIdAndResults>
{
%TypeHeaderCode
#include <list>
#include <Roi2Spectrum.h>
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
    	        PyInt_Check(PySequence_Fast_GET_ITEM(p,0)) &&
                PyList_Check(PySequence_Fast_GET_ITEM(p,1));
  if(aReturnFlag)
    {
      PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
      for(int k = 0;aReturnFlag && k < PyList_Size(aTmpList);++k)
	aReturnFlag = sipCanConvertToType(PyList_GET_ITEM(aTmpList, k),
					  sipType_Tasks_Roi2SpectrumResult, SIP_NOT_NONE);
    }
 }
 return aReturnFlag;
}

std::list<SoftOpRoi2Spectrum::RoiIdAndResults> *roiIdNResultPt = 
	new std::list<SoftOpRoi2Spectrum::RoiIdAndResults>();

for(int i = 0;i < PyList_Size(sipPy);++i)
{
 PyObject *p = PyList_GET_ITEM(sipPy, i);
 int anId = PyInt_AsLong(PySequence_Fast_GET_ITEM(p,0));
 PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
 
 roiIdNResultPt->push_back(SoftOpRoi2Spectrum::RoiIdAndResults(anId,std::list<Tasks::Roi2SpectrumResult>()));
 for(int k = 0;k < PyList_Size(aTmpList);++k)
   {
     int state;
     Tasks::Roi2SpectrumResult *aRoi2SpectrumResult = 
       reinterpret_cast<Tasks::Roi2SpectrumResult*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								     sipType_Tasks_Roi2SpectrumResult, 0,	
								     SIP_NOT_NONE,
								     &state, sipIsErr));
     // Deal with any errors.
     if (*sipIsErr)
       {
	 sipReleaseType(aRoi2SpectrumResult, sipType_Tasks_Roi2SpectrumResult, state);
	 
	 // Tidy up.
	 delete roiIdNResultPt;

	 // There is no temporary instance.
	 return 0;
       }
     
     roiIdNResultPt->back().second.push_back(*aRoi2SpectrumResult);
     sipReleaseType(aRoi2SpectrumResult, sipType_Tasks_Roi2SpectrumResult, state);
   }
}
*sipCppPtr = roiIdNResultPt;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
 PyObject *l;

 // Create the Python list of the correct length.
 if (!(l = PyList_New(sipCpp->size())))
  return NULL;

 sipTransferObj = NULL;
 int i = 0;
 for (std::list<SoftOpRoi2Spectrum::RoiIdAndResults>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
  {
    int m = 0;
    std::list<Tasks::Roi2SpectrumResult> &aRoi2SpectrumResultList = (*j).second;
    PyObject *PyRoiResultList = PyList_New(aRoi2SpectrumResultList.size());
    for(std::list<Tasks::Roi2SpectrumResult>::iterator k = aRoi2SpectrumResultList.begin();
        k != aRoi2SpectrumResultList.end();++k,++m)
    {
     PyObject *wobj;

     // Get the Python wrapper for the QWidget instance, creating a new
     // one if necessary, and handle any ownership transfer.
     Tasks::Roi2SpectrumResult *aResultPt = new Tasks::Roi2SpectrumResult(*k);
     if (!(wobj = sipConvertFromNewType(aResultPt, sipType_Tasks_Roi2SpectrumResult, sipTransferObj)))
     {
      delete aResultPt;
      // There was an error so garbage collect the Python lists.
      Py_DECREF(PyRoiResultList);
      Py_DECREF(l);
      return NULL;
     }
     PyList_SET_ITEM(PyRoiResultList,m,wobj);
    }

   PyObject *anIndexPy = PyInt_FromLong((*j).first);
   PyList_SET_ITEM(l, i, PyTuple_Pack(2,anIndexPy,PyRoiResultList));
   Py_DECREF(anIndexPy);Py_DECREF(PyRoiResultList);
  }

 // Return the Python list.
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
};

struct SoftOpKey
{
%TypeHeaderCode
#include <SoftOpId.h>
%End
 SoftOpKey();
 SoftOpKey(SoftOpId id,const char *name);
 
 SoftOpId m_id;
 const char *m_name;
};

class SoftOpBackgroundSubstraction
{
%TypeHeaderCode
#include <SoftOpId.h>
using namespace lima;
%End
 public:
  SoftOpBackgroundSubstraction();
  ~SoftOpBackgroundSubstraction();
    
  void setBackgroundImage(Data &aData);
};

class SoftOpBinning
{
%TypeHeaderCode
#include <SoftOpId.h>
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
#include <SoftOpId.h>
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
#include <SoftOpId.h>
using namespace lima;
%End
public:
  SoftOpFlatfieldCorrection();
  virtual ~SoftOpFlatfieldCorrection();
    
  void setFlatFieldImage(Data &aData);
};

class SoftOpFlip
{
%TypeHeaderCode
#include <SoftOpId.h>
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
#include <SoftOpId.h>
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
#include <SoftOpId.h>
using namespace lima;
%End
 public:
  typedef std::pair<std::string,std::list<Tasks::RoiCounterResult> > RoiNameAndResults;
  typedef std::pair<std::string,lima::Roi> RoiNameAndRoi;
  typedef std::pair<std::string,lima::ArcRoi> RoiNameAndArcRoi;
  typedef std::pair<std::string,Tasks::RoiCounterTask*> RoiNameAndTask;

  SoftOpRoiCounter();
  ~SoftOpRoiCounter();

  void update(const std::list<SoftOpRoiCounter::RoiNameAndRoi>&);
  void update(const std::list<SoftOpRoiCounter::RoiNameAndArcRoi>&);

  void setLut(const std::string& name,
	      const Point& origin,Data &lut);
  void setLutMask(const std::string& name,
		  const Point& origin,Data &lut);
  void getTasks(std::list<SoftOpRoiCounter::RoiNameAndTask>& /Out/);

  void names(std::list<std::string>& roi_names /Out/) const;
  void remove(const std::list<std::string>& names);
  void clearAllRoi();

  void clearCounterStatus();
  int  getCounterStatus() const;
    
  void setMask(Data &aMask);

  void setBufferSize(int size);
  void getBufferSize(int &size /Out/) const;
    
  void readCounters(int from,std::list<SoftOpRoiCounter::RoiNameAndResults> &result /Out/) const;
};

class SoftOpRoi2Spectrum
{
public:
  typedef std::pair<int,std::list<Tasks::Roi2SpectrumResult> > RoiIdAndResults;

  SoftOpRoi2Spectrum();
  virtual ~SoftOpRoi2Spectrum();

  void add(const std::list<Roi> &rois); 
  void set(const std::list<Roi> &rois); 
  void get(std::list<Roi>& /Out/) const;
  void del(const std::list<int> &roiIds);
  void clearAllRoi();		/* clear all roi */

  void getRoiMode(std::list<int>& /Out/) const;
  void setRoiMode(int roiId,int mode);

  void clearCounterStatus();
  int  getCounterStatus() const;

  // probably needed in future
  //void setMask(Data &aMask);

  void setBufferSize(int size);
  void getBufferSize(int &size /Out/) const;
    
  void readCounters(int from,std::list<SoftOpRoi2Spectrum::RoiIdAndResults> &result /Out/) const;
  void createImage(int roiId,int &from /In,Out/,Data &aData /Out/) const;
};

class SoftOpSoftRoi
{
%TypeHeaderCode
#include <SoftOpId.h>
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
#include <SoftOpId.h>
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
#include <SoftOpId.h>
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
#include <SoftOpId.h>
using namespace lima;
%End
public:
  SoftUserSinkTask();
  virtual ~SoftUserSinkTask();

  void setSinkTask(SinkTaskBase*);

  void registerCallback(SoftCallback&);
  void unregisterCallback(SoftCallback&);
};
