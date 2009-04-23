#ifndef HWCAP_H
#define HWCAP_H

namespace lima
{

class HwCap
{
public:
	enum Type {
		Sync, Buffer,		// Standard: these are mandatory
		Roi, Bin, Flip,		// Image
		Kinetics, FrameTransfer,// Acquisition modes
		Timing,			// Read only detector timmings
		SerialLine, 		// Generic detector serial line
	};

	Type getType() const;
	void *getCtrlObj() const;

	HwCap(Type type, void *ctrl_obj);


 private:
	Type m_type;
	void *m_ctrl_obj;
};
 
} // namespace lima

#endif // HWCAP_H
