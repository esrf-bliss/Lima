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



