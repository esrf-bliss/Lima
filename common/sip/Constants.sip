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
	IntTrig, 
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop,
};

enum BufferMode {
	Linear, Circular,
};
