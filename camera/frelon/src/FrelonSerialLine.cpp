#include "FrelonSerialLine.h"
#include "RegEx.h"
#include "MiscUtils.h"
#include <sstream>

using namespace lima;
using namespace lima::Frelon;
using namespace std;

const double SerialLine::TimeoutSingle    = 0.5;
const double SerialLine::TimeoutNormal    = 2.0;
const double SerialLine::TimeoutMultiLine = 3.0;
const double SerialLine::TimeoutReset     = 5.0;


SerialLine::SerialLine(Espia::SerialLine& espia_ser_line)
	: m_espia_ser_line(espia_ser_line)
{
	m_espia_ser_line.setLineTerm("\r\n");
	m_espia_ser_line.setTimeout(TimeoutNormal);

	m_multi_line_cmd = false;
	m_reset_cmd = false;
	m_last_warn = 0;

	flush();
}

Espia::SerialLine& SerialLine::getEspiaSerialLine()
{
	return m_espia_ser_line;
}

void SerialLine::write(const string& buffer, bool no_wait)
{
	MsgPartStrMapType msg_parts;
	splitMsg(buffer, msg_parts);
	const string& cmd = msg_parts[MsgCmd];

	m_multi_line_cmd = false;
	m_reset_cmd = (cmd == CmdStrMap[Reset]);
	if (!m_reset_cmd) {
		MultiLineCmdStrMapType::const_iterator it, end;
		end = MultiLineCmdStrMap.end();
		for (it = MultiLineCmdStrMap.begin(); it != end; ++it) {
			if (it->second == cmd) {
				m_multi_line_cmd = true;
				break;
			}
		}
	}

	string sync = !msg_parts[MsgSync].size() ? ">" : "";
	string term = !msg_parts[MsgTerm].size() ? "\r\n" : "";
	string msg = sync + buffer + term;

	m_espia_ser_line.write(msg, no_wait);
}

void SerialLine::read(string& buffer, int max_len, double timeout)
{
	m_espia_ser_line.read(buffer, max_len, timeout);
}

void SerialLine::readStr(string& buffer, int max_len, 
			 const string& term, double timeout)
{
	m_espia_ser_line.readStr(buffer, max_len, term, timeout);
}

void SerialLine::readLine(string& buffer, int max_len, double timeout)
{
	if ((timeout == TimeoutDefault) && (m_multi_line_cmd))
		readMultiLine(buffer, max_len);
	else
		readSingleLine(buffer, max_len, timeout);
}

void SerialLine::readSingleLine(string& buffer, int max_len, double timeout)
{
	if ((timeout == TimeoutDefault) && (m_reset_cmd)) {
		timeout = TimeoutReset;
		m_reset_cmd = false;
	}
	m_espia_ser_line.readLine(buffer, max_len, timeout);
}

void SerialLine::readMultiLine(string& buffer, int max_len)
{
	Timestamp timeout = Timestamp::now() + Timestamp(TimeoutMultiLine);

	buffer.clear();

	while (Timestamp::now() < timeout) {
		string ans;
		int len = max_len - buffer.size();
		try {
			m_espia_ser_line.readLine(ans, len, TimeoutSingle);
			buffer += ans;
		} catch (Exception e) {
			if (!buffer.empty())
				break;
		}
	}

	if (buffer.empty())
		throw LIMA_HW_EXC(Error, "Timeout reading Frelon multi line");
}

void SerialLine::flush()
{
	m_espia_ser_line.flush();
}

void SerialLine::getNbAvailBytes(int &avail)
{
	m_espia_ser_line.getNbAvailBytes(avail);
}

void SerialLine::setTimeout(double timeout)
{
	m_espia_ser_line.setTimeout(timeout);
}

void SerialLine::getTimeout(double& timeout) const
{
	m_espia_ser_line.getTimeout(timeout);
}


void SerialLine::splitMsg(const string& msg, 
			  MsgPartStrMapType& msg_parts) const
{
	msg_parts.clear();

	const static RegEx re("^(?P<sync>>)?"
			      "(?P<cmd>[A-Z]+)"
			      "((?P<req>\\?)|(?P<val>[0-9]+))?"
			      "(?P<term>[\r\n]+)?$");

	RegEx::FullNameMatchType match;
	if (!re.matchName(msg, match))
		throw LIMA_HW_EXC(InvalidValue, "Invalid Frelon message");

	typedef pair<MsgPart, string> KeyPair;
	static const KeyPair key_list[] = {
		KeyPair(MsgSync, "sync"), KeyPair(MsgCmd, "cmd"), 
		KeyPair(MsgVal,  "val"),  KeyPair(MsgReq, "req"),  
		KeyPair(MsgTerm, "term"),
	};
	const KeyPair *it, *end = C_LIST_END(key_list);
	for (it = key_list; it != end; ++it) {
		const MsgPart& key = it->first;
		const string&  grp = it->second;
		msg_parts[key] = string(match[grp].start, match[grp].end);
	}
}

void SerialLine::decodeFmtResp(const string& ans, string& fmt_resp)
{
	fmt_resp.clear();

	const static RegEx re("!(OK(:(?P<resp>[^\r]+))?|"
			        "W\a?:(?P<warn>[^\r]+)|"
			        "E\a?:(?P<err>[^\r]+))\r\n");

	RegEx::FullNameMatchType match;
	if (!re.matchName(ans, match))
		throw LIMA_HW_EXC(Error, "Invalid Frelon answer");

	RegEx::SingleMatchType& err = match["err"];
	if (err) {
		string err_str(err.start, err.end);
		string err_desc = string("Frelon Error: ") + err_str;
		throw LIMA_HW_EXC(Error, err_desc);
	}

	RegEx::SingleMatchType& warn = match["warn"];
	if (warn) {
		string warn_str(err.start, err.end);
		istringstream is(warn_str);
		is >> m_last_warn;
		return;
	}

	RegEx::SingleMatchType& resp = match["resp"];
	fmt_resp = string(resp.start, resp.end);
}

void SerialLine::sendFmtCmd(const string& cmd, string& resp)
{
	write(cmd);
	string ans;
	readLine(ans);
	decodeFmtResp(ans, resp);
}

int SerialLine::getLastWarning()
{
	int last_warn = m_last_warn;
	m_last_warn = 0;
	return last_warn;
}
