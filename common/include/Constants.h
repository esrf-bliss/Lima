#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <ostream>

namespace lima
{

enum AlignDir {
	Floor, Ceil,
};

std::ostream& operator <<(std::ostream& os, AlignDir align_dir);

enum ImageType {
	Bpp8, Bpp10, Bpp12, Bpp14, Bpp16, Bpp32
};

std::ostream& operator <<(std::ostream& os, ImageType image_type);

enum AcqMode {
	Single, Concatenation, Accumulation,
};

std::ostream& operator <<(std::ostream& os, AcqMode acq_mode);

enum TrigMode {
	IntTrig, 
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop,
};

std::ostream& operator <<(std::ostream& os, TrigMode trig_mode);

enum BufferMode {
	Linear, Circular,
};

std::ostream& operator <<(std::ostream& os, BufferMode buffer_mode);

enum AcqStatus {
	AcqReady, AcqRunning, AcqFault,
};

std::ostream& operator <<(std::ostream& os, AcqStatus acq_status);

enum DetStatus {
	DetIdle			= 0x00,
	DetFault		= 0x01, 
	DetWaitForTrigger	= 0x02,
	DetShutterOpen		= 0x04,
	DetExposure		= 0x08,
	DetShutterClose		= 0x10,
	DetChargeShift		= 0x20,
	DetReadout		= 0x40,
	DetLatency		= 0x80,
};

DetStatus  operator | (DetStatus  s1, DetStatus  s2);
DetStatus& operator |=(DetStatus& s1, DetStatus  s2);

std::ostream& operator <<(std::ostream& os, DetStatus det_status);

} // namespace lima

#endif // CONSTANTS_H
