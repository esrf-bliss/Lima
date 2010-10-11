
namespace Frelon
{

%TypeHeaderCode
#include "Frelon.h"
#include "MiscUtils.h"
%End

enum Reg {
	NbFrames,	ExpTime,	ShutCloseTime,	LatencyTime,
	RoiLineBegin,	RoiLineWidth,	RoiPixelBegin,	RoiPixelWidth,
	ChanMode,	TimeUnit,	RoiEnable,	RoiFast, 
	AntiBloom,	BinVert,	BinHorz,	ConfigHD,
	RoiKinetic,	ShutEnable,	HardTrigDisable,
	PixelFreq,	LineFreq,	FlipMode,	IntCalib,
	DisplayImage,	AdcFloatDiode,	AdcSignal,	
	DarkPixelCalib,	DarkPixelMode,	ChanControl,	Mire,
	AoiLineBegin,	AoiLineWidth,	AoiPixelBegin,	AoiPixelWidth,
	AoiImageHeight,	AoiImageWidth,	ChanOnImage,	ChanOnCcd,
	Version,	CompSerNb,	Warn,		LastWarn,
	LineClockPer,	PixelClockPer,	FirstPHIVLen,	PHIHSetupLen,
	SingleVertXfer,	SingleHorzXfer,	AllVertXfer,	AllHorzXfer,
	ReadoutTime,	TransferTime,   CcdModesAvail,
};
/*
typedef std::map<Reg, std::string> RegStrMapType;
extern RegStrMapType RegStrMap;

typedef std::vector<Reg> RegListType;
extern RegListType NonCacheableRegList;
*/

const int MaxRegVal;

enum Cmd {
	Reset,		Start,		Stop,		Save,
};
/*
typedef std::map<Cmd, std::string> CmdStrMapType;
extern CmdStrMapType CmdStrMap;
*/

enum MultiLineCmd {
	Help,		Config,		Dac,		Volt,
	Aoi,		PLL,		Timing,
};
/*
typedef std::map<MultiLineCmd, std::string> MultiLineCmdStrMapType;
extern MultiLineCmdStrMapType MultiLineCmdStrMap;
*/

enum FrameTransferMode {
	FFM = 0, FTM = 1,
};

enum InputChan {
	Chan1    = 1 /*(1 << 0)*/,
	Chan2    = 2 /*(1 << 1)*/,
	Chan3    = 4 /*(1 << 2)*/,
	Chan4    = 8 /*(1 << 3)*/,
	Chan13   = 5 /*Chan1  | Chan3*/,
	Chan24   = 10 /*Chan2  | Chan4*/,
	Chan12   = 3 /*Chan1  | Chan2*/,
	Chan34   = 12 /*Chan3  | Chan4*/,
	Chan1234 = 15 /*Chan12 | Chan34*/,
};
/*
typedef std::pair<int, int> ChanRange;
typedef std::map<FrameTransferMode, ChanRange> FTMChanRangeMapType;
extern FTMChanRangeMapType FTMChanRangeMap;

typedef std::vector<InputChan> InputChanList;
typedef std::map<FrameTransferMode, InputChanList> FTMInputChanListMapType;
extern FTMInputChanListMapType FTMInputChanListMap;
*/

enum SerNbParam {
	SerNb      = 0x00ff,
	SPB1Kodak  = 0x2000,
	SPB1Adc16  = 0x4000,
	SPB2Sign   = 0x0100,
	SPB2Type   = 0x7000,
	Taper      = 0x8000,
};

enum RoiMode {
	None, Slow, Fast, Kinetic,
};

enum TimeUnitFactor {
	Milliseconds, Microseconds,
};
/*
typedef std::map<TimeUnitFactor, double> TimeUnitFactorMapType;
extern TimeUnitFactorMapType TimeUnitFactorMap;


extern const FrameDim MaxFrameDim;
*/
enum ChipType {
	Atmel,
	Kodak,
	E2V,
};
/*
typedef std::map<ChipType, double> ChipPixelSizeMapType;
extern ChipPixelSizeMapType ChipPixelSizeMap;
*/
enum {
	MaxBinX = 8,
	MaxBinY = 1024,
};

enum Status {
        Wait       = 0x80,
        Transfer   = 0x40,
        Exposure   = 0x20,
        Shutter    = 0x10,
        Readout    = 0x08,
        Latency    = 0x04,
        ExtStart   = 0x02,
        ExtStop    = 0x01,
	StatusMask = 0xff,
};

enum ShutMode {
	Off, AutoFrame,
};


}; // namespace Frelon
