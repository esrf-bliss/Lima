#ifndef HWINTERFACE_H
#define HWINTERFACE_H

#include "HwCap.h"
#include <vector>
#include <ostream>

namespace lima
{

class HwInterface
{
 public:
	typedef std::vector<HwCap> CapList;

	enum AcqStatus {
		AcqReady, AcqRunning, AcqFault,
	};

	enum DetStatus {
		DetIdle		= 0x00,
		DetFault	= 0x01, 
		WaitForTrigger	= 0x02,
		ShutterOpen	= 0x04,
		Exposure	= 0x08,
		ShutterClose	= 0x10,
		ChargeShift	= 0x20,
		Readout		= 0x40,
		Latency		= 0x80,
	};

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

	virtual const CapList& getCapList() const = 0;

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
	const CapList& cap_list = getCapList();

	typedef CapList::const_iterator It;
	for (It i = cap_list.begin(); i != cap_list.end(); ++i)
		if (i->getCtrlObj(ctrl_obj))
			return true;

	ctrl_obj = NULL;
	return false;
}

HwInterface::DetStatus  operator | (HwInterface::DetStatus  s1,
				    HwInterface::DetStatus  s2);
HwInterface::DetStatus& operator |=(HwInterface::DetStatus& s1,
				    HwInterface::DetStatus  s2);

std::ostream& operator <<(std::ostream& os, 
			  HwInterface::AcqStatus acq_status);
std::ostream& operator <<(std::ostream& os, 
			  HwInterface::DetStatus det_status);
std::ostream& operator <<(std::ostream& os, 
			  const HwInterface::StatusType& status);


} // namespace lima

#endif // HWINTERFACE_H
