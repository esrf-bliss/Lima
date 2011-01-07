#include "HwFrameInfo.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

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

	int orig_prec = os.precision();
	os << "time_stamp=" << fixed << setprecision(6) 
	   << info.frame_timestamp << setprecision(orig_prec) << ", "
	   << "valid_pixels=" << info.valid_pixels << ", "
	   << "buffer_owner_ship=" << aBufferOwnerShipPt
	   << ">";

	return os;
}

HwFrameInfo::HwFrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
			 Timestamp timestamp, int pixels, OwnerShip owner) 
  : acq_frame_nb(frame_nb), frame_ptr(), frame_dim(),
    frame_timestamp(timestamp), valid_pixels(pixels),buffer_owner_ship(owner) 
{
  if(dim)
    frame_dim = *dim;
  if(owner == Transfer)
    {
      if(frame_dim.isValid())
	{
	  int size = frame_dim.getMemSize();
#ifdef __unix
	  if(posix_memalign(&frame_ptr,16,size))
#else  // Window
	  frame_ptr = _aligned_malloc(size,16);
	  if(!frame_ptr)
#endif
	    throw LIMA_HW_EXC(Error,"Memory allocation");
	  memcpy(frame_ptr,ptr,size);
	}
      else
	frame_ptr = NULL;
    }
  else
    frame_ptr = ptr;
}

HwFrameInfo::HwFrameInfo(const HwFrameInfo &aFrameInfo) :
  acq_frame_nb(aFrameInfo.acq_frame_nb),
  frame_ptr(aFrameInfo.frame_ptr),
  frame_dim(aFrameInfo.frame_dim),
  frame_timestamp(aFrameInfo.frame_timestamp),
  valid_pixels(aFrameInfo.valid_pixels),
  buffer_owner_ship(aFrameInfo.buffer_owner_ship)
{
  std::cout << "HwFrameInfo::HwFrameInfo copy" << std::endl;
}
