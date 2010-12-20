#ifndef HWBINCTRLOBJ_H
#define HWBINCTRLOBJ_H

#include "Compatibility.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima
{

class DLL_EXPORT HwBinCtrlObj
{
	DEB_CLASS(DebModHardware, "HwBinCtrlObj");

 public:
	HwBinCtrlObj();
	virtual ~HwBinCtrlObj();

	virtual void setBin(const Bin& bin) = 0;
	virtual void getBin(Bin& bin) = 0;
	virtual void checkBin(Bin& bin) = 0;

};


} // namespace lima

#endif // HWBINCTRLOBJ_H
