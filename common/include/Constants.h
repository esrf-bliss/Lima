#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace lima
{

enum AlignDir {
	Floor, Ceil,
};

enum ImageType {
	Bpp8, Bpp10, Bpp12, Bpp14, Bpp16, Bpp32
};

enum AcqMode {
	Single, Accumulation, Concatenation,
};

enum TrigMode {
	Internal, 
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop,
};

enum BufferMode {
	Linear, Circular,
};

} // namespace lima

#endif // CONSTANTS_H
