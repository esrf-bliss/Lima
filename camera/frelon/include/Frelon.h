#ifndef FRELON_H
#define FRELON_H

#include "EspiaSerialLine.h"
#include <string>
#include <map>

namespace lima
{

namespace Frelon
{

enum Reg {
	NrFrames,	ExpTime,	ShutCloseTime,	LatencyTime,
	RoiLineBegin,	RoiLineWidth,	RoiPixelBegin,	RoiPixelWidth,
	ChanMode,	TimeUnit,	RoiEnable,	RoiFast, 
	AntiBloom,	BinVert,	BinHorz,	ConfigHD,
	RoiKinetic,	ShutEnable,	HardTrigDisable,
	PixelFreq,	LineFreq,	Flip,		IntCalib,
	DisplayImage,	AdcFloatDiode,	AdcSignal,	
	DarkPixelCalib,	DarkPixelMode,	ChanControl,	Mire,
	AoiLineBegin,	AoiLineWidth,	AoiPixelBegin,	AoiPixelWidth,
	AoiImageHeight,	AoiImageWidth,	ChanOnImage,	ChanOnCcd,
	Version,	SerNr,		Warn,
};

extern std::map<Reg, std::string> RegStrMap;

enum Cmd {
	Reset,		Start,		Stop,		Save,
};

extern std::map<Cmd, std::string> CmdStrMap;

enum MultiLineCmd {
	Help,		Config,		Dac,		Volt,
	Aoi,
};

extern std::map<MultiLineCmd, std::string> MultiLineCmdStrMap;


} // namespace Frelon

} // namespace lima

#endif // FRELON_H
