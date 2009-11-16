
class HwCap
{
%TypeHeaderCode
#include <iostream>
#include <HwCap.h>
#include <HwDetInfoCtrlObj.h>
#include <HwBufferCtrlObj.h>
#include <HwSyncCtrlObj.h>
#include <HwBinCtrlObj.h>
#include <LimaConvertor.h>
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
	HwCap(HwSyncCtrlObj*);
	HwCap(HwBinCtrlObj*);
	HwCap(HwRoiCtrlObj*);
	HwCap(HwFlipCtrlObj*);
	Type getType() const;
	SIP_PYOBJECT getCtrlObj();
%MethodCode
SIP_CONVERT_CTRLOBJECT_TO_SIPOBJECT(sipCpp);
%End
};
 
