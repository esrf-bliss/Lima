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
%MappedType std::vector<HwCap>
{
%TypeHeaderCode
#include <vector>
#include "lima/HwCap.h"
%End
%ConvertToTypeCode
if(sipIsErr == NULL)
 return PySequence_Check(sipPy);

*sipCppPtr = new std::vector<HwCap>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
{
  PyObject *itemPt = PyList_GetItem(sipPy,i);
  int state;
  HwCap *hwCapPt = (HwCap*)sipConvertToType(itemPt,sipType_HwCap, sipTransferObj,SIP_NOT_NONE, &state,sipIsErr);
  if(!*sipIsErr)
  {
    if(hwCapPt)
      (*sipCppPtr)->operator[](i) = HwCap(*hwCapPt);
  }
  sipReleaseType(hwCapPt,sipType_HwCap,state);
}
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode

PyObject *returnList =  PyList_New(sipCpp->size());
int aPos = 0;
for(std::vector<HwCap>::iterator i = sipCpp->begin();
    i != sipCpp->end();++i,++aPos)
{
  PyObject *aHwCap = sipConvertFromType(&(*i),sipType_HwCap,NULL);
  PyList_SET_ITEM(returnList,aPos,aHwCap);
  Py_DECREF(aHwCap);
}
return returnList;
%End
};

class HwInterface
{
%TypeHeaderCode
#include "lima/HwCap.h"
#include "lima/HwInterface.h"

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
        struct StatusType {
	  enum Basic {Fault,Ready,Exposure,Readout,Latency,Config};
	  void set(Basic);
		AcqStatus acq;
		DetStatus det;
		DetStatus det_mask;
	SIP_PYOBJECT __repr__() const;
%MethodCode
        LIMA_REPR_CODE
%End
	};

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

	virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const = 0;

	SIP_PYOBJECT getHwCtrlObj(HwCap::Type cap_type);
%MethodCode
	HwInterface::CapList cap_list;
	sipCpp->getCapList(cap_list);

	typedef HwInterface::CapList::const_iterator It;
	for (It i = cap_list.begin(); i != cap_list.end(); ++i)
		if (i->getType() == a0) {
			LIMA_CONVERT_HWCAP_TO_SIP_CTRLOBJ(i, HW);
			break;
		}
%End
	virtual void reset(ResetLevel reset_level) = 0;
	virtual void prepareAcq() = 0;
	virtual void startAcq() = 0;
	virtual void stopAcq() = 0;
	virtual void getStatus(StatusType& status /Out/) = 0;
	virtual int getNbAcquiredFrames();
	virtual int getNbHwAcquiredFrames() = 0;

	virtual bool firstProcessingInPlace() const ;

};
