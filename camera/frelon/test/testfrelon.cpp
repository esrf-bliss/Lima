#include "FrelonCamera.h"

#include <iostream>

using namespace lima;
using namespace std;

typedef Espia::SerialLine  ESL;
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

void frelon_cmd(FSL& frelon_ser_line, const string& cmd)
{
	split_msg(frelon_ser_line, cmd);
	frelon_ser_line.write(cmd);

	Timestamp t0, t1;
	string ans;
	int max_len = 10000;

	t0 = Timestamp::now();
	frelon_ser_line.readLine(ans, max_len);
	t1 = Timestamp::now();
	cout << "Elapsed " << (t1 - t0) << " sec" << endl;
	print_str("Ans", ans);
}

void test_frelon()
{
	Espia::Dev espia(0);
	ESL espia_ser_line(espia);
	FSL frelon_ser_line(espia_ser_line);
	
	frelon_ser_line.flush();

	string msg;

	msg = "RST";
	frelon_cmd(frelon_ser_line, msg);

	msg = ">C\r\n";
	frelon_cmd(frelon_ser_line, msg);

	msg = ">I?\r\n";
	frelon_cmd(frelon_ser_line, msg);

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
