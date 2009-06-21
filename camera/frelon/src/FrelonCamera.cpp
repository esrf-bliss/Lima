#include "FrelonCamera.h"
#include "RegEx.h"
#include <sstream>

using namespace lima::Frelon;
using namespace std;

Camera::Camera(Espia::SerialLine& espia_ser_line)
	: m_ser_line(espia_ser_line)
{

}

SerialLine& Camera::getSerialLine()
{
	return m_ser_line;
}

void Camera::sendCmd(Cmd cmd)
{
	string resp;
	m_ser_line.sendFmtCmd(CmdStrMap[cmd], resp);
}

void Camera::writeRegister(Reg reg, int val)
{
	ostringstream cmd;
	cmd << RegStrMap[reg] << val;
	string resp;
	m_ser_line.sendFmtCmd(cmd.str(), resp);
}

void Camera::readRegister(Reg reg, int& val)
{
	string resp, cmd = RegStrMap[reg] + "?";
	m_ser_line.sendFmtCmd(cmd, resp);
	istringstream is(resp);
	is >> val;
}

void Camera::hardReset()
{
	sendCmd(Reset);
}

void Camera::getVersion(string& ver)
{
	string cmd = RegStrMap[Version] + "?";
	m_ser_line.sendFmtCmd(cmd, ver);
}
