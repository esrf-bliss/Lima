#include "Frelon.h"
#include "MiscUtils.h"

using namespace lima;
using namespace lima::Frelon;
using namespace std;

typedef pair<Reg, string> RegPair;
static const RegPair RegStrCList[] = {
	RegPair(NbFrames,	"N"),
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
	RegPair(CompSerNb,	"SN"),
	RegPair(Warn,		"W"),
	RegPair(LastWarn,	"LW"),

	RegPair(LineClockPer,	"TLC"),
	RegPair(PixelClockPer,	"TPC"),
	RegPair(FirstPHIVLen,	"TFV"),
	RegPair(PHIHSetupLen,	"THS"),
	RegPair(SingleVertXfer,	"TOV"),
	RegPair(SingleHorzXfer,	"TOH"),
	RegPair(AllVertXfer,	"TAV"),
	RegPair(AllHorzXfer,	"TAH"),
	RegPair(ReadoutTime,	"TRD"),
	RegPair(TransferTime,	"TTR"),
	RegPair(CcdModesAvail,	"CMA"),

};
RegStrMapType lima::Frelon::RegStrMap(C_LIST_ITERS(RegStrCList));

static Reg NonCacheableRegCList[] = {
	Warn, 
	AoiLineBegin,   AoiLineWidth,  AoiPixelBegin, AoiPixelWidth,
	AoiImageHeight, AoiImageWidth, ChanOnImage,   ChanOnCcd,
};
RegListType 
lima::Frelon::NonCacheableRegList(C_LIST_ITERS(NonCacheableRegCList));

const int lima::Frelon::MaxRegVal = (1 << 16) - 1;

typedef pair<Cmd, string> CmdPair;
static const CmdPair CmdStrCList[] = {
	CmdPair(Reset,		"RST"),
	CmdPair(Start,		"S"),
	CmdPair(Stop,		"O"),
	CmdPair(Save,		"SAV"),
};
CmdStrMapType lima::Frelon::CmdStrMap(C_LIST_ITERS(CmdStrCList));


typedef pair<MultiLineCmd, string> MLCmdPair;
static const MLCmdPair MLCmdStrCList[] = {
	MLCmdPair(Help,		"H"),
	MLCmdPair(Config,	"C"),
	MLCmdPair(Dac,		"D"),
	MLCmdPair(Volt,		"V"),
	MLCmdPair(Aoi,		"AOI"),
	MLCmdPair(PLL,		"PLL"),
	MLCmdPair(Timing,	"TIM"),
};
MultiLineCmdStrMapType 
lima::Frelon::MultiLineCmdStrMap(C_LIST_ITERS(MLCmdStrCList));


typedef pair<FrameTransferMode, ChanRange> RangePair;
static const RangePair FTMChanRangeCList[] = {
	RangePair(FFM, ChanRange(1,  10)),
	RangePair(FTM, ChanRange(10, 13)),
};
FTMChanRangeMapType 
lima::Frelon::FTMChanRangeMap(C_LIST_ITERS(FTMChanRangeCList));


static const InputChan FFMInputChanCList[] = {
	Chan1, Chan2, Chan3, Chan4, Chan13, Chan24, Chan12, Chan34, Chan1234,
};
static const InputChan FTMInputChanCList[] = {
	Chan1234, Chan34, Chan12,
};
typedef pair<FrameTransferMode, InputChanList> InputChanPair;
static const InputChanPair FTMInputChanListCList[] = {
	InputChanPair(FFM, InputChanList(C_LIST_ITERS(FFMInputChanCList))),
	InputChanPair(FTM, InputChanList(C_LIST_ITERS(FTMInputChanCList))),
};
FTMInputChanListMapType 
lima::Frelon::FTMInputChanListMap(C_LIST_ITERS(FTMInputChanListCList));


typedef pair<TimeUnitFactor, double> FactorPair;
static const FactorPair TimeUnitFactorCList[] = {
	FactorPair(Milliseconds, 1e-3),
	FactorPair(Microseconds, 1e-6),
};
TimeUnitFactorMapType 
lima::Frelon::TimeUnitFactorMap(C_LIST_ITERS(TimeUnitFactorCList));


const FrameDim lima::Frelon::MaxFrameDim(2048, 2048, Bpp16);


typedef pair<ChipType, double> ChipSizePair;
static const ChipSizePair ChipPixelSizeCList[] = {
	ChipSizePair(Atmel, 14e-6),
	ChipSizePair(Kodak, 24e-6),
	ChipSizePair(E2V,   15e-6),
};
ChipPixelSizeMapType 
lima::Frelon::ChipPixelSizeMap(C_LIST_ITERS(ChipPixelSizeCList));
