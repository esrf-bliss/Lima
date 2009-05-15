#include <ctime>
#include <cmath>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include "FrameBuilder.h"

using namespace lima;
using namespace std;


FrameBuilder::FrameBuilder()
{
	bin_X = 1;
	bin_Y = 1;
	width = height = 1024;
	depth = 2;
	GaussPeak p={512, 512, 100, 1};
	peaks.push_back(p);

	this->_frame_nr = 0;
}


FrameBuilder::FrameBuilder( int bin_X, int bin_Y, 
                            int width, int height, int depth,
                            std::vector<struct GaussPeak> &peaks ):
	bin_X(bin_X), bin_Y(bin_Y), 
	width(width), height(height), depth(depth), 
	peaks(peaks)
{
	this->_frame_nr = 0;
}


FrameBuilder::~FrameBuilder()
{
}


void FrameBuilder::resetFrameNr(int frame_nr)
{
	this->_frame_nr = frame_nr;
}


unsigned long FrameBuilder::getCurrFrameNr()
{
	return this->_frame_nr;
}


#define SGM_FWHM 0.42466090014400952136075141705144  // 1/(2*sqrt(2*ln(2)))

double gauss2D( double x, double y, double x0, double y0, double fwhm, double max )
{
	double sigma = SGM_FWHM * fwhm;
	return max * exp(-((x-x0)*(x-x0) + (y-y0)*(y-y0))/(2*sigma*sigma));
}


double FrameBuilder::dataXY( int x, int y )
{
	double value=0.0;
	vector<GaussPeak>::iterator p;

	for( p = peaks.begin( ); p != peaks.end( ); ++p) {
		value += gauss2D(x, y, p->x0, p->y0, p->fwhm, p->max);
	}
	return value;
}


int FrameBuilder::writeFrameData(unsigned char *ptr)
{
	int x, y;
	double data;
	unsigned char *p1;
	unsigned short *p2;
	unsigned long *p4;
	

	switch( depth ) {
		case 1 :
			p1 = (unsigned char *) ptr;
			break;
		case 2 :
			p2 = (unsigned short *) ptr;
			break;
		case 4 :
			p4 = (unsigned long *) ptr;
			break;
		default:
			return -1;
	}

	for( y=0; y<height; y++ ) {
		for( x=0; x<width; x++ ) {
			data = dataXY(x, y);
			switch( depth ) {
				case 1 :
					*p1 = (unsigned char) (255.0 * data);
					p1++;
					break;
				case 2 :
					*p2 = (unsigned short) (65535.0 * data);
					p2++;
					break;
				case 4 :
					*p4 = (unsigned long) (4294967295.0 * data);
					p4++;
					break;
			}
		}
	}
	return 0;
}


void FrameBuilder::getNextFrame( unsigned char *ptr )
{
	int ret = writeFrameData( ptr );

	if( ret < 0 ) {
		// throw an exception
	} else {
		_frame_nr++;
	}
}
