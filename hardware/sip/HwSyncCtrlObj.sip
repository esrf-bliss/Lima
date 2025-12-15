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
class HwSyncCtrlObj
{
%TypeHeaderCode
#include "lima/HwSyncCtrlObj.h"
using namespace lima;
%End
public:
	enum AutoExposureMode {OFF,ON};

	class ValidRangesType 
	{
	  public:
	  ValidRangesType();
	  ValidRangesType(double minExpTime,
			  double maxExpTime,
			  double minLatTime,
			  double maxLatTime);
	  ValidRangesType(const HwSyncCtrlObj::ValidRangesType &range);

	  double min_exp_time;
	  double max_exp_time;
	  double min_lat_time;
	  double max_lat_time;
	  SIP_PYOBJECT __repr__() const;
%MethodCode
          LIMA_REPR_CODE
%End
	};
	
	class ValidRangesCallback
	{
	public:
	  ValidRangesCallback();
	  virtual ~ValidRangesCallback();
	protected:
	  virtual void validRangesChanged(const HwSyncCtrlObj::ValidRangesType&) = 0;
	};


	HwSyncCtrlObj();
	virtual ~HwSyncCtrlObj();

	virtual bool checkTrigMode(TrigMode trig_mode) = 0;
	virtual void setTrigMode(TrigMode  trig_mode) = 0;
	virtual void getTrigMode(TrigMode& trig_mode /Out/) = 0;

	virtual void setExpTime(double  exp_time) = 0;
	virtual void getExpTime(double& exp_time /Out/) = 0;
	virtual bool checkAutoExposureMode(AutoExposureMode mode) const;
	void setAutoExposureMode(AutoExposureMode mode);
	void getAutoExposureMode(AutoExposureMode& mode /Out/) const;

	virtual void setLatTime(double  lat_time) = 0;
	virtual void getLatTime(double& lat_time /Out/) = 0;

	virtual void setNbFrames(int  nb_frames);
	virtual void getNbFrames(int& nb_frames /Out/);

	virtual void setNbHwFrames(int  nb_frames) = 0;
	virtual void getNbHwFrames(int& nb_frames /Out/) = 0;


	virtual void getValidRanges(HwSyncCtrlObj::ValidRangesType& range /Out/) = 0;

	void registerValidRangesCallback(ValidRangesCallback* cb);
	void unregisterValidRangesCallback(ValidRangesCallback* cb);
	void validRangesChanged(const HwSyncCtrlObj::ValidRangesType&);
	void getAcqMode(AcqMode &acqMode /Out/);
 protected:
	void setAcqMode(AcqMode acqMode);
};
 
