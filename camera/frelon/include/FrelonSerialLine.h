//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef FRELONSERIALLINE_H
#define FRELONSERIALLINE_H

#include "Frelon.h"
#include "ThreadUtils.h"

namespace lima
{

namespace Frelon
{

class SerialLine : public HwSerialLine
{
	DEB_CLASS_NAMESPC(DebModCameraCom, "SerialLine", "Frelon");

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

	typedef std::map<MsgPart, std::string> MsgPartStrMapType;

	static const double TimeoutSingle, TimeoutNormal, TimeoutMultiLine, 
			    TimeoutReset;
	
	SerialLine(Espia::SerialLine& espia_ser_line);
	virtual ~SerialLine();

	Espia::SerialLine& getEspiaSerialLine();

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

	virtual void getNbAvailBytes(int &avail);
	
	virtual void setTimeout(double timeout);
	virtual void getTimeout(double& timeout) const;

	void splitMsg(const std::string& msg, 
		      MsgPartStrMapType& msg_parts) const;
	void decodeFmtResp(const std::string& ans, std::string& fmt_resp);

	void sendFmtCmd(const std::string& cmd, std::string& resp);

	void writeRegister(Reg reg, int  val);
	void readRegister (Reg reg, int& val);

	int getLastWarning();

	void clearCache();
	void setCacheActive(bool  cache_act);
	void getCacheActive(bool& cache_act);

 private:
	enum RegOp {
		None, DoCmd, ReadReg, WriteReg, DoReset, MultiRead
	};
	friend std::ostream& operator <<(std::ostream& os, RegOp op);

	typedef std::map<Reg, int> RegValMapType;

	AutoMutex lock(int mode);

	virtual void writeCmd(const std::string& buffer, 
			      bool no_wait = false);
	virtual void readResp(std::string& buffer, 
			      int max_len = MaxReadLen, 
			      double timeout = TimeoutDefault);

	class ReadRespCleanup {
	public:
		ReadRespCleanup(SerialLine& ser_line)
			: m_ser_line(ser_line)
		{}

		~ReadRespCleanup()
		{ m_ser_line.readRespCleanup(); }

	private:
		SerialLine& m_ser_line;
	};
	friend class RespCleanup;

	void readRespCleanup();

	bool isRegCacheable(Reg reg);
	bool getRegCacheVal(Reg reg, int& val);

	double getRegSleepTime(Reg reg);
 
	Espia::SerialLine& m_espia_ser_line;
	Cond m_cond;
	int m_last_warn;

	RegValMapType m_reg_cache;
	bool m_cache_act;
	RegOp m_curr_op;
	Reg m_curr_reg;
	bool m_curr_cache;
	std::string m_curr_resp;
	std::string m_curr_fmt_resp;
};

std::ostream& operator <<(std::ostream& os, SerialLine::RegOp op);


inline AutoMutex SerialLine::lock(int mode)
{
	return AutoMutex(m_cond.mutex(), mode);
}


} // namespace Frelon



} // namespace lima


#endif // FRELONSERIALLINE_H
