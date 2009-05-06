#ifndef HWSYNCCTRLOBJ_H
#define HWSYNCCTRLOBJ_H

#include "Constants.h"

namespace lima
{

class HwSyncCtrlObj
{
public:
	HwSyncCtrlObj();
	virtual ~HwSyncCtrlObj();

	virtual void setTrigMode(TrigMode  trig_mode) = 0;
	virtual void getTrigMode(TrigMode& trig_mode) = 0;

	virtual void setExpTime(double  exp_time) = 0;
	virtual void getExpTime(double& exp_time) = 0;

	virtual void setLatTime(double  lat_time) = 0;
	virtual void getLatTime(double& lat_time) = 0;

	virtual void setShutMode(ShutMode  shut_mode) = 0;
	virtual void getShutMode(ShutMode& shut_mode) = 0;

	virtual void setShutState(bool  shut_open) = 0;
	virtual void getShutState(bool& shut_open) = 0;

	virtual void setShutOpenTime (double  shut_open_time)  = 0;
	virtual void getShutOpenTime (double& shut_open_time)  = 0;
	virtual void setShutCloseTime(double  shut_close_time) = 0;
	virtual void getShutCloseTime(double& shut_close_time) = 0;

	virtual void setNbFrames(int  nb_frames) = 0;
	virtual void getNbFrames(int& nb_frames) = 0;

 private:
};
 
} // namespace lima

#endif // HWSYNCCTRLOBJ_H
