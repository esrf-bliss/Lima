class HwShutterCtrlObj
{
%TypeHeaderCode
#include <HwShutterCtrlObj.h>
using namespace lima;
%End

public:
	HwShutterCtrlObj();
	virtual ~HwShutterCtrlObj();

	virtual bool checkMode(ShutterMode shut_mode) const = 0;
	virtual void getModeList(ShutterModeList&  mode_list /Out/) const = 0;
	virtual void setMode(ShutterMode  shut_mode) = 0;
	virtual void getMode(ShutterMode& shut_mode /Out/) const = 0;

	virtual void setState(bool  shut_open) = 0;
	virtual void getState(bool& shut_open /Out/) const = 0;

	virtual void setOpenTime (double  shut_open_time)  = 0;
	virtual void getOpenTime (double& shut_open_time /Out/) const = 0;
	virtual void setCloseTime(double  shut_close_time) = 0;
	virtual void getCloseTime(double& shut_close_time /Out/) const = 0;
};
