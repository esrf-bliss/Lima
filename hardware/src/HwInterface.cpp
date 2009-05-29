#include "HwInterface.h"

using namespace lima;
using namespace std;

HwInterface::HwInterface()
{
}

HwInterface::~HwInterface()
{
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

ostream& lima::operator <<(ostream& os, 
			   const HwInterface::StatusType& status)
{
	return os << "<"
		  << "acq=" << status.acq << ", "
		  << "det=" << status.det 
		  << ">";
}



