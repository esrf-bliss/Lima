#ifndef HWCAP_H
#define HWCAP_H

#include "Compatibility.h"
#include "Constants.h"

namespace lima
{
class HwDetInfoCtrlObj;
class HwBufferCtrlObj;
class HwSyncCtrlObj;
class HwBinCtrlObj;
class HwRoiCtrlObj;
class HwFlipCtrlObj;
class HwShutterCtrlObj;

class DLL_EXPORT HwCap
{
public:
	enum Type {
	        Undef,
		DetInfo, Buffer, Sync, 	// Standard: these are mandatory
		Roi, Bin, Flip,		// Image operations
		Kinetics, FrameTransfer,// Acquisition modes
		Timing,			// Read only detector timmings
		Shutter,		// Shutter control
		SerialLine, 		// Generic detector serial line
	};
	HwCap() : m_type(Undef),m_ctrl_obj(NULL) {}
	
	template <class CtrlObj>
	HwCap(CtrlObj *ctrl_obj) : m_ctrl_obj(ctrl_obj)
	{ m_type = getTypeFromCtrlObj(ctrl_obj); }

	HwCap(const HwCap &cap) : m_type(cap.m_type),m_ctrl_obj(cap.m_ctrl_obj) {}

	Type getType() const
	{ return m_type; }

	static Type getTypeFromCtrlObj(HwDetInfoCtrlObj*);
	static Type getTypeFromCtrlObj(HwBufferCtrlObj*);
	static Type getTypeFromCtrlObj(HwSyncCtrlObj*);
	static Type getTypeFromCtrlObj(HwBinCtrlObj*);
	static Type getTypeFromCtrlObj(HwRoiCtrlObj*);
	static Type getTypeFromCtrlObj(HwFlipCtrlObj*);
	static Type getTypeFromCtrlObj(HwShutterCtrlObj*);

	template <class CtrlObj>
	bool getCtrlObj(CtrlObj *& ctrl_obj) const
	{ 
		bool ok = (m_type == getTypeFromCtrlObj(ctrl_obj));
		ctrl_obj = ok ? (CtrlObj *) m_ctrl_obj : NULL;
		return ok;
	}
 private:
	Type m_type;
	void *m_ctrl_obj;
};
 
} // namespace lima

#include "HwDetInfoCtrlObj.h"
#include "HwBufferCtrlObj.h"
#include "HwSyncCtrlObj.h"
#include "HwBinCtrlObj.h"
#include "HwRoiCtrlObj.h"
#include "HwFlipCtrlObj.h"
#include "HwShutterCtrlObj.h"

#endif // HWCAP_H
