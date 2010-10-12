#ifndef HWSHUTTERCTRLOBJ_H
#define HWSHUTTERCTRLOBJ_H

#include "Constants.h"
#include "Debug.h"
#include <vector>

namespace lima
{

class HwShutterCtrlObj
{
	DEB_CLASS(DebModHardware, "HwShutterCtrlObj");

public:
	HwShutterCtrlObj();
	virtual ~HwShutterCtrlObj();

	virtual bool checkMode(ShutterMode shut_mode) const = 0;
	virtual void getModeList(ShutterModeList&  mode_list) const = 0;
	virtual void setMode(ShutterMode  shut_mode) = 0;
	virtual void getMode(ShutterMode& shut_mode) const = 0;

	virtual void setState(bool  shut_open) = 0;
	virtual void getState(bool& shut_open) const = 0;

	virtual void setOpenTime (double  shut_open_time)  = 0;
	virtual void getOpenTime (double& shut_open_time) const = 0;
	virtual void setCloseTime(double  shut_close_time) = 0;
	virtual void getCloseTime(double& shut_close_time) const = 0;
};



} // namespace lima

#endif // HWSHUTTERCTRLOBJ_H
