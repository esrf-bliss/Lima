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
class SoftOpExternalMgr
{
%TypeHeaderCode
#include "lima/SoftOpExternalMgr.h"
using namespace lima;
%End

  public:
    typedef std::string alias;
    typedef int stage;
    SoftOpExternalMgr();
    ~SoftOpExternalMgr();
    
    SIP_PYOBJECT getAvailableOp() const;
%MethodCode
    const SoftOpKey *SoftOpTable;
    sipCpp->getAvailableOp(SoftOpTable);
    int index = 0;
    PyObject *aReturnListPt = PyList_New(0);
    while(1)
     {
       const SoftOpKey &aSoftOpKey = SoftOpTable[index++];
       if(!aSoftOpKey.m_name) break;
       PyObject *wobj = sipConvertFromType((void*)(&aSoftOpKey),sipType_SoftOpKey,NULL);
       if(!wobj) //Error
	{
	  Py_DECREF(aReturnListPt);
          return NULL;
	}
       PyList_Append(aReturnListPt,wobj);
       Py_DECREF(wobj);
     }
sipRes = aReturnListPt;
%End
    void getActiveOp(std::map<int,std::list<std::string> > &activeOp /Out/) const;
    void getActiveStageOp(int,std::list<std::string> &activeOp /Out/) const;

    SIP_PYOBJECT addOp(SoftOpId,const alias&,int stage);
%MethodCode
    SoftOpInstance anInstance;
TRY
    sipCpp->addOp(a0,*a1,a2,anInstance);
INSTANCE_2_SoftOp
CATCH
%End
    void delOp(const alias&);
    SIP_PYOBJECT getOpClass(const alias&) const;
%MethodCode
    SoftOpInstance anInstance;
TRY
    sipCpp->getOpClass(*a0,anInstance);
INSTANCE_2_SoftOp
CATCH
%End

    void setEndLinkTaskCallback(TaskEventCallback *aCbk);
    void setEndSinkTaskCallback(TaskEventCallback *aCbk);

    void addTo(TaskMgr&,int begin_stage,int &last_link_task /Out/,
               int &last_sink_task /Out/);
    
    void isTaskActive(bool &linkTaskFlag /Out/,bool &sinkTaskFlag /Out/) const;
    void prepare();
%TypeCode
#define INSTANCE_2_SoftOp \
    if(anInstance.m_opt) \
      { \
	switch(anInstance.m_key.m_id) \
	{ \
	case(BACKGROUNDSUBSTRACTION): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpBackgroundSubstraction,NULL);break; \
	case(BINNING): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpBinning,NULL);break; \
	case(BPM): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpBpm,NULL);break; \
	case(FLATFIELDCORRECTION): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpFlatfieldCorrection,NULL);break; \
	case(FLIP): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpFlip,NULL);break; \
	case(MASK): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpMask,NULL);break; \
	case(ROICOUNTERS): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpRoiCounter,NULL);break; \
	case(ROI2SPECTRUM): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpRoi2Spectrum,NULL);break; \
	case(SOFTROI): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpSoftRoi,NULL);break; \
	case(PEAKFINDER): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftOpPeakFinder,NULL);break; \
	case(USER_LINK_TASK): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftUserLinkTask,NULL);break; \
	case(USER_SINK_TASK): \
	  sipRes = sipConvertFromType(anInstance.m_opt,sipType_SoftUserSinkTask,NULL);break; \
	default: \
	  throw LIMA_CTL_EXC(NotSupported,"Sofware class not yet managed"); \
	} \
      } \
    else \
      { \
        Py_INCREF(Py_None); \
	sipRes = Py_None; \
      }

#define TRY try \
      {
#define CATCH						\
    }							\
    catch(Exception &exp)				\
    {							\
    const std::string& tmpString = exp.getErrMsg();	\
    const char *detail = tmpString.c_str();		\
    PyErr_SetString(sipException_Exception, detail);	\
    return NULL;					\
    }
%End

  private:
    SoftOpExternalMgr(const SoftOpExternalMgr&);
};
