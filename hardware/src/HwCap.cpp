#include "HwCap.h"

using namespace lima;

HwCap::HwCap(Type type, void *ctrl_obj)
{
	m_type = type;
	m_ctrl_obj = ctrl_obj;
}

HwCap::Type HwCap::getType() const
{
	return m_type;
}

void *HwCap::getCtrlObj() const
{
	return m_ctrl_obj;
}


