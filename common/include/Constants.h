#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "LimaCompatibility.h"
#include <ostream>
#include <vector>

namespace lima
{

enum AlignDir {
	Floor, Ceil,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, AlignDir align_dir);
/**@brief ImageType is the depth of detectors images
 * - Bpp8 means 8 bits unsigned
 * - Bpp8S means 8 bits signed....
 */
enum ImageType {
	Bpp8, Bpp8S, Bpp10, Bpp10S, Bpp12, Bpp12S, Bpp14, Bpp14S, 
	Bpp16, Bpp16S, Bpp32, Bpp32S
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, ImageType image_type);

enum AcqMode {
	Single, Concatenation, Accumulation,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, AcqMode acq_mode);

enum TrigMode {
	IntTrig,IntTrigMult,
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, TrigMode trig_mode);

enum BufferMode {
	Linear, Circular,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, BufferMode buffer_mode);

enum ShutterMode {
  ShutterManual, ShutterAutoFrame, ShutterAutoSequence
};

typedef std::vector<ShutterMode> ShutterModeList;

LIMACORE_API std::ostream& operator <<(std::ostream& os, ShutterMode shutter_mode);

enum AcqStatus {
	AcqReady, AcqRunning, AcqFault,
};

LIMACORE_API std::ostream& operator <<(std::ostream& os, AcqStatus acq_status);

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

LIMACORE_API DetStatus  operator | (DetStatus  s1, DetStatus  s2);
LIMACORE_API DetStatus& operator |=(DetStatus& s1, DetStatus  s2);

LIMACORE_API std::ostream& operator <<(std::ostream& os, DetStatus det_status);

} // namespace lima

#endif // CONSTANTS_H
