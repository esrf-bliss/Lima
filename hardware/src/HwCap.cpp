#include "HwCap.h"

namespace lima
{


template <>
HwCap::Type HwCap::getTypeFromCtrlObj<>(HwDetInfoCtrlObj *p)
{
	return DetInfo;
}

template <>
HwCap::Type HwCap::getTypeFromCtrlObj<>(HwBufferCtrlObj *p)
{
	return Buffer;
}

template <>
HwCap::Type HwCap::getTypeFromCtrlObj<>(HwSyncCtrlObj *p)
{
	return Sync;
}

template <>
HwCap::Type HwCap::getTypeFromCtrlObj<>(HwBinCtrlObj *p)
{
	return Bin;
}

template <>
HwCap::Type HwCap::getTypeFromCtrlObj<>(HwRoiCtrlObj *p)
{
	return Roi;
}


} // namespace lima
