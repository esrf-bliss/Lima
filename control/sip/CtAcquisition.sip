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
class CtAcquisition {
%TypeHeaderCode
#include <CtAcquisition.h>
#include <Constants.h>
using namespace lima;
%End
    public:
     enum AccTimeMode {Live,Real};

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

		const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
	};

	CtAcquisition(HwInterface *hw);
	~CtAcquisition();

	// --- global

	void setPars(const Parameters &pars);
	void getPars(Parameters& pars /Out/) const;

	void reset();
	void apply(CtControl::ApplyPolicy policy);
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
};

