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

	virtual void setMode(Mode  mode) = 0;
	virtual void getMode(Mode& mode /Out/) = 0;

	virtual void setState(bool  open) = 0;
	virtual void getState(bool& open /Out/) = 0;

	virtual void setOpenTime (double  shut_open_time)  = 0;
	virtual void getOpenTime (double& shut_open_time /Out/)  = 0;
	virtual void setCloseTime(double  shut_close_time) = 0;
	virtual void getCloseTime(double& shut_close_time /Out/) = 0;
};
