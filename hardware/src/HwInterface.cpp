#include "HwInterface.h"

using namespace lima;
using namespace std;

HwInterface::HwInterface()
{
	DEB_CONSTRUCTOR();
}

HwInterface::~HwInterface()
{
	DEB_DESTRUCTOR();
}

int HwInterface::getNbAcquiredFrames()
{
	DEB_MEMBER_FUNCT();

	HwBufferCtrlObj *buffer_ctrl;
	getHwCtrlObj(buffer_ctrl);
	int nb_acc_frames;
	buffer_ctrl->getNbAccFrames(nb_acc_frames);
	int nb_acq_frames =  getNbHwAcquiredFrames() / nb_acc_frames;

	DEB_RETURN() << DEB_VAR1(nb_acq_frames);
	return nb_acq_frames;
}

ostream& lima::operator <<(ostream& os,
			   HwInterface::AcqStatus acq_status)
{
	string name = "Unknown";
	switch (acq_status) {
	case HwInterface::AcqReady:	name = "AcqReady";	break;
	case HwInterface::AcqRunning:	name = "AcqRunning";	break;
	case HwInterface::AcqFault:	name = "AcqFault";	break;
	}
	return os << name;
}

void AddToken(string& str, const string& token, const string& sep)
{
	if (str.length() > 0)
		str += sep;
	str += token;
}

ostream& lima::operator <<(ostream& os, 
			   HwInterface::DetStatus det_status)
{
	if (det_status == HwInterface::DetIdle)
		return os << "Idle";

	string name, sep = "+";
	if (det_status & HwInterface::DetFault)
		AddToken(name, "DetFault", sep);
	if (det_status & HwInterface::WaitForTrigger)
		AddToken(name, "WaitForTrigger", sep);
	if (det_status & HwInterface::ShutterOpen)
		AddToken(name, "ShutterOpen", sep);
	if (det_status & HwInterface::Exposure)
		AddToken(name, "Exposure", sep);
	if (det_status & HwInterface::ShutterClose)
		AddToken(name, "ShutterClose", sep);
	if (det_status & HwInterface::ChargeShift)
		AddToken(name, "ChargeShift", sep);
	if (det_status & HwInterface::Readout)
		AddToken(name, "Readout", sep);
	if (det_status & HwInterface::Latency)
		AddToken(name, "Latency", sep);
	return os << name;
}

HwInterface::DetStatus lima::operator |(HwInterface::DetStatus s1,
					HwInterface::DetStatus s2)
{
	return HwInterface::DetStatus(int(s1) | int(s2));
}

HwInterface::DetStatus& lima::operator |=(HwInterface::DetStatus& s1,
					  HwInterface::DetStatus  s2)
{
	return s1 = s1 | s2;
}

ostream& lima::operator <<(ostream& os, 
			   const HwInterface::StatusType& status)
{
	return os << "<"
		  << "acq=" << status.acq << ", "
		  << "det=" << status.det 
		  << ">";
}

ostream& lima::operator <<(ostream& os,
			   HwInterface::ResetLevel reset_level)
{
	string name = "Unknown";
	switch (reset_level) {
	case HwInterface::SoftReset:	name = "SoftReset";	break;
	case HwInterface::HardReset:	name = "HardReset";	break;
	}
	return os << name;
}



