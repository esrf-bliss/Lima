#include "HwInterface.h"

using namespace lima;

HwInterface::HwInterface()
{
}

HwInterface::~HwInterface()
{
}

const HwCap *HwInterface::getCapOfType(HwCap::Type cap_type) const
{
	const CapList& cap_list = getCapList();

	typedef CapList::const_iterator It;
	for (It i = cap_list.begin(); i != cap_list.end(); ++i)
		if (i->getType() == cap_type)
			return &(*i);

	return NULL;
}
