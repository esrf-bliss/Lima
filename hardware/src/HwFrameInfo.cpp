#include "HwFrameInfo.h"

using namespace lima;
using namespace std;


ostream& lima::operator <<(ostream& os, const HwFrameInfoType& info)
{
	os << "<"
	   << "acq_frame_nb=" << info.acq_frame_nb << ", "
	   << "frame_ptr=" << info.frame_ptr << ", "
	   << "frame_dim=" << *info.frame_dim << ", "
	   << "time_stamp=" << info.frame_timestamp << ", "
	   << "valid_pixels=" << info.valid_pixels 
	   << ">";

	return os;
}
