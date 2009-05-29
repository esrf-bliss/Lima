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

} // namespace lima
