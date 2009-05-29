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
	Single, Accumulation, Concatenation,
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

} // namespace lima

#endif // CONSTANTS_H
