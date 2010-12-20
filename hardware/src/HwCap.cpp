#include "HwCap.h"

namespace lima
{


HwCap::Type HwCap::getTypeFromCtrlObj(HwDetInfoCtrlObj *p)
{
	return DetInfo;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwBufferCtrlObj *p)
{
	return Buffer;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwSyncCtrlObj *p)
{
	return Sync;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwBinCtrlObj *p)
{
	return Bin;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwRoiCtrlObj *p)
{
	return Roi;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwFlipCtrlObj *p)
{
	return Flip;
}


HwCap::Type HwCap::getTypeFromCtrlObj(HwShutterCtrlObj *p)
{
	return Shutter;
}


} // namespace lima
