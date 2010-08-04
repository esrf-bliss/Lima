enum AlignDir {
	Floor, Ceil,
};

enum ImageType {
  Bpp8, Bpp8S, Bpp10, Bpp10S, Bpp12, Bpp12S, Bpp14, Bpp14S, Bpp16, Bpp16S, Bpp32, Bpp32S
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

enum AcqStatus {
	AcqReady, AcqRunning, AcqFault,
};

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

