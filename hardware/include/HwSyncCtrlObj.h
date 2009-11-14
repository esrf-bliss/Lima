#ifndef HWSYNCCTRLOBJ_H
#define HWSYNCCTRLOBJ_H

#include "Constants.h"
#include "HwBufferCtrlObj.h"

namespace lima
{


class HwSyncCtrlObj
{
	DEB_CLASS(DebModHardware, "HwSyncCtrlObj");

public:
	typedef struct ValidRanges {
		double min_exp_time, max_exp_time;
		double min_lat_time, max_lat_time;
	} ValidRangesType;

	HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl);
	virtual ~HwSyncCtrlObj();

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

 private:
	HwBufferCtrlObj& m_buffer_ctrl;
};
 
} // namespace lima

#endif // HWSYNCCTRLOBJ_H
