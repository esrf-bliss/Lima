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
	
	enum AnsPart {
		AnsResp, AnsWarn, AnsErr,
	};
	
	enum {
		MaxReadLen = 10000,
	};

	static const double TimeoutSingle, TimeoutNormal, TimeoutMultiLine, 
			    TimeoutReset;
	
	SerialLine(Espia::SerialLine& espia_ser_line);
	
	virtual void write(const std::string& buffer, 
			   bool no_wait = false);
	virtual void read(std::string& buffer, 
			  int max_len = MaxReadLen, 
			  double timeout = TimeoutDefault);
	virtual void readStr(std::string& buffer, int max_len, 
			     const std::string& term, 
			     double timeout = TimeoutDefault);
	virtual void readLine(std::string& buffer, 
			      int max_len = MaxReadLen, 
			      double timeout = TimeoutDefault);
	virtual void readSingleLine(std::string& buffer, 
				    int max_len = MaxReadLen, 
				    double timeout = TimeoutDefault);
	virtual void readMultiLine(std::string& buffer, 
				   int max_len = MaxReadLen);
	
	virtual void flush();

	virtual void getNumAvailBytes(int &avail);
	
	virtual void setTimeout(double timeout);
	virtual void getTimeout(double& timeout) const;

	void splitMsg(const std::string& msg, 
		      std::map<MsgPart, std::string>& msg_parts) const;
	void decodeFmtResp(const std::string& ans, std::string& fmt_resp);

	void sendFmtCmd(const std::string& cmd, std::string& resp);

	int getLastWarning();

 private:
	Espia::SerialLine& m_espia_ser_line;
	bool m_multi_line_cmd;
	bool m_reset_cmd;
	int m_last_warn;
};


} // namespace Frelon



} // namespace lima


#endif // FRELONSERIALLINE_H
