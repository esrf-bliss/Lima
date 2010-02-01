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
        enum OwnerShip {MANAGED,TRANSFERT};

	int acq_frame_nb;
	void *frame_ptr;
	const FrameDim *frame_dim;
	Timestamp frame_timestamp;
	int valid_pixels;
        OwnerShip owner_ship;

	HwFrameInfo() 
		: acq_frame_nb(-1), frame_ptr(NULL), frame_dim(NULL),
		  frame_timestamp(), valid_pixels(0), owner_ship(MANAGED) {}

	HwFrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
		    Timestamp timestamp, int pixels, OwnerShip owner = MANAGED) 
		: acq_frame_nb(frame_nb), frame_ptr(ptr), frame_dim(dim),
		  frame_timestamp(timestamp), valid_pixels(pixels),owner_ship(owner) {}

	bool isValid() const;
} HwFrameInfoType;

std::ostream& operator <<(std::ostream& os, const HwFrameInfoType& info);

}

#endif // HWFRAMEINFO_H
