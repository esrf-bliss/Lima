#include "Frelon.h"

using namespace lima::Frelon;
using namespace std;

typedef pair<Reg, string> RegPair;
static const RegPair RegStrList[] = {
	RegPair(NrFrames,	"N"),
	RegPair(ExpTime,	"I"),
	RegPair(ShutCloseTime,	"F"),
	RegPair(LatencyTime,	"T"),
	RegPair(RoiLineBegin,	"RLB"),
	RegPair(RoiLineWidth,	"RLW"),
	RegPair(RoiPixelBegin,	"RPB"),
	RegPair(RoiPixelWidth,	"RPW"),
	RegPair(ChanMode,	"M"),
	RegPair(TimeUnit,	"Z"),
	RegPair(RoiEnable,	"R"),
	RegPair(RoiFast,	"RF"),
	RegPair(AntiBloom,	"BL"),
	RegPair(BinVert,	"BV"),
	RegPair(BinHorz,	"BH"),
	RegPair(ConfigHD,	"CNF"),
	RegPair(RoiKinetic,	"SPE"),
	RegPair(ShutEnable,	"U"),
	RegPair(HardTrigDisable,"HTD"),

	RegPair(PixelFreq,	"P"),
	RegPair(LineFreq,	"L"),

	RegPair(Flip,		"FLI"),
	RegPair(IntCalib,	"IE"),
	RegPair(DisplayImage,	"X"),
	RegPair(AdcFloatDiode,	"ADS"),
	RegPair(AdcSignal,	"ASS"),
	RegPair(DarkPixelCalib,	"DPE"),
	RegPair(DarkPixelMode,	"DPM"),
	RegPair(ChanControl,	"CCS"),
	RegPair(Mire,		"MIR"),

	RegPair(AoiLineBegin,	"ALB"),
	RegPair(AoiLineWidth,	"ALW"),
	RegPair(AoiPixelBegin,	"APB"),
	RegPair(AoiPixelWidth,	"APW"),
	RegPair(AoiImageHeight,	"IMH"),
	RegPair(AoiImageWidth,	"IMW"),
	RegPair(ChanOnImage,	"COI"),
	RegPair(ChanOnCcd,	"COC"),

	RegPair(Version,	"VER"),
	RegPair(SerNr,		"SN"),
	RegPair(Warn,		"W"),
};
static int RegStrListLen = sizeof(RegStrList) / sizeof(RegStrList[0]);
map<Reg, string> lima::Frelon::RegStrMap(RegStrList, (RegStrList + 
						      RegStrListLen));


typedef pair<Cmd, string> CmdPair;
static const CmdPair CmdStrList[] = {
	CmdPair(Reset,		"RST"),
	CmdPair(Start,		"S"),
	CmdPair(Stop,		"O"),
	CmdPair(Save,		"SAV"),
};
static int CmdStrListLen = sizeof(CmdStrList) / sizeof(CmdStrList[0]);
map<Cmd, string> lima::Frelon::CmdStrMap(CmdStrList, (CmdStrList + 
						      CmdStrListLen));


typedef pair<MultiLineCmd, string> MLCmdPair;
static const MLCmdPair MLCmdStrList[] = {
	MLCmdPair(Help,		"H"),
	MLCmdPair(Config,	"C"),
	MLCmdPair(Dac,		"D"),
	MLCmdPair(Volt,		"V"),
	MLCmdPair(Aoi,		"AOI"),
};
static int MLCmdStrListLen = sizeof(MLCmdStrList) / sizeof(MLCmdStrList[0]);
static const MLCmdPair *MLCmdStrEnd = MLCmdStrList + MLCmdStrListLen;
map<MultiLineCmd, string> lima::Frelon::MultiLineCmdStrMap(MLCmdStrList, 
							   MLCmdStrEnd);
