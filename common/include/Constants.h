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

enum ShutMode {
	Manual, AutoFrame, AutoSeq,
};

enum BufferMode {
	Linear, Circular,
};

} // lima

#endif // CONSTANTS_H
