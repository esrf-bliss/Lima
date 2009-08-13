class HwSyncCtrlObj
{
%TypeHeaderCode
#include <HwSyncCtrlObj.h>
using namespace lima;
%End
public:
	struct ValidRangesType 
	{
	  double min_exp_time;
	  double max_exp_time;
	  double min_lat_time;
	  double max_lat_time;
	};

	HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl);
	virtual ~HwSyncCtrlObj();

	virtual void setTrigMode(TrigMode  trig_mode) = 0;
	virtual void getTrigMode(TrigMode& trig_mode /Out/) = 0;

	virtual void setExpTime(double  exp_time) = 0;
	virtual void getExpTime(double& exp_time /Out/) = 0;

	virtual void setLatTime(double  lat_time) = 0;
	virtual void getLatTime(double& lat_time /Out/) = 0;

	virtual void setNbFrames(int  nb_frames);
	virtual void getNbFrames(int& nb_frames /Out/);

	virtual void setNbHwFrames(int  nb_frames) = 0;
	virtual void getNbHwFrames(int& nb_frames /Out/) = 0;


	virtual void getValidRanges(ValidRangesType& range /Out/) = 0;
};
 
