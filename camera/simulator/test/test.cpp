#include <vector>
#include <exception>
#include "FrameBuilder.h"
#include "BufferSave.h"
#include "SizeUtils.h"
#include "Exceptions.h"

using namespace std;
using namespace lima;

int main( void )
{
	FrameBuilder fb;

	Bin bin = Bin(2,2);
	fb.setBin(bin);
	int binX = bin.getX();
	int binY = bin.getY();

	FrameDim fd;
	fb.getFrameDim(fd);
	int width = fd.getSize().getWidth();
	int height = fd.getSize().getHeight();
	int depth = fd.getDepth();

	BufferSave bs("boza", FMT_EDF);
	unsigned char *buffer;
	FrameInfoType finfo;


	buffer = NULL;
	try {
		int size = width * height * depth;
		buffer = new unsigned char[size];
	} catch( bad_alloc& ) {
		goto end;
	}

	for( int i=0; i<10; i++ ) {
		fb.getNextFrame( buffer );

		finfo.acq_frame_nb = fb.getFrameNr();
		finfo.frame_ptr = buffer;
		finfo.width = width/binX;
		finfo.height = height/binY;
		finfo.depth = depth;
		finfo.frame_time_stamp = 0; /* XXX */

		bs.writeFrame(finfo);
	}
 end:
	if( buffer )
		delete[] buffer;

	return 0;
}
