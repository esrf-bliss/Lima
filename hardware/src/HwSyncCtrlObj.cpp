#include "HwSyncCtrlObj.h"

using namespace lima;

HwSyncCtrlObj::HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl)
	: m_buffer_ctrl(buffer_ctrl)
{
	DEB_CONSTRUCTOR();
}

HwSyncCtrlObj::~HwSyncCtrlObj()
{
	DEB_DESTRUCTOR();
}

void HwSyncCtrlObj::setNbFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);

	setNbHwFrames(nb_frames);
}

void HwSyncCtrlObj::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	getNbHwFrames(nb_frames);
	DEB_RETURN() << DEB_VAR1(nb_frames);
}


std::ostream& lima::operator<<(std::ostream& os,const HwSyncCtrlObj::ValidRangesType &range)
{
  return os << "<" 
	    << "min_exp_time=" << range.min_exp_time << ","
	    << "max_exp_time=" << range.max_exp_time << ","
	    << "min_lat_time=" << range.min_lat_time << ","
	    << "max_lat_time=" << range.max_lat_time << ">";
}
