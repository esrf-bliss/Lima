class HwRoiCtrlObj
{
%TypeHeaderCode
#include <HwRoiCtrlObj.h>
%End
 public:
	HwRoiCtrlObj();
	virtual ~HwRoiCtrlObj();

	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi /Out/) = 0;
	virtual void setRoi(const Roi& set_roi) = 0;
	virtual void getRoi(Roi& hw_roi /Out/) = 0;
};
