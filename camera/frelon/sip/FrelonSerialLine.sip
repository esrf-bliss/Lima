
namespace Frelon
{

class SerialLine : HwSerialLine
{

%TypeHeaderCode
#include "FrelonSerialLine.h"
#include "RegEx.h"
#include "MiscUtils.h"
#include <sstream>
%End

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

//	typedef std::map<MsgPart, std::string> MsgPartStrMapType;

//	static const double TimeoutSingle, TimeoutNormal, TimeoutMultiLine, 
//			    TimeoutReset;
	
	SerialLine(Espia::SerialLine& espia_ser_line);
	
	Espia::SerialLine& getEspiaSerialLine();

	virtual void write(const std::string& buffer, 
			   bool no_wait = false);
	virtual void read(std::string& buffer /Out/, 
			  int max_len = MaxReadLen, 
			  double timeout = HwSerialLine::TimeoutDefault);
	virtual void readStr(std::string& buffer /Out/, int max_len, 
			     const std::string& term, 
			     double timeout = HwSerialLine::TimeoutDefault);
	virtual void readLine(std::string& buffer /Out/, 
			      int max_len = MaxReadLen, 
			      double timeout = HwSerialLine::TimeoutDefault);
	virtual void readSingleLine(std::string& buffer /Out/, 
				    int max_len = MaxReadLen, 
				 double timeout = HwSerialLine::TimeoutDefault);
	virtual void readMultiLine(std::string& buffer /Out/, 
				   int max_len = MaxReadLen);
	
	virtual void flush();

	virtual void getNbAvailBytes(int &avail /Out/);
	
	virtual void setTimeout(double timeout);
	virtual void getTimeout(double& timeout /Out/) const;

//	void splitMsg(const std::string& msg, 
//		      MsgPartStrMapType& msg_parts) const;
	void decodeFmtResp(const std::string& ans, std::string& fmt_resp /Out/);

	void sendFmtCmd(const std::string& cmd, std::string& resp /Out/);

	int getLastWarning();

};


}; // namespace Frelon
