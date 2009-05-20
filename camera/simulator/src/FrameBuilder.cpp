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
	GaussPeak p={512, 512, 100, 100};
	m_peaks.push_back(p);
	m_grow_factor = 1.00;
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
	double val=0.0;
	vector<GaussPeak>::iterator p;

	for( p = m_peaks.begin( ); p != m_peaks.end( ); ++p) {
		val += gauss2D(x, y, p->x0, p->y0, p->fwhm, p->max);
	}
	val = val + val*(m_grow_factor*m_frame_nr);
	return val;
}


template <class depth> 
void FrameBuilder::fillData( unsigned char *ptr )
{
	int x, bx, y, by;
	int binX = m_bin.getX();
	int binY = m_bin.getY();
	int width = m_frame_dim.getSize().getWidth();
	int height = m_frame_dim.getSize().getHeight();
	depth *p = (depth *) ptr;
	double data, max;


	max = (double) ((depth) -1);
	for( by=0; by<height/binY; by++ ) {
		for( bx=0; bx<width/binX; bx++ ) {
			data = 0.0;
			for( y=by*binY; y<by*binY+binY; y++ ) {
				for( x=bx*binX; x<bx*binX+binX; x++ ) {
					data += dataXY(x, y);
				}
			}
			if( data > max ) data = max;  // ???
			*p++ = (depth) data;
		}
	}
}


int FrameBuilder::writeFrameData(unsigned char *ptr)
{
	switch( m_frame_dim.getDepth() ) {
		case 1 :
			fillData<unsigned char>(ptr);
			break;
		case 2 :
			fillData<unsigned short>(ptr);
			break;
		case 4 :
			fillData<unsigned long>(ptr);
			break;
		default:
			return -1;
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
