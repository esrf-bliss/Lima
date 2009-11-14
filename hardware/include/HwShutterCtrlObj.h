#ifndef HWSHUTTERCTRLOBJ_H
#define HWSHUTTERCTRLOBJ_H

#include "Constants.h"
#include "Debug.h"

namespace lima
{

class HwShutterCtrlObj
{
	DEB_CLASS(DebModHardware, "HwShutterCtrlObj");

public:
	enum Mode {
		Manual, AutoFrame, AutoSeq,
	};

	HwShutterCtrlObj();
	virtual ~HwShutterCtrlObj();

	virtual void setMode(Mode  mode) = 0;
	virtual void getMode(Mode& mode) = 0;

	virtual void setState(bool  open) = 0;
	virtual void getState(bool& open) = 0;

	virtual void setOpenTime (double  shut_open_time)  = 0;
	virtual void getOpenTime (double& shut_open_time)  = 0;
	virtual void setCloseTime(double  shut_close_time) = 0;
	virtual void getCloseTime(double& shut_close_time) = 0;

};



} // namespace lima

#endif // HWSHUTTERCTRLOBJ_H
