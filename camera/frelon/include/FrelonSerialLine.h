#ifndef FRELONSERIALLINE_H
#define FRELONSERIALLINE_H

#include "Frelon.h"

namespace lima
{

namespace Frelon
{

class SerialLine : public HwSerialLine
{
 public:
	enum MsgPart {
		MsgSync, MsgCmd, MsgVal, MsgReq, MsgTerm, 
	};
	
	static const double TimeoutSingle, TimeoutNormal, TimeoutMax, 
			    TimeoutReset;
	
	SerialLine(Espia::SerialLine& espia_ser_line);
	
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
	Espia::SerialLine& m_espia_ser_line;
	bool m_multi_line_cmd;
};


} // namespace Frelon



} // namespace lima


#endif // FRELONSERIALLINE_H
