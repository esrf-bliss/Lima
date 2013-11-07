//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
/***************************************************************//**
 * @file FrameBuilder.cpp
 * @brief This file contains the FrameBuilder class implementation
 *
 * @author A.Kirov
 * @date 03/06/2009
 *******************************************************************/

#include <ctime>
#include <cmath>
#include <vector>
#ifdef __unix
#include <sys/time.h>
#else
#include <time_compat.h>
#endif
#include <unistd.h>
#include "SimulatorFrameBuilder.h"
#include "SizeUtils.h"

using namespace lima;
using namespace lima::Simulator;
using namespace std;


/***************************************************************//**
 * @brief FrameBuilder class default constructor
 *
 *******************************************************************/
FrameBuilder::FrameBuilder()
{
	FrameDim frame_dim = FrameDim(1024, 1024, Bpp32);
	Bin bin = Bin(1,1);
	Roi roi = Roi(0, Size(0,0));  // Or the whole frame?
	GaussPeak p(512, 512, 100, 100); // in unbinned units!
	vector<struct GaussPeak> peaks(&p, &p + 1);
	double grow_factor = 1.00;

	init(frame_dim, bin, roi, peaks, grow_factor);
}


/***************************************************************//**
 * @brief FrameBuilder class constructor setting member variables
 *
 * Calls initialisation routine
 *
 * @param[in] frame_dim    The frame dimensions
 * @param[in] roi          RoI in BINNED units
 * @param[in] peaks        A vector of GaussPeak structures
 * @param[in] grow_factor  Peaks grow % with each frame
 *******************************************************************/
FrameBuilder::FrameBuilder( FrameDim &frame_dim, Bin &bin, Roi &roi,
                            const PeakList &peaks,
                            double grow_factor )
{
	init(frame_dim, bin, roi, peaks, grow_factor);
}


/***************************************************************//**
 * @brief FrameBuilder class initialiser setting member variables
 *
 * Before setting we check the values for consistency
 *
 * @param[in] frame_dim    The frame dimensions
 * @param[in] roi          RoI in BINNED units
 * @param[in] peaks        A vector of GaussPeak structures
 * @param[in] grow_factor  Peaks grow % with each frame
 *******************************************************************/
void FrameBuilder::init( FrameDim &frame_dim, Bin &bin, Roi &roi,
			 const PeakList &peaks,
			 double grow_factor )
{
	checkValid(frame_dim, bin, roi);
	setPeaks(peaks);

	m_frame_dim = frame_dim;
	m_bin = bin; 
	m_roi = roi;
	
	m_fill_type = Gauss;
	m_rot_axis = RotationY;

	m_grow_factor = grow_factor;
	m_frame_nr = 0;
	m_rot_angle = 0;
	m_rot_speed = 0;
	m_diffract_x = frame_dim.getSize().getWidth() / 2;
	m_diffract_y = frame_dim.getSize().getHeight() / 2;

	m_diffract_sx = 0;
	m_diffract_sy = 0;
}


/***************************************************************//**
 * @brief FrameBuilder class destructor
 *
 *******************************************************************/
FrameBuilder::~FrameBuilder()
{
}


/***************************************************************//**
 * @brief Checks the consistency of FrameDim, Bin and RoI
 *
 * First checks if Binning is valid
 * Then checks if FrameDim is inside of the MaxImageSize
 * Finally checks if the RoI is consistent with the binned frame dim
 *******************************************************************/
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


/***************************************************************//**
 * @brief Checks if Gauss peak centers are inside the MaxImageSize
 *
 *******************************************************************/
void FrameBuilder::checkPeaks( PeakList const &peaks )
{
	Size max_size;
	getMaxImageSize(max_size);
	Roi roi = Roi(0, max_size);
	
	vector<GaussPeak>::const_iterator p;
	for( p = peaks.begin( ); p != peaks.end( ); ++p ) {
		if( ! roi.containsPoint(Point(int(p->x0), int(p->y0))) )
			throw LIMA_HW_EXC(InvalidValue, "Peak too far");
	}
	
}


/***************************************************************//**
 * @brief Gets frame dimention
 *
 * @param[out] dim  FrameDim object reference
 *******************************************************************/
void FrameBuilder::getFrameDim( FrameDim &dim ) const
{
	dim = m_frame_dim;
}


/***************************************************************//**
 * @brief Sets frame dimention
 *
 * @param[in] dim  FrameDim object reference
 *******************************************************************/
void FrameBuilder::setFrameDim( const FrameDim &dim )
{
	checkValid(dim, m_bin, m_roi);

	m_frame_dim = dim;

	// Reset Bin and RoI?
}


/***************************************************************//**
 * @brief Gets the Binning
 *
 * @param[out] bin  Bin object reference
 *******************************************************************/
void FrameBuilder::getBin( Bin &bin ) const
{
	bin = m_bin;
}


/***************************************************************//**
 * @brief Sets the Binning
 *
 * @param[in] bin  Bin object reference
 *******************************************************************/
void FrameBuilder::setBin( const Bin &bin )
{
	checkValid(m_frame_dim, bin, m_roi);

	m_bin = bin;
}


/***************************************************************//**
 * @brief Returns the closest Binning supported by the "hardware"
 *
 * @param[in,out] bin  Bin object reference
 *******************************************************************/
void FrameBuilder::checkBin( Bin &bin ) const
{
	if ((bin == Bin(1,1)) || (bin == Bin(1,2)) || (bin == Bin(2,1)))
		bin = Bin(1,1);
	else
		bin = Bin(2,2);
}


/***************************************************************//**
 * @brief Gets the RoI
 *
 * @param[out] roi  Roi object reference
 *******************************************************************/
void FrameBuilder::getRoi( Roi &roi ) const
{
	roi = m_roi;
}


/***************************************************************//**
 * @brief Sets the RoI
 *
 * @param[in] roi  Roi object reference
 *******************************************************************/
void FrameBuilder::setRoi( const Roi &roi )
{
	checkValid(m_frame_dim, m_bin, roi);

	m_roi = roi;
}


/***************************************************************//**
 * @brief Returns the closest RoI supported by the "hardware"
 *
 * @param[out] roi  Roi object reference
 *******************************************************************/
void FrameBuilder::checkRoi( Roi &roi ) const
{
	roi.alignCornersTo(8, Ceil);
}


/***************************************************************//**
 * @brief Gets the configured Gauss peaks vector
 *
 * @param[out] peaks  GaussPeak vector
 *******************************************************************/
void FrameBuilder::getPeaks( PeakList &peaks ) const
{
	peaks = m_peaks;
}


/***************************************************************//**
 * @brief Sets Gauss peaks
 *
 * @param[in] peaks  GaussPeak vector
 *******************************************************************/
void FrameBuilder::setPeaks( const PeakList &peaks )
{
	checkPeaks(peaks);

	m_peaks = peaks;
	while (m_peak_angles.size() < m_peaks.size())
		m_peak_angles.push_back(0);
}

/***************************************************************//**
 * @brief Gets peak angles
 *
 * @param[in] angles  double vector
 *******************************************************************/
void FrameBuilder::getPeakAngles( std::vector<double> &angles ) const
{
	angles = m_peak_angles;
}

/***************************************************************//**
 * @brief Sets peak angles
 *
 * @param[in] angles  double vector
 *******************************************************************/
void FrameBuilder::setPeakAngles( const std::vector<double> &angles )
{
	m_peak_angles = angles;
}

/***************************************************************//**
 * @brief Gets the image filling type
 *
 * @param[in] fill_type  FillType
 *******************************************************************/
void FrameBuilder::getFillType( FillType &fill_type ) const
{
	fill_type = m_fill_type;
}

/***************************************************************//**
 * @brief Gets the image filling type
 *
 * @param[in] fill_type  FillType
 *******************************************************************/
void FrameBuilder::setFillType( FillType fill_type )
{
	m_fill_type = fill_type;
}

/***************************************************************//**
 * @brief Gets the rotation axis policy
 *
 * @param[in] rot_axis  RotationAxis
 *******************************************************************/
void FrameBuilder::getRotationAxis( RotationAxis &rot_axis ) const
{
	rot_axis = m_rot_axis;
}

/***************************************************************//**
 * @brief Gets the rotation axis policy
 *
 * @param[in] rot_axis  RotationAxis
 *******************************************************************/
void FrameBuilder::setRotationAxis( RotationAxis rot_axis )
{
	m_rot_axis = rot_axis;
}

/***************************************************************//**
 * @brief Gets the peak rotation angle
 *
 * @param[in] a  Rotation angle (deg)
 *******************************************************************/
void FrameBuilder::getRotationAngle( double &a ) const
{
	a = m_rot_angle;
}

/***************************************************************//**
 * @brief Sets the peak rotation angle
 *
 * @param[in] a  Rotation angle (deg)
 *******************************************************************/
void FrameBuilder::setRotationAngle( const double &a )
{
	m_rot_angle = a;
}

/***************************************************************//**
 * @brief Gets the peak rotation speed
 *
 * @param[in] s  Rotation speed (deg / frame)
 *******************************************************************/
void FrameBuilder::getRotationSpeed( double &s ) const
{
	s = m_rot_speed;
}

/***************************************************************//**
 * @brief Sets the peak rotation speed
 *
 * @param[in] s  Rotation speed (deg / frame)
 *******************************************************************/
void FrameBuilder::setRotationSpeed( const double &s )
{
	m_rot_speed = s;
}
	
/***************************************************************//**
 * @brief Gets the configured peaks grow factor
 *
 * @param[out] grow_factor  a double
 *******************************************************************/
void FrameBuilder::getGrowFactor( double &grow_factor ) const
{
	grow_factor = m_grow_factor;
}


/***************************************************************//**
 * @brief Sets the peaks grow factor
 *
 * @param[in] grow_factor  a double
 *******************************************************************/
void FrameBuilder::setGrowFactor( const double &grow_factor )
{
	// Any restrictions?
	m_grow_factor = grow_factor;
}

/***************************************************************//**
 * @brief Gets the source displacement for diffraction
 *
 * @param[out] x, y  positions (double)
 *******************************************************************/
void FrameBuilder::getDiffractionPos( double &x, double &y ) const
{
	x = m_diffract_x;
	y = m_diffract_y;
}

/***************************************************************//**
 * @brief Gets the source displacement for diffraction
 *
 * @param[out] x, y  positions (double)
 *******************************************************************/
void FrameBuilder::setDiffractionPos( const double &x, const double &y )
{
	m_diffract_x = x;
	m_diffract_y = y;
}

/***************************************************************//**
 * @brief Gets the source displacement speed for diffraction
 *
 * @param[out] sx, sy  x and y speeds (double)
 *******************************************************************/
void FrameBuilder::getDiffractionSpeed( double &sx, double &sy ) const
{
	sx = m_diffract_sx;
	sy = m_diffract_sy;
}

/***************************************************************//**
 * @brief Gets the source displacement speed for diffraction
 *
 * @param[out] sx, sy  x and y speeds (double)
 *******************************************************************/
void FrameBuilder::setDiffractionSpeed( const double &sx, const double &sy )
{
	m_diffract_sx = sx;
	m_diffract_sy = sy;
}


#define SGM_FWHM 0.42466090014400952136075141705144  // 1/(2*sqrt(2*ln(2)))

/***************************************************************//**
 * @brief Calculates Gauss(x,y) for given peak parameters
 *
 * @param[in] x     double X-coord
 * @param[in] y     double Y-coord
 * @param[in] x0    double X-coord of the center
 * @param[in] y0    double Y-coord of the center
 * @param[in] fwhm  double Full Width at Half Maximum
 * @param[in] max   double the central maximum value
 * @return Gauss(x,y) double 
 *******************************************************************/
double FrameBuilder::gauss2D( double x, double y, double x0, double y0, 
			      double fwhm, double max )
{
	double sigma = SGM_FWHM * fabs(fwhm);
	return max * exp(-((x-x0)*(x-x0) + (y-y0)*(y-y0))/(2*sigma*sigma));
}



FrameBuilder::PeakList FrameBuilder::getGaussPeaksFrom3d(double angle)
{
        PeakList gauss_peaks;
        PeakList::iterator pit, pend = m_peaks.end();
        std::vector<double>::iterator ait, aend = m_peak_angles.end();
        
	int rot_y = (m_rot_axis == RotationY);
	Size size = m_frame_dim.getSize();
	int dim = rot_y ? size.getWidth() : size.getHeight();
	int center = dim / 2;
        for (pit = m_peaks.begin(), ait = m_peak_angles.begin(); 
	     (pit != pend) && (ait != aend); ++pit, ++ait) {
                double rad = (angle + *ait) * M_PI / 180;
                double x = pit->x0;
                double y = pit->y0;
		double *coord = rot_y ? &x : &y;
		double r = abs(center - *coord);
		if (center > *coord)
			rad += M_PI;
		*coord = center + int(r * cos(rad));
                GaussPeak peak(x, y, pit->fwhm, pit->max);
                gauss_peaks.push_back(peak);
        }

        return gauss_peaks;
}

/***************************************************************//**
 * @brief Calculates the summary intensity at certain point
 *
 * @param[in] x  int X-coord
 * @param[in] y  int Y-coord
 * @return    intensity  double 
 *******************************************************************/
double FrameBuilder::dataDiffract( double x, double y )
{
	x -= m_frame_dim.getSize().getWidth() / 2;
	y -= m_frame_dim.getSize().getHeight() / 2;
	double r = sqrt(pow(x, 2.0) + pow(y, 2.0));
	double w = (2 * M_PI / 100 * (0.5 + r / 500));
	double ar = (r >= 300) ? (r - 300) : 0;
	double a = exp(-pow(ar / 500, 4.0)) / (r / 5000 + 0.1);
	return a * pow(cos(r * w), 20.0);
}

/***************************************************************//**
 * @brief Calculates the summary intensity at certain point
 *
 * @param[in] x  int X-coord
 * @param[in] y  int Y-coord
 * @return    intensity  double 
 *******************************************************************/
double FrameBuilder::dataXY( const PeakList &peaks, int x, int y )
{
	double val=0.0;
	PeakList::const_iterator p;

	double gx, gy;

	if (m_fill_type == Gauss) {
		gx = x;
		gy = y;
	} else {
		gx = m_diffract_x + m_frame_nr * m_diffract_sx;
		gy = m_diffract_y + m_frame_nr * m_diffract_sy;
	}

	for( p = peaks.begin( ); p != peaks.end( ); ++p ) {
		val += gauss2D(gx, gy, p->x0, p->y0, p->fwhm, p->max);
	}
	val *= (1 + m_grow_factor * m_frame_nr);

	if (m_fill_type == Diffraction) 
		val *= dataDiffract(x, y);

	return val;
}


/***************************************************************//**
 * @brief Calculates and writes the "image" into the buffer
 *
 * This function also applies the "hardware" binning
 *
 * @todo Support more depths, not only 1, 2, and 4 bytes
 * @param[in] ptr  an (unsigned char) pointer to an allocated buffer
 *******************************************************************/
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
        
        double rot_angle = m_rot_angle + m_rot_speed * m_frame_nr;
        PeakList peaks = getGaussPeaksFrom3d(rot_angle);

	max = (double) ((depth) -1);
	for( by=by0; by<byM; by++ ) {
		for( bx=bx0; bx<bxM; bx++ ) {
			data = 0.0;
			for( y=by*binY; y<by*binY+binY; y++ ) {
				for( x=bx*binX; x<bx*binX+binX; x++ ) {
					data += dataXY(peaks, x, y);
				}
			}
			if( data > max ) data = max;  // ???
			*p++ = (depth) data;
		}
	}
}


/***************************************************************//**
 * @brief Fills the next frame into the buffer
 *
 * @param[in] ptr  an (unsigned char) pointer to an allocated buffer
 *
 * @exception lima::Exception  The image depth is not 1,2 or 4
 *******************************************************************/
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
			fillData<unsigned int>(ptr);
			break;
		default:
			throw LIMA_HW_EXC(NotSupported, "Invalid depth");
	}
	++m_frame_nr;
}


/***************************************************************//**
 * @brief Sets the internal frame number to a value. Default is 0.
 *
 * @param[in] frame_nr  int  The frame number, or nothing
 *******************************************************************/
void FrameBuilder::resetFrameNr( int frame_nr )
{
	m_frame_nr = frame_nr;
}


/***************************************************************//**
 * @brief Gets the internal frame number
 *
 * @return  unsigned long  The frame number.
 *******************************************************************/
unsigned long FrameBuilder::getFrameNr()
{
	return m_frame_nr;
}


/***************************************************************//**
 * @brief Gets the maximum "hardware" image size
 *
 * @param[out]  max_size  Reference to a Size object
 *******************************************************************/
void FrameBuilder::getMaxImageSize(Size& max_size)
{
	int max_dim = 1024;
	max_size = Size(max_dim, max_dim);
}
