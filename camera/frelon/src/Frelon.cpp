#include "Frelon.h"
#include "RegEx.h"

using namespace lima;
using namespace std;

#define FRELON_SINGLE_SER_TIMEOUT	0.5
#define FRELON_NORM_SER_TIMEOUT		1.5
#define FRELON_MAX_SER_TIMEOUT		2.0
#define FRELON_RESET_SER_TIMEOUT	5.0

Frelon::SerialLine::SerialLine(EspiaSerialLine& espia_ser_line)
	: m_espia_ser_line(espia_ser_line)
{
	m_espia_ser_line.setLineTerm("\r\n");
	m_espia_ser_line.setTimeout(FRELON_NORM_SER_TIMEOUT);
	m_multi_line_cmd = true;
}

void Frelon::SerialLine::splitMsg(const string& msg, 
				  map<MsgPart, string>& msg_parts)
{
	msg_parts.clear();

	RegEx re("^(?P<pre>>)?"
		 "(?P<cmd>[A-Z]+)"
		 "((?P<req>\\?)|(?P<val>[0-9]+))?"
		 "(?P<term>[\r\n]+)?$");

	RegEx::FullNameMatchType match;
	if (!re.matchName(msg, match))
		throw LIMA_HW_EXC(InvalidValue, "Invalid Frelon message");



}


Frelon::Frelon(EspiaSerialLine& espia_ser_line)
	: m_ser_line(espia_ser_line)
{

}

