#include "FrelonSerialLine.h"
#include "RegEx.h"

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
}

void SerialLine::write(const string& buffer, bool no_wait)
{
	map<MsgPart, string> msg_parts;
	splitMsg(buffer, msg_parts);
	const string& cmd = msg_parts[MsgCmd];

	m_multi_line_cmd = false;
	m_reset_cmd = (cmd == CmdStrMap[Reset]);
	if (!m_reset_cmd) {
		map<MultiLineCmd, string>::const_iterator it, end;
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

#include <iostream>
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

void SerialLine::getNumAvailBytes(int &avail)
{
	m_espia_ser_line.getNumAvailBytes(avail);
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
			  map<MsgPart, string>& msg_parts) const
{
	msg_parts.clear();

	RegEx re("^(?P<sync>>)?"
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
	static int key_len = sizeof(key_list) / sizeof(key_list[0]);

	const KeyPair *it, *end = key_list + key_len;
	for (it = key_list; it != end; ++it) {
		const MsgPart& key = it->first;
		const string&  grp = it->second;
		msg_parts[key] = string(match[grp].start, match[grp].end);
	}
}

