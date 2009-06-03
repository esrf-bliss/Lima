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
	m_frame_dim = FrameDim(1024, 1024, Bpp16);
	m_bin = Bin(1,1);
	m_roi = Roi(0, Size(0,0));  // Or the whole frame?
	GaussPeak p={512, 512, 100, 100}; // in unbinned units!
	m_peaks.push_back(p);
	m_grow_factor = 1.00;
	m_frame_nr = 0;
}


FrameBuilder::FrameBuilder( FrameDim &frame_dim, Bin &bin, Roi &roi,
                            vector<struct GaussPeak> &peaks,
                            double grow_factor ):
	m_grow_factor(grow_factor)
{
	checkValid(frame_dim, bin, roi);
	checkPeaks(peaks);

	m_frame_dim = frame_dim;
	m_bin = bin; 
	m_roi = roi;
	m_peaks = peaks;

	m_frame_nr = 0;
}


FrameBuilder::~FrameBuilder()
{
}


void FrameBuilder::checkValid( const FrameDim &frame_dim, const Bin &bin, 
                               const Roi &roi ) throw(Exception)
{
	Size max_size;
	getMaxImageSize( max_size );

	Bin valid_bin = bin;
	checkBin(valid_bin);
	if (valid_bin != bin)
		throw LIMA_HW_EXC(InvalidValue, "Invalid bin");

	if( (frame_dim.getSize().getWidth()  > max_size.getWidth()) ||
	    (frame_dim.getSize().getHeight() > max_size.getHeight()) )
		throw LIMA_HW_EXC(InvalidValue, "Frame size too big");

	FrameDim bin_dim = frame_dim / bin;

	if( roi.getSize() != 0 ) {
		bin_dim.checkValidRoi(roi);
	}
}


void FrameBuilder::checkPeaks( std::vector<struct GaussPeak> const &peaks )
{
	Size max_size;
	getMaxImageSize(max_size);
	Roi roi = Roi(0, max_size);
	
	vector<GaussPeak>::const_iterator p;
	for( p = peaks.begin( ); p != peaks.end( ); ++p ) {
		if( ! roi.containsPoint(Point(p->x0, p->y0)) )
			throw Exception( Hardware, InvalidValue, "Peak too far",
			                 __FILE__, __FUNCTION__, __LINE__ );
	}
	
}


void FrameBuilder::getFrameDim( FrameDim &dim ) const
{
	dim = m_frame_dim;
}


void FrameBuilder::setFrameDim( const FrameDim &dim )
{
	checkValid(dim, m_bin, m_roi);

	m_frame_dim = dim;

	// Reset Bin and RoI?
}


void FrameBuilder::getBin( Bin &bin ) const
{
	bin = m_bin;
}


void FrameBuilder::setBin( const Bin &bin )
{
	checkValid(m_frame_dim, bin, m_roi);

	m_bin = bin;
}


void FrameBuilder::checkBin( Bin &bin ) const
{
	if ((bin == Bin(1,1)) || (bin == Bin(1,2)) || (bin == Bin(2,1)))
		bin = Bin(1,1);
	else
		bin = Bin(2,2);
}


void FrameBuilder::getRoi( Roi &roi ) const
{
	roi = m_roi;
}


void FrameBuilder::setRoi( const Roi &roi )
{
	checkValid(m_frame_dim, m_bin, roi);

	m_roi = roi;
}


void FrameBuilder::checkRoi( Roi &roi ) const
{
	roi.alignCornersTo(8, Ceil);
}


void FrameBuilder::getPeaks( std::vector<struct GaussPeak> &peaks ) const
{
	peaks = m_peaks;
}


void FrameBuilder::setPeaks( const std::vector<struct GaussPeak> &peaks )
{
	checkPeaks(peaks);

	m_peaks = peaks;
}


void FrameBuilder::getGrowFactor( double &grow_factor ) const
{
	grow_factor = m_grow_factor;
}


void FrameBuilder::setGrowFactor( const double &grow_factor )
{
	// Any restrictions?
	m_grow_factor = grow_factor;
}


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
	int x, bx, bx0, bxM, y, by, by0, byM;
	int binX = m_bin.getX();
	int binY = m_bin.getY();
	int width = m_frame_dim.getSize().getWidth();
	int height = m_frame_dim.getSize().getHeight();
	depth *p = (depth *) ptr;
	double data, max;

	if( m_roi.getSize() != 0 ) {
		bx0 = m_roi.getTopLeft().x;
		bxM = m_roi.getBottomRight().x+1;
		by0 = m_roi.getTopLeft().y;
		byM = m_roi.getBottomRight().y+1;
	} else {
		bx0 = by0 = 0;
		bxM = width/binX;
		byM = height/binY;
	}

	max = (double) ((depth) -1);
	for( by=by0; by<byM; by++ ) {
		for( bx=bx0; bx<bxM; bx++ ) {
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


void FrameBuilder::getNextFrame( unsigned char *ptr ) throw (Exception)
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
			throw Exception( Hardware, NotSupported, "Depth",
			                 __FILE__, __FUNCTION__, __LINE__ );
	}
	++m_frame_nr;
}


void FrameBuilder::resetFrameNr( int frame_nr )
{
	m_frame_nr = frame_nr;
}


unsigned long FrameBuilder::getFrameNr()
{
	return m_frame_nr;
}


void FrameBuilder::getMaxImageSize(Size& max_size)
{
	int max_dim = 8 * 1024;
	max_size = Size(max_dim, max_dim);
}
