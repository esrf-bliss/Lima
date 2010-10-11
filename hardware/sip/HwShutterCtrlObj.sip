class HwShutterCtrlObj
{
%TypeHeaderCode
#include <HwShutterCtrlObj.h>
using namespace lima;
%End

public:
	enum Mode {
		Manual, AutoFrame, AutoSeq,
	};

	HwShutterCtrlObj();
	virtual ~HwShutterCtrlObj();

	virtual bool checkMode(Mode shut_mode) = 0;
	virtual void setMode(Mode  shut_mode) = 0;
	virtual void getMode(Mode& shut_mode /Out/) = 0;

	virtual void setState(bool  shut_open) = 0;
	virtual void getState(bool& shut_open /Out/) = 0;

	virtual void setOpenTime (double  shut_open_time)  = 0;
	virtual void getOpenTime (double& shut_open_time /Out/)  = 0;
	virtual void setCloseTime(double  shut_close_time) = 0;
	virtual void getCloseTime(double& shut_close_time /Out/) = 0;
};
