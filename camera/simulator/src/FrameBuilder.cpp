#include <ctime>
#include <cmath>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include "FrameBuilder.h"
#include "SizeUtils.h"

using namespace lima;
using namespace std;


FrameBuilder::FrameBuilder()
{
	m_bin = Bin(1,1);
	m_frame_dim = FrameDim(1024, 1024, Bpp16);
	GaussPeak p={512, 512, 100, 1};
	m_peaks.push_back(p);
	m_grow_factor = 0.0;
	m_frame_nr = 0;
}


FrameBuilder::FrameBuilder( Bin &bin, FrameDim &frame_dim,
                            std::vector<struct GaussPeak> &peaks,
                            double grow_factor ):
	m_bin(bin), 
	m_frame_dim(frame_dim), 
	m_peaks(peaks), 
	m_grow_factor(grow_factor)
{
	m_frame_nr = 0;
}


FrameBuilder::~FrameBuilder()
{
}


void FrameBuilder::resetFrameNr(int frame_nr)
{
	m_frame_nr = frame_nr;
}


unsigned long FrameBuilder::getFrameNr()
{
	return m_frame_nr;
}

/*
FrameDim& FrameBuilder::getFrameDim() {
	return m_frame_dim;
}


void FrameBuilder::setFrameDim(FrameDim &frame_dim) {
	m_frame_dim = frame_dim;
}
*/

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

	for( p = m_peaks.begin( ); p != m_peaks.end( ); ++p) {
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
	

	switch( m_frame_dim.getDepth() ) {
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

	for( y=0; y<m_frame_dim.getSize().getHeight(); y++ ) {
		for( x=0; x<m_frame_dim.getSize().getWidth(); x++ ) {
			data = dataXY(x, y);
			switch( m_frame_dim.getDepth() ) {
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
		m_frame_nr++;
	}
}
