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
	BufferSave bs("boza", FMT_EDF);
	FrameInfoType finfo;
	unsigned char *buffer;
	FrameDim fd = fb.m_frame_dim;


	buffer = NULL;
	try {
		int size = fd.getSize().getWidth()*
		           fd.getSize().getHeight()*
		           fd.getDepth();
		buffer = new unsigned char[size];
	} catch( bad_alloc& ) {
		goto end;
	}

	for( int i=0; i<5; i++ ) {
		fb.getNextFrame( buffer );

		finfo.acq_frame_nb = fb.getFrameNr();
		finfo.frame_ptr = buffer;
		finfo.width =  fd.getSize().getWidth();
		finfo.height = fd.getSize().getHeight();
		finfo.depth = fd.getDepth();
		finfo.frame_time_stamp = 0; /* XXX */

		bs.writeFrame(finfo);
	}
 end:
	if( buffer )
		delete[] buffer;

	return 0;
}
