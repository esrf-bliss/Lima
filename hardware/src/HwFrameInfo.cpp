#include "HwFrameInfo.h"
#include <iomanip>

using namespace lima;
using namespace std;


ostream& lima::operator <<(ostream& os, const HwFrameInfoType& info)
{
	os << "<"
	   << "acq_frame_nb=" << info.acq_frame_nb << ", "
	   << "frame_ptr=" << info.frame_ptr << ", "
	   << "frame_dim=" << *info.frame_dim << ", "
	   << "time_stamp=" << fixed << setprecision(6) 
	   << info.frame_timestamp << setprecision(0) << ", "
	   << "valid_pixels=" << info.valid_pixels 
	   << ">";

	return os;
}
