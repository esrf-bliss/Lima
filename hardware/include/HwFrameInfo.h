#ifndef HWFRAMEINFO_H
#define HWFRAMEINFO_H

#include "SizeUtils.h"
#include "Timestamp.h"
#include <ostream>

namespace lima
{

/*******************************************************************
 * \typedef HwFrameInfoType
 * \brief Structure containing information about acquired frame
 *
 *
 *******************************************************************/

typedef struct HwFrameInfo {
        enum OwnerShip {Managed,Transfer};

	int acq_frame_nb;
	void *frame_ptr;
	FrameDim frame_dim;
	Timestamp frame_timestamp;
	int valid_pixels;
        OwnerShip buffer_owner_ship;

	HwFrameInfo() 
		: acq_frame_nb(-1), frame_ptr(NULL), frame_dim(),
		  frame_timestamp(), valid_pixels(0), buffer_owner_ship(Managed) {}

	HwFrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
		    Timestamp timestamp, int pixels, OwnerShip owner);
  
        HwFrameInfo(const HwFrameInfo &anInfo);

	bool isValid() const;
} HwFrameInfoType;

std::ostream& operator <<(std::ostream& os, const HwFrameInfoType& info);

}

#endif // HWFRAMEINFO_H
