#include "FrelonCamera.h"

#include <iostream>

using namespace lima;
using namespace std;

void print_str(const string& desc, const string& str)
{
	cout << desc << " \"" << str << "\"" << endl;
}

void split_msg(const Frelon::SerialLine& frelon_ser_line, const string& msg)
{
	map<Frelon::SerialLine::MsgPart, string> msg_parts;

	frelon_ser_line.splitMsg(msg, msg_parts);

	print_str("Msg",  msg);
	print_str("Sync", msg_parts[Frelon::SerialLine::MsgSync]);
	print_str("Cmd ", msg_parts[Frelon::SerialLine::MsgCmd]);
	print_str("Val ", msg_parts[Frelon::SerialLine::MsgVal]);
	print_str("Req ", msg_parts[Frelon::SerialLine::MsgReq]);
	print_str("Term", msg_parts[Frelon::SerialLine::MsgTerm]);
	cout << endl;
}

void frelon_cmd(Frelon::SerialLine& frelon_ser_line, const string& cmd)
{
	split_msg(frelon_ser_line, cmd);
	frelon_ser_line.write(cmd);

	Timestamp t0, t1;
	string ans;

	t0 = Timestamp::now();
	frelon_ser_line.readLine(ans);
	t1 = Timestamp::now();
	cout << "Elapsed " << (t1 - t0) << " sec" << endl;
	print_str("Ans", ans);
}

void frelon_read_reg(Frelon::Camera& frelon_cam, Frelon::Reg reg)
{
	Timestamp t0, t1;
	int val;

	print_str("Reg", Frelon::RegStrMap[reg]);
	t0 = Timestamp::now();
	frelon_cam.readRegister(reg, val);
	t1 = Timestamp::now();
	cout << "Elapsed " << (t1 - t0) << " sec" << endl;
	cout << "Val" << " " << val << endl;
}

void test_frelon(bool do_reset)
{
	Espia::Dev espia(0);
	Espia::SerialLine espia_ser_line(espia);
	Frelon::SerialLine frelon_ser_line(espia_ser_line);
	Frelon::Camera frelon_cam(espia_ser_line);

	string msg;

	if (do_reset) {
		cout << "Resetting camera ... " << endl;
		frelon_cam.hardReset();
		cout << "  Done!" << endl;
	}

	string ver;
	frelon_cam.getVersion(ver);
	print_str("Ver", ver);

	msg = ">C\r\n";
	frelon_cmd(frelon_ser_line, msg);

	frelon_read_reg(frelon_cam, Frelon::ExpTime);
	frelon_cam.writeRegister(Frelon::ExpTime, 200);
	frelon_read_reg(frelon_cam, Frelon::ExpTime);

	msg = ">C\r\n";
	frelon_cmd(frelon_ser_line, msg);

	frelon_cam.writeRegister(Frelon::ExpTime, 100);
	frelon_read_reg(frelon_cam, Frelon::ExpTime);
}

int main(int argc, char *argv[])
{
	
	try {
		bool do_reset = (argc > 1) && (string(argv[1]) == "reset");
		test_frelon(do_reset);
	} catch (Exception e) {
		cerr << "LIMA Exception: " << e << endl;
	}

	return 0;
}
