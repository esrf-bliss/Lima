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
#ifndef HWSYNCCTRLOBJ_H
#define HWSYNCCTRLOBJ_H

#include "lima/LimaCompatibility.h"
#include "lima/Constants.h"
#include "lima/HwBufferCtrlObj.h"

namespace lima
{

class CtAcquisition;

class LIMACORE_API HwSyncCtrlObj
{
	DEB_CLASS(DebModHardware, "HwSyncCtrlObj");
	friend class CtAcquisition;
public:
	enum AutoExposureMode {OFF,ON};
	enum AccessMode {Master,Monitor};

	struct ValidRangesType {
		ValidRangesType() :
			min_exp_time(-1.),
			max_exp_time(-1.),
			min_lat_time(-1.),
			max_lat_time(-1.)
		{}

		ValidRangesType(double minExpTime,
				double maxExpTime,
				double minLatTime,
				double maxLatTime) :
			min_exp_time(minExpTime),
			max_exp_time(maxExpTime),
			min_lat_time(minLatTime),
			max_lat_time(maxLatTime)
		{}

		ValidRangesType(const ValidRangesType& range) :
			min_exp_time(range.min_exp_time),
			max_exp_time(range.max_exp_time),
			min_lat_time(range.min_lat_time),
			max_lat_time(range.max_lat_time)
		{}

		double min_exp_time, max_exp_time;
		double min_lat_time, max_lat_time;
	};

	class LIMACORE_API ValidRangesCallback
	{
	  DEB_CLASS(DebModHardware,"HwSyncCtrlObj::ValidRangesCallback");
	public:
	  ValidRangesCallback();
	  virtual ~ValidRangesCallback();
	protected:
	  virtual void validRangesChanged(const HwSyncCtrlObj::ValidRangesType&) = 0;
	private:
	  friend class HwSyncCtrlObj;
	  HwSyncCtrlObj*	m_hw_sync;
	};

	HwSyncCtrlObj();
	virtual ~HwSyncCtrlObj();

	/// Check wether a given trigger mode is supported
	virtual bool checkTrigMode(TrigMode trig_mode) = 0;
	/// Set the triggering mode
	virtual void setTrigMode(TrigMode  trig_mode) = 0;
	/// Get the current triggering mode
	virtual void getTrigMode(TrigMode& trig_mode) = 0;

	/// Set the frame exposure time
	virtual void setExpTime(double  exp_time) = 0;
	/// Get the current frame exposure time
	virtual void getExpTime(double& exp_time) = 0;

	/// Check wether a given auto exposure mode is supported
	virtual bool checkAutoExposureMode(AutoExposureMode mode) const;
	virtual void setHwAutoExposureMode(AutoExposureMode mode);

	void setAutoExposureMode(AutoExposureMode mode);
	void getAutoExposureMode(AutoExposureMode& mode) const;

	/// Set the latency time between frames
	virtual void setLatTime(double  lat_time) = 0;
	/// Get the current latency time between frames
	virtual void getLatTime(double& lat_time) = 0;

	virtual void setNbFrames(int  nb_frames);
	virtual void getNbFrames(int& nb_frames);

	virtual void setNbHwFrames(int  nb_frames) = 0;
	virtual void getNbHwFrames(int& nb_frames) = 0;

	virtual void getValidRanges(ValidRangesType& valid_ranges) = 0;

	void registerValidRangesCallback(ValidRangesCallback* cb);
	void unregisterValidRangesCallback(ValidRangesCallback* cb);
        inline void validRangesChanged(const ValidRangesType &ranges)
        {
	  DEB_MEMBER_FUNCT();
	  if(m_valid_ranges_cb)
	    m_valid_ranges_cb->validRangesChanged(ranges);
	}
	inline void getAcqMode(AcqMode &acqMode) const {acqMode = m_acq_mode;}

	inline void getAccessMode(AccessMode& mode) const {mode = m_access_mode;}
 protected:
	inline void setAcqMode(AcqMode acqMode) {m_acq_mode = acqMode;}

	AutoExposureMode        m_auto_exposure_mode;
	AccessMode		m_access_mode;
 private:
	AcqMode		 	m_acq_mode;
	ValidRangesCallback* 	m_valid_ranges_cb;
};

LIMACORE_API std::ostream& operator <<(std::ostream& os,
				       const HwSyncCtrlObj::ValidRangesType&);
LIMACORE_API std::ostream& operator <<(std::ostream& os,
				       const HwSyncCtrlObj::AutoExposureMode&);
LIMACORE_API const char* convert_2_string(HwSyncCtrlObj::AutoExposureMode mode);
LIMACORE_API void convert_from_string(const std::string&,HwSyncCtrlObj::AutoExposureMode&);
} // namespace lima

#endif // HWSYNCCTRLOBJ_H
