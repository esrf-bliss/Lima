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
	
	static const double TimeoutSingle, TimeoutNormal, TimeoutMultiLine, 
			    TimeoutReset;
	
	SerialLine(Espia::SerialLine& espia_ser_line);
	
	virtual void write(const std::string& buffer, 
			   bool no_wait = false);
	virtual void read(std::string& buffer, int max_len, 
			  double timeout = TimeoutDefault);
	virtual void readStr(std::string& buffer, int max_len, 
			     const std::string& term, 
			     double timeout = TimeoutDefault);
	virtual void readLine(std::string& buffer, int max_len, 
			      double timeout = TimeoutDefault);
	virtual void readSingleLine(std::string& buffer, int max_len, 
				    double timeout = TimeoutDefault);
	virtual void readMultiLine(std::string& buffer, int max_len);
	
	virtual void flush();

	virtual void getNumAvailBytes(int &avail);
	
	virtual void setTimeout(double timeout);
	virtual void getTimeout(double& timeout) const;

	void splitMsg(const std::string& msg, 
		      std::map<MsgPart, std::string>& msg_parts) const;

 private:
	Espia::SerialLine& m_espia_ser_line;
	bool m_multi_line_cmd;
	bool m_reset_cmd;
};


} // namespace Frelon



} // namespace lima


#endif // FRELONSERIALLINE_H
