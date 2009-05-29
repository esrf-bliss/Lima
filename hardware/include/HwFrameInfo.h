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
	int acq_frame_nb;
	void *frame_ptr;
	const FrameDim *frame_dim;
	Timestamp frame_timestamp;
	int valid_pixels;

	HwFrameInfo() 
		: acq_frame_nb(-1), frame_ptr(NULL), frame_dim(NULL),
		  frame_timestamp(), valid_pixels(0) {}

	HwFrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
		    Timestamp timestamp, int pixels) 
		: acq_frame_nb(frame_nb), frame_ptr(ptr), frame_dim(dim),
		  frame_timestamp(timestamp), valid_pixels(pixels) {}
} HwFrameInfoType;

std::ostream& operator <<(std::ostream& os, const HwFrameInfoType& info);

}

#endif // HWFRAMEINFO_H
