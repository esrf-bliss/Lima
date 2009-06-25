#ifndef FRELON_H
#define FRELON_H

#include "EspiaSerialLine.h"
#include <string>
#include <map>
#include <vector>

namespace lima
{

namespace Frelon
{

enum Reg {
	NbFrames,	ExpTime,	ShutCloseTime,	LatencyTime,
	RoiLineBegin,	RoiLineWidth,	RoiPixelBegin,	RoiPixelWidth,
	ChanMode,	TimeUnit,	RoiEnable,	RoiFast, 
	AntiBloom,	BinVert,	BinHorz,	ConfigHD,
	RoiKinetic,	ShutEnable,	HardTrigDisable,
	PixelFreq,	LineFreq,	Flip,		IntCalib,
	DisplayImage,	AdcFloatDiode,	AdcSignal,	
	DarkPixelCalib,	DarkPixelMode,	ChanControl,	Mire,
	AoiLineBegin,	AoiLineWidth,	AoiPixelBegin,	AoiPixelWidth,
	AoiImageHeight,	AoiImageWidth,	ChanOnImage,	ChanOnCcd,
	Version,	CompSerNb,	Warn,
};

typedef std::map<Reg, std::string> RegStrMapType;
extern RegStrMapType RegStrMap;


enum Cmd {
	Reset,		Start,		Stop,		Save,
};

typedef std::map<Cmd, std::string> CmdStrMapType;
extern CmdStrMapType CmdStrMap;


enum MultiLineCmd {
	Help,		Config,		Dac,		Volt,
	Aoi,
};

typedef std::map<MultiLineCmd, std::string> MultiLineCmdStrMapType;
extern MultiLineCmdStrMapType MultiLineCmdStrMap;


enum FrameTransferMode {
	FFM = 0, FTM = 1,
};

enum InputChan {
	Chan1    = (1 << 0),
	Chan2    = (1 << 1),
	Chan3    = (1 << 2),
	Chan4    = (1 << 3),
	Chan13   = Chan1  | Chan3,
	Chan24   = Chan2  | Chan4,
	Chan12   = Chan1  | Chan2,
	Chan34   = Chan3  | Chan4,
	Chan1234 = Chan12 | Chan34,
};

typedef std::pair<int, int> ChanRange;
typedef std::map<FrameTransferMode, ChanRange> FTMChanRangeMapType;
extern FTMChanRangeMapType FTMChanRangeMap;

typedef std::vector<InputChan> InputChanList;
typedef std::map<FrameTransferMode, InputChanList> FTMInputChanListMapType;
extern FTMInputChanListMapType FTMInputChanListMap;


enum SerNbParam {
	SerNb = 0x00ff,
	F4M   = 0x2000,
	F2k16 = 0x4000,
	Taper = 0x8000,
};

enum RoiMode {
	None, Slow, Fast, Kinetic,
};

enum TimeUnitFactor {
	Milliseconds, Microseconds,
};

typedef std::map<TimeUnitFactor, double> TimeUnitFactorMapType;
extern TimeUnitFactorMapType TimeUnitFactorMap;


extern const FrameDim MaxFrameDim;

enum ChipType {
	Atmel,
	Kodak,
};

typedef std::map<ChipType, double> ChipPixelSizeMapType;
extern ChipPixelSizeMapType ChipPixelSizeMap;

enum {
	MaxBinX = 8,
	MaxBinY = 1024,
};


} // namespace Frelon

} // namespace lima

#endif // FRELON_H
