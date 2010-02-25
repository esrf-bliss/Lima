#ifndef HWINTERFACE_H
#define HWINTERFACE_H

#include "HwCap.h"
#include <vector>
#include <ostream>

namespace lima
{

class HwInterface
{
	DEB_CLASS(DebModHardware, "HwInterface");

 public:
	typedef std::vector<HwCap> CapList;

	typedef struct Status {
		AcqStatus acq;
		DetStatus det;
		DetStatus det_mask;
	} StatusType;

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

	virtual void getCapList(CapList &) const = 0;

	template <class CtrlObj>
	bool getHwCtrlObj(CtrlObj *& ctrl_obj_ptr) const;

	virtual void reset(ResetLevel reset_level) = 0;
	virtual void prepareAcq() = 0;
	virtual void startAcq() = 0;
	virtual void stopAcq() = 0;
	virtual void getStatus(StatusType& status) = 0;

	virtual int getNbAcquiredFrames();
	virtual int getNbHwAcquiredFrames() = 0;
};

template <class CtrlObj>
bool HwInterface::getHwCtrlObj(CtrlObj *& ctrl_obj) const
{
        CapList cap_list;
	getCapList(cap_list);
	typedef CapList::const_iterator It;
	for (It i = cap_list.begin(); i != cap_list.end(); ++i)
		if (i->getCtrlObj(ctrl_obj))
			return true;

	ctrl_obj = NULL;
	return false;
}

std::ostream& operator <<(std::ostream& os, 
			  const HwInterface::StatusType& status);
std::ostream& operator <<(std::ostream& os, 
			  HwInterface::ResetLevel reset_level);


} // namespace lima

#endif // HWINTERFACE_H
