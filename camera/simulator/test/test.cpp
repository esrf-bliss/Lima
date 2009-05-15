#include <vector>
#include "FrameBuilder.h"
#include "BufferSave.h"

using namespace std;
using namespace lima;

int main( void )
{
	GaussPeak p[2]={{333, 512, 50, 1}, {666, 512, 100, 1}};
	vector<GaussPeak> pv( p, p + sizeof(p)/sizeof(*p) );
	FrameBuilder fb, fb2(1, 1, 1024, 1024, 2, pv);
	BufferSave bs("boza", FMT_EDF);
	FrameInfoType finfo;
	unsigned char *_buffer;


	_buffer = NULL;
	try {
		int size = fb.width*fb.height*fb.depth;
		_buffer = new unsigned char[size];
	} catch( bad_alloc& ) {
		/* ??? */
	}

	for( int i=0; i<5; i++ ) {
		fb.getNextFrame( _buffer );

		finfo.acq_frame_nb = fb.getCurrFrameNr();
		finfo.frame_ptr = _buffer;
		finfo.width =  fb.width;
		finfo.height = fb.height;
		finfo.depth = fb.depth;
		finfo.frame_time_stamp = 0; /* XXX */

		bs.writeFrame(finfo);
	}

	if( _buffer )
		delete[] _buffer;

	return 0;
}
