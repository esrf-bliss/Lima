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
	m_frame_dim = FrameDim(1024, 1024, Bpp32);
	m_bin = Bin(1,1);
	m_roi = Roi(0, Size(0,0));  // Or the whole frame?
	GaussPeak p={512, 512, 100, 100}; // in unbinned units!
	m_peaks.push_back(p);
	m_grow_factor = 1.00;
	m_frame_nr = 0;
}


/***************************************************************//**
 * @brief FrameBuilder class constructor setting member variables
 *
 * Before setting we check the values for consistency
 *
 * @param[in] frame_dim    The frame dimensions
 * @param[in] roi          RoI in BINNED units
 * @param[in] peaks        A vector of GaussPeak structures
 * @param[in] grow_factor  Peaks grow % with each frame
 *******************************************************************/
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
void FrameBuilder::checkPeaks( std::vector<struct GaussPeak> const &peaks )
{
	Size max_size;
	getMaxImageSize(max_size);
	Roi roi = Roi(0, max_size);
	
	vector<GaussPeak>::const_iterator p;
	for( p = peaks.begin( ); p != peaks.end( ); ++p ) {
		if( ! roi.containsPoint(Point(p->x0, p->y0)) )
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
void FrameBuilder::getPeaks( std::vector<struct GaussPeak> &peaks ) const
{
	peaks = m_peaks;
}


/***************************************************************//**
 * @brief Sets Gauss peaks
 *
 * @param[in] peaks  GaussPeak vector
 *******************************************************************/
void FrameBuilder::setPeaks( const std::vector<struct GaussPeak> &peaks )
{
	checkPeaks(peaks);

	m_peaks = peaks;
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
double gauss2D( double x, double y, double x0, double y0, double fwhm, double max )
{
	double sigma = SGM_FWHM * fwhm;
	return max * exp(-((x-x0)*(x-x0) + (y-y0)*(y-y0))/(2*sigma*sigma));
}


/***************************************************************//**
 * @brief Calculates the summary intensity at certain point
 *
 * @param[in] x  int X-coord
 * @param[in] y  int Y-coord
 * @return    intensity  double 
 *******************************************************************/
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
