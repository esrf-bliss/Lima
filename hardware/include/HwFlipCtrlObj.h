#ifndef HWFLIPCTRLOBJ_H
#define HWFLIPCTRLOBJ_H

#include "Compatibility.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima
{

class DLL_EXPORT HwFlipCtrlObj
{
	DEB_CLASS(DebModHardware, "HwFlipCtrlObj");

 public:
	HwFlipCtrlObj();
	virtual ~HwFlipCtrlObj();

	virtual void setFlip(const Flip& flip) = 0;
	virtual void getFlip(Flip& flip) = 0;
	virtual void checkFlip(Flip& flip) = 0;

};


} // namespace lima

#endif // HWFLIPCTRLOBJ_H
