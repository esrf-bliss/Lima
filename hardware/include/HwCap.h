#ifndef HWCAP_H
#define HWCAP_H

namespace lima
{

class HwCap
{
public:
	enum Type {
		DetInfo, ImageInfo,	// Standard: these are mandatory
		Sync, Buffer,		
		Roi, Bin, Flip,		// Image operations
		Kinetics, FrameTransfer,// Acquisition modes
		Timing,			// Read only detector timmings
		SerialLine, 		// Generic detector serial line
	};

	HwCap(Type type, void *ctrl_obj);

	Type getType() const;
	void *getCtrlObj() const;

 private:
	Type m_type;
	void *m_ctrl_obj;
};
 
} // namespace lima

#include "HwSyncCtrlObj.h"
#include "HwBufferCtrlObj.h"

#endif // HWCAP_H
