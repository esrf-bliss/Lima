#ifndef FRELON_H
#define FRELON_H

#include "EspiaSerialLine.h"
#include <string>
#include <map>

namespace lima
{

class Frelon
{
 public:
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

	enum Cmd {
		Reset,		Start,		Stop,		Save,
	};

	enum MultiLineCmd {
		Help,		Config,		Dac,		Volt,
		Aoi,
	};

	class SerialLine : public HwSerialLine
	{
	public:
		enum MsgPart {
			MsgSync, MsgCmd, MsgVal, MsgReq, MsgTerm, 
		};

		static const double TimeoutSingle, TimeoutNormal, TimeoutMax, 
				    TimeoutReset;

		SerialLine(EspiaSerialLine& espia_ser_line);
	
		virtual void write(const std::string& buffer, 
				   bool no_wait = false);
		virtual void read(std::string& buffer, int len, 
				  double timeout = TimeoutDefault);
		virtual void readStr(std::string& buffer, int len, 
				     double timeout = TimeoutDefault);

		virtual void getNumAvailBytes(int &avail);

		void splitMsg(const std::string& msg, 
			      std::map<MsgPart, std::string>& msg_parts) const;

	private:
		EspiaSerialLine& m_espia_ser_line;
		bool m_multi_line_cmd;
	};


	Frelon(EspiaSerialLine& espia_ser_line);


 private:
	SerialLine m_ser_line;
};


} // namespace lima

#endif // FRELON_H
