
class HwCap
{
%TypeHeaderCode
#include <HwCap.h>
#include <HwDetInfoCtrlObj.h>
#include <HwBufferCtrlObj.h>
#include <HwSyncCtrlObj.h>
#include <HwBinCtrlObj.h>
using namespace lima;
%End
public:
	enum Type {
		DetInfo, Buffer, Sync, 	// Standard: these are mandatory
		Roi, Bin, Flip,		// Image operations
		Kinetics, FrameTransfer,// Acquisition modes
		Timing,			// Read only detector timmings
		Shutter,		// Shutter control
		SerialLine, 		// Generic detector serial line
	};
        HwCap(HwDetInfoCtrlObj*);
	HwCap(HwBufferCtrlObj*);
	Type getType() const;
	SIP_PYOBJECT getCtrlObj();
%MethodCode

%End
};
 
