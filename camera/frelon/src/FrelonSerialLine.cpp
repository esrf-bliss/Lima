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

	m_last_warn = 0;

	m_curr_op = None;
	m_curr_cache = false;
	m_cache_act = true;

	flush();
}

Espia::SerialLine& SerialLine::getEspiaSerialLine()
{
	return m_espia_ser_line;
}

void SerialLine::write(const string& buffer, bool no_wait)
{
	AutoMutex l = lock(AutoMutex::Locked);

	while (m_curr_op != None)
		m_cond.wait();

	writeCmd(buffer, no_wait);

	l.leaveLocked();
}

void SerialLine::writeCmd(const string& buffer, bool no_wait)
{
	MsgPartStrMapType msg_parts;
	splitMsg(buffer, msg_parts);
	const string& cmd = msg_parts[MsgCmd];

	if (cmd == CmdStrMap[Reset]) {
		m_curr_op = DoReset;
		m_reg_cache.clear();
	} else {
		MultiLineCmdStrMapType::const_iterator it, end;
		end = MultiLineCmdStrMap.end();
		it = FindMapValue(MultiLineCmdStrMap, cmd);
		if (it != end)
			m_curr_op = MultiRead;
	}

	bool reg_found = false;
	if (m_curr_op == None) {
		RegStrMapType::const_iterator it, end = RegStrMap.end();
		it = FindMapValue(RegStrMap, cmd);
		reg_found = (it != end);
		if (reg_found)
			m_curr_reg = it->first;
	}

	m_curr_cache = false;
	if (reg_found && isRegCacheable(m_curr_reg)) {
		bool is_req = !msg_parts[MsgReq].empty();
		m_curr_op = is_req ? ReadReg : WriteReg;
		const string& cache_val = m_reg_cache[m_curr_reg];
		if (is_req) {
			m_curr_resp = cache_val;
			m_curr_cache = !m_curr_resp.empty();
		} else {
			m_curr_resp = msg_parts[MsgVal];
			m_curr_cache = (m_curr_resp == cache_val);
		}
		if (m_curr_cache)
			return;
	}

	if (m_curr_op == None)
		m_curr_op = DoCmd;

	string sync = msg_parts[MsgSync].empty() ? ">" : "";
	string term = msg_parts[MsgTerm].empty() ? "\r\n" : "";
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
	AutoMutex l = lock(AutoMutex::PrevLocked);

	readResp(buffer, max_len, timeout);
}

void SerialLine::readResp(string& buffer, int max_len, double timeout)
{
	if (m_curr_op == None)
		throw LIMA_HW_EXC(Error, "readLine without previous write");

	if ((m_curr_op == MultiRead) && (timeout == TimeoutDefault))
		readMultiLine(buffer, max_len);
	else
		readSingleLine(buffer, max_len, timeout);

	m_curr_op = None;
	m_cond.signal();
}

void SerialLine::readSingleLine(string& buffer, int max_len, double timeout)
{
	bool is_req = (m_curr_op == ReadReg);
	if (m_curr_cache) {
		ostringstream os;
		os << "!OK";
		if (is_req)
			os << ":" << m_curr_resp;
		os << "\r\n";
		buffer = os.str();
		m_curr_fmt_resp = m_curr_resp;
		return;
	}

	if ((m_curr_op == DoReset) && (timeout == TimeoutDefault))
		timeout = TimeoutReset;
	m_espia_ser_line.readLine(buffer, max_len, timeout);

	decodeFmtResp(buffer, m_curr_fmt_resp);
	
	bool reg_op = ((m_curr_op == WriteReg) || (m_curr_op == ReadReg));
	if (!reg_op || !isRegCacheable(m_curr_reg))
		return;

	string& cache_val = m_reg_cache[m_curr_reg];
	cache_val = is_req ? m_curr_fmt_resp : m_curr_resp;
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

bool SerialLine::isRegCacheable(Reg reg)
{
	if (!m_cache_act)
		return false;

	const RegListType& list = NonCacheableRegList;
	return (find(list.begin(), list.end(), reg) == list.end());
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
	AutoMutex l = lock(AutoMutex::Locked);

	m_curr_fmt_resp.clear();

	writeCmd(cmd);
	string ans;
	readResp(ans);

	resp = m_curr_fmt_resp;
}

int SerialLine::getLastWarning()
{
	int last_warn = m_last_warn;
	m_last_warn = 0;
	return last_warn;
}

void SerialLine::clearCache()
{
	AutoMutex l = lock(AutoMutex::Locked);
	m_reg_cache.clear();
}

void SerialLine::setCacheActive(bool cache_act)
{
	AutoMutex l = lock(AutoMutex::Locked);
	if (cache_act && !m_cache_act)
		m_reg_cache.clear();
	m_cache_act = cache_act;
}

void SerialLine::getCacheActive(bool& cache_act)
{
	AutoMutex l = lock(AutoMutex::Locked);
	cache_act = m_cache_act;
}
