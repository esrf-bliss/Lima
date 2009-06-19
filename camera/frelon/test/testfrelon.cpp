#include "Frelon.h"

#include <iostream>

using namespace lima;
using namespace std;

typedef EspiaSerialLine    ESL;
typedef Frelon::SerialLine FSL;

void print_str(const string& desc, const string& str)
{
	cout << desc << " \"" << str << "\"" << endl;
}

void split_msg(const FSL& frelon_ser_line, const string& msg)
{
	map<FSL::MsgPart, string> msg_parts;

	frelon_ser_line.splitMsg(msg, msg_parts);

	print_str("Msg",  msg);
	print_str("Sync", msg_parts[FSL::MsgSync]);
	print_str("Cmd ", msg_parts[FSL::MsgCmd]);
	print_str("Val ", msg_parts[FSL::MsgVal]);
	print_str("Req ", msg_parts[FSL::MsgReq]);
	print_str("Term", msg_parts[FSL::MsgTerm]);
	cout << endl;
}

void test_frelon()
{
	EspiaDev espia(0);
	ESL espia_ser_line(espia);
	
	string msg, ans;

	espia_ser_line.setTimeout(FSL::TimeoutNormal);
	espia_ser_line.setLineTerm("\r\n");

	msg = ">C\r\n";
	espia_ser_line.write(msg);
	espia_ser_line.read(ans, 10000);
	print_str("Ans", ans);

	msg = ">I?\r\n";
	espia_ser_line.write(msg);
	espia_ser_line.readLine(ans, 10000, FSL::TimeoutSingle);
	print_str("Ans", ans);

	FSL frelon_ser_line(espia_ser_line);
	msg = ">C\r\n";
	split_msg(frelon_ser_line, msg);
	msg = ">I?\r\n";
	split_msg(frelon_ser_line, msg);
	msg = "N1000";
	split_msg(frelon_ser_line, msg);

}

int main(int argc, char *argv[])
{

	try {
		test_frelon();
	} catch (Exception e) {
		cerr << "LIMA Exception: " << e << endl;
	}

	return 0;
}
