#include "HwFrameInfo.h"
#include <iomanip>

using namespace lima;
using namespace std;

bool HwFrameInfo::isValid() const
{
  return (acq_frame_nb >= 0) && frame_ptr && frame_dim.isValid() && 
    frame_timestamp.isSet() && (valid_pixels > 0);
}

ostream& lima::operator <<(ostream& os, const HwFrameInfoType& info)
{
        const char *aBufferOwnerShipPt = 
        info.buffer_owner_ship == HwFrameInfoType::Managed ? "Managed" : "Transfer";

	os << "<"
	   << "acq_frame_nb=" << info.acq_frame_nb << ", "
	   << "frame_ptr=" << info.frame_ptr << ", ";
	if(info.frame_dim.isValid())	
	   os << "frame_dim=" << info.frame_dim << ", ";

	os << "time_stamp=" << fixed << setprecision(6) 
	   << info.frame_timestamp << setprecision(0) << ", "
	   << "valid_pixels=" << info.valid_pixels << ", "
	   << "buffer_owner_ship=" << aBufferOwnerShipPt
	   << ">";

	return os;
}
