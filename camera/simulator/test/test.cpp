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

	FrameDim fd = full_fd / bin;
	
	BufferSave bs(BufferSave::EDF, "boza");
	bs.setTotFileFrames(1);

	int size = fd.getMemSize();
	AutoPtr<unsigned char, true> buffer;
	buffer = new unsigned char[size];

	Timestamp start = Timestamp::now();

	for( int i=0; i<10; i++ ) {
		int frame_nb = fb.getFrameNr();
		fb.getNextFrame( buffer );

		Timestamp t = Timestamp::now() - start;
		FrameInfoType finfo(frame_nb, buffer, &fd, t);
		bs.writeFrame(finfo);
	}

	return 0;

  } catch (Exception &e) {
  	cout << "Exception: " << e.getErrDesc() << endl;
	return -1;
  }
}
