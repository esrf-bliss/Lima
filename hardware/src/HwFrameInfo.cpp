//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include "lima/HwFrameInfo.h"

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

	int orig_prec = int(os.precision());
	os << "time_stamp=" << fixed << setprecision(6) 
	   << info.frame_timestamp << setprecision(orig_prec) << ", "
	   << "valid_pixels=" << info.valid_pixels << ", "
	   << "buffer_owner_ship=" << aBufferOwnerShipPt;
	if (!info.sideband_data.empty()) {
		os << ", sideband_data=<";
		typedef sideband::DataContainer Container;
		Container::const_iterator it, end = info.sideband_data.end();
		const char *sep = "";
		for (it = info.sideband_data.begin(); it != end; ++it, sep = ", ")
			os << sep << it->first;
		os << ">";
	}
	os << ">";

	return os;
}

HwFrameInfo::HwFrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
			 Timestamp timestamp, int pixels, OwnerShip owner,
			 const sideband::DataContainer& sideband)
  : acq_frame_nb(frame_nb), frame_ptr(), frame_dim(),
    frame_timestamp(timestamp), valid_pixels(pixels),buffer_owner_ship(owner),
    sideband_data(sideband)
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
  buffer_owner_ship(aFrameInfo.buffer_owner_ship),
  sideband_data(aFrameInfo.sideband_data)
{
}
