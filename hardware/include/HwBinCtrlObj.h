#ifndef HWBINCTRLOBJ_H
#define HWBINCTRLOBJ_H

#include "SizeUtils.h"

namespace lima
{

class HwBinCtrlObj
{
 public:
	virtual ~HwBinCtrlObj();

	virtual void setBin(const Bin& bin) = 0;
	virtual void getBin(Bin& bin) = 0;
	virtual void checkBin(Bin& bin) = 0;

};


} // namespace lima

#endif // HWBINCTRLOBJ_H
