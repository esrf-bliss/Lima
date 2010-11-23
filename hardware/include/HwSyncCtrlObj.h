#ifndef HWSYNCCTRLOBJ_H
#define HWSYNCCTRLOBJ_H

#include "Constants.h"
#include "HwBufferCtrlObj.h"

namespace lima
{

class CtAcquisition;

class HwSyncCtrlObj
{
	DEB_CLASS(DebModHardware, "HwSyncCtrlObj");
	friend class CtAcquisition;
public:
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

	HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl);
	virtual ~HwSyncCtrlObj();

	virtual bool checkTrigMode(TrigMode trig_mode) = 0;
	virtual void setTrigMode(TrigMode  trig_mode) = 0;
	virtual void getTrigMode(TrigMode& trig_mode) = 0;

	virtual void setExpTime(double  exp_time) = 0;
	virtual void getExpTime(double& exp_time) = 0;

	virtual void setLatTime(double  lat_time) = 0;
	virtual void getLatTime(double& lat_time) = 0;

	virtual void setNbFrames(int  nb_frames);
	virtual void getNbFrames(int& nb_frames);

	virtual void setNbHwFrames(int  nb_frames) = 0;
	virtual void getNbHwFrames(int& nb_frames) = 0;

	virtual void getValidRanges(ValidRangesType& valid_ranges) = 0;
 protected:
	inline void getAcqMode(AcqMode &acqMode) const {acqMode = m_acq_mode;}
	inline void setAcqMode(AcqMode acqMode) {m_acq_mode = acqMode;}
 private:
	HwBufferCtrlObj& m_buffer_ctrl;
	AcqMode		 m_acq_mode;
};

std::ostream& operator <<(std::ostream& os, 
			  const HwSyncCtrlObj::ValidRangesType&);

} // namespace lima

#endif // HWSYNCCTRLOBJ_H
