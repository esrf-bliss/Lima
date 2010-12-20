#ifndef HWROICTRLOBJ_H
#define HWROICTRLOBJ_H

#include "Compatibility.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima
{

class DLL_EXPORT HwRoiCtrlObj
{
	DEB_CLASS(DebModHardware, "HwRoiCtrlObj");

 public:
	HwRoiCtrlObj();
	virtual ~HwRoiCtrlObj();

	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi) = 0;
	virtual void setRoi(const Roi& set_roi) = 0;
	virtual void getRoi(Roi& hw_roi) = 0;
};


} // namespace lima


# endif // HWROICTRLOBJ_H
