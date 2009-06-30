%MappedType std::vector<HwCap>
{
%TypeHeaderCode
#include <vector>
#include <HwCap.h>
%End
%ConvertToTypeCode
if(sipIsErr == NULL)
	return 1;
*sipCppPtr = NULL;
%End

%ConvertFromTypeCode
return Py_None;
%End
};

class HwInterface
{
%TypeHeaderCode
#include <HwInterface.h>
using namespace lima;
%End  
 public:
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

        struct StatusType {
		HwInterface::AcqStatus acq;
		HwInterface::DetStatus det;
		HwInterface::DetStatus det_mask;
	};

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

	virtual const std::vector<HwCap>& getCapList() const = 0;

//	template <class CtrlObj>
//	bool getHwCtrlObj(CtrlObj *& ctrl_obj_ptr) const;

	virtual void reset(ResetLevel reset_level) = 0;
	virtual void prepareAcq() = 0;
	virtual void startAcq() = 0;
	virtual void stopAcq() = 0;
	virtual void getStatus(StatusType& status) = 0;
	virtual int getNbAcquiredFrames() = 0;
};
