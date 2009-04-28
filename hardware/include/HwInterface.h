#ifndef HWINTERFACE_H
#define HWINTERFACE_H

#include "HwCap.h"
#include <vector>

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
		DetFault=0x1, 
		WaitForTrigger=0x2,
		ShutterOpen=0x4,
		Exposure=0x8,
		ShutterClose=0x10,
		ChargeShift=0x20,
		Readout=0x40,
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

	virtual void reset(ResetLevel reset_level) = 0;
	virtual void prepareAcq() = 0;
	virtual void startAcq() = 0;
	virtual void stopAcq() = 0;
	virtual void getStatus(StatusType& status) = 0;
	virtual unsigned long getAcqFrameNb() = 0;
};

} // namespace lima

#endif // HWINTERFACE_H
