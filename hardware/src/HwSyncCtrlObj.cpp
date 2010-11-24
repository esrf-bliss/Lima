#include "HwSyncCtrlObj.h"

using namespace lima;

HwSyncCtrlObj::HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl)
  : m_buffer_ctrl(buffer_ctrl),
    m_acq_mode(Single),
    m_valid_ranges_cb(NULL)
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

void HwSyncCtrlObj::registerValidRangesCallback(ValidRangesCallback *cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(cb, m_valid_ranges_cb);

  if(m_valid_ranges_cb)
    {
      DEB_ERROR() << "ValidRangesCallback already registered";
      throw LIMA_CTL_EXC(InvalidValue,"ValidRangesCallback already registered");
    }

  m_valid_ranges_cb = cb;
}

void HwSyncCtrlObj::unregisterValidRangesCallback(ValidRangesCallback *cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(cb, m_valid_ranges_cb);

  if(m_valid_ranges_cb != cb)
    {
      DEB_ERROR() << "ValidRangesCallback not registered";
      throw LIMA_CTL_EXC(InvalidValue,"ValidRangesCallback not registered");
    }

  m_valid_ranges_cb = NULL;
}

std::ostream& lima::operator<<(std::ostream& os,const HwSyncCtrlObj::ValidRangesType &range)
{
  return os << "<" 
	    << "min_exp_time=" << range.min_exp_time << ","
	    << "max_exp_time=" << range.max_exp_time << ","
	    << "min_lat_time=" << range.min_lat_time << ","
	    << "max_lat_time=" << range.max_lat_time << ">";
}
