
class HwCap
{
%TypeHeaderCode
#include <iostream>
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
	HwCap(HwSyncCtrlObj*);
	HwCap(HwBinCtrlObj*);
	Type getType() const;
	SIP_PYOBJECT getCtrlObj();
%MethodCode
switch(sipCpp->getType())
{
case HwCap::DetInfo:
{
  HwDetInfoCtrlObj *aCtrlObjPt;
  if(sipCpp->getCtrlObj(aCtrlObjPt))
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwDetInfoCtrlObj,NULL); 
  break;
}
case HwCap::Buffer:
{
  HwBufferCtrlObj *aCtrlObjPt;
  if(sipCpp->getCtrlObj(aCtrlObjPt))
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwBufferCtrlObj,NULL); 
  break;
}
case HwCap::Sync:
{
  HwSyncCtrlObj *aCtrlObjPt;
  if(sipCpp->getCtrlObj(aCtrlObjPt))
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwSyncCtrlObj,NULL); 
  break;
}
case HwCap::Bin:
{
  HwBinCtrlObj *aCtrlObjPt;
  if(sipCpp->getCtrlObj(aCtrlObjPt))
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwBinCtrlObj,NULL); 
  break;
}
default:
std::cerr << "HwCap getCtrlObj : sip wrapping Type -> " << sipCpp->getType() << " not yet managed" << std::endl;break;
}
%End
};
 
