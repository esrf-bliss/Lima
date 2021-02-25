///###########################################################################
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
class CtAcquisition {
%TypeHeaderCode
#include "lima/CtAcquisition.h"
#include "lima/Constants.h"
using namespace lima;
%End
    public:
     enum AccTimeMode {Live,Real};
     enum AutoExposureMode {
      OFF,	  ///< Always off
      ON,	  ///< Always on
      ON_LIVE	  ///< ON during live and OFF for standard Acquisition
     };
     typedef std::vector<AutoExposureMode> AutoExposureModeList;

	struct Parameters {
		Parameters();
		void reset();

		AcqMode	         		acqMode;
                CtAcquisition::AccTimeMode      accTimeMode;
		int	         		acqNbFrames;
		double	         		acqExpoTime;
		double	         		accMaxExpoTime;
		int	         		concatNbFrames;
		double	         		latencyTime;
		TrigMode         		triggerMode;

		SIP_PYOBJECT __repr__() const;
%MethodCode
	LIMA_REPR_CODE
%End
	};

	CtAcquisition(HwInterface *hw);
	~CtAcquisition();

	// --- global

	void setPars(const Parameters &pars);
	void getPars(Parameters& pars /Out/) const;

	void reset();
	void apply(CtControl::ApplyPolicy policy, CtControl *control=NULL);
	void sync();

	// --- acq modes

	void setAcqMode(AcqMode mode);
	void getAcqMode(AcqMode& mode /Out/) const;

        void setAccTimeMode(CtAcquisition::AccTimeMode mode);
	void getAccTimeMode(CtAcquisition::AccTimeMode &mode /Out/) const;

	void setAcqNbFrames(int nframes);
	void getAcqNbFrames(int& nframes /Out/) const;
	void getAccLiveTime(double& acc_live_time /Out/) const;
        void getAccDeadTime(double& acc_dead_time /Out/) const;

	void setAcqExpoTime(double acq_time);
	void getAcqExpoTime(double& acq_time /Out/) const;
	bool checkAutoExposureMode(AutoExposureMode mode) const;
    	void getAutoExposureModeList(AutoExposureModeList& modes /Out/) const;
    	void setAutoExposureMode(AutoExposureMode mode);
    	void getAutoExposureMode(AutoExposureMode& mode /Out/) const;

	void setAccMaxExpoTime(double max_time);
	void getAccMaxExpoTime(double& acc_time /Out/) const;

	void getAccNbFrames(int& nframes /Out/) const;
	void getAccExpoTime(double& acc_time /Out/) const;

	void setConcatNbFrames(int nframes);
	void getConcatNbFrames(int& nframes /Out/) const;

	// --- common

	void setLatencyTime(double latency_time);
	void getLatencyTime(double& latency_time /Out/) const;

	void setTriggerMode(TrigMode mode);
	void getTriggerMode(TrigMode& mode /Out/) const;
	void getTriggerModeList(TrigModeList& modes /Out/) const;

	bool isMonitorMode() const;

	private:
	CtAcquisition(const CtAcquisition&);
};

%MappedType CtAcquisition::AutoExposureModeList
{
%TypeHeaderCode
#include <vector>
#include "lima/CtAcquisition.h"
using namespace lima;
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = !!PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
	aReturnFlag = !!sipCanConvertToEnum(PySequence_Fast_GET_ITEM(sipPy,i),
					  sipType_CtAcquisition_AutoExposureMode);
      return aReturnFlag;
    }
  std::vector<CtAcquisition::AutoExposureMode> *modeListPt = new std::vector<CtAcquisition::AutoExposureMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<CtAcquisition::AutoExposureMode>(SIPLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::vector<CtAcquisition::AutoExposureMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_CtAcquisition_AutoExposureMode);
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
