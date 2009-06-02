#include <vector>
#include <exception>
#include <iostream>

#include "FrameBuilder.h"
#include "BufferSave.h"
#include "SizeUtils.h"
#include "Exceptions.h"
#include "AutoObj.h"

using namespace std;
using namespace lima;

int main( void )
{
  try {

	FrameBuilder fb;

	FrameDim full_fd;
	fb.getFrameDim(full_fd);
	
	Bin bin = Bin(2,2);
	fb.setBin(bin);

	Roi roi = Roi(Point(128, 128), Point(384, 384));
	fb.setRoi(roi);

//	FrameDim fd = full_fd/bin;
	FrameDim fd = FrameDim(roi.getSize(), full_fd.getImageType());

	BufferSave bs(BufferSave::EDF, "test");
	bs.setTotFileFrames(1);

	int size = fd.getMemSize();
	AutoPtr<unsigned char, true> buffer;
	buffer = new unsigned char[size];

	Timestamp start = Timestamp::now();

	for( int i=0; i<10; i++ ) {
		int frame_nb = fb.getFrameNr();
		fb.getNextFrame( buffer );

		Timestamp t = Timestamp::now() - start;
		int pixels = Point(fd.getSize()).getArea();
		HwFrameInfoType finfo(frame_nb, buffer, &fd, t, pixels);
		bs.writeFrame(finfo);
	}

	return 0;

  } catch (Exception &e) {
  	cerr << e << endl;
	return -1;
  }
}
