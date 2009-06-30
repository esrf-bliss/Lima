struct HwFrameInfoType
 {
%TypeHeaderCode
#include <HwFrameInfo.h>
using namespace lima;
%End
 int acq_frame_nb;
 void *frame_ptr;
 const FrameDim *frame_dim;
 Timestamp frame_timestamp;
 int valid_pixels;

 HwFrameInfoType();
};
