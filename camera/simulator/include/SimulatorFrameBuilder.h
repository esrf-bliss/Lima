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
 * @file FrameBuilder.h
 * @brief This file contains the FrameBuilder class for the Simulator
 *
 * @author A.Kirov
 * @date 03/06/2009
 *******************************************************************/

#ifndef FRAMEBUILDER_H
#define FRAMEBUILDER_H

#include <vector>
#include "SimulatorCompatibility.h"
#include "SizeUtils.h"
#include "Exceptions.h"

namespace lima {

namespace Simulator {

struct LIBSIMULATOR_API GaussPeak {
	double x0, y0;     /// The center of the peak
	double fwhm;       /// Full Width at Half Maximum
	double max;        /// The maximum value
	GaussPeak() 
		: x0(0), y0(0), fwhm(0)
	{max = 0;}
	GaussPeak(const GaussPeak& o) 
		: x0(o.x0), y0(o.y0), fwhm(o.fwhm)
	{max = o.max;}
	GaussPeak(double x, double y, double w, double m) 
		: x0(x), y0(y), fwhm(w)
	{max = m;}
};


/***************************************************************//**
 * @class FrameBuilder
 *
 * @brief This class configures and generates frames for the Simulator
 *
 *******************************************************************/
class LIBSIMULATOR_API FrameBuilder {

  public:
	enum FillType { 
		Gauss, Diffraction,
	};
	enum RotationAxis {
		Static, RotationX, RotationY,
	};

	typedef std::vector<struct GaussPeak> PeakList;

	FrameBuilder();
	FrameBuilder( FrameDim &frame_dim, Bin &bin, Roi &roi,
	              const PeakList &peaks, double grow_factor );
	~FrameBuilder();

	void getFrameDim( FrameDim &dim ) const;
	void setFrameDim( const FrameDim &dim );

	void getBin( Bin &bin ) const;
	void setBin( const Bin &bin );
	void checkBin( Bin &bin ) const;

	void getRoi( Roi &roi ) const;
	void setRoi( const Roi &roi );
	void checkRoi( Roi &roi ) const;

	void getPeaks( PeakList &peaks ) const;
	void setPeaks( const PeakList &peaks );

	void getPeakAngles( std::vector<double> &angles ) const;
	void setPeakAngles( const std::vector<double> &angles );
	
	void getFillType( FillType &fill_type ) const;
	void setFillType( FillType fill_type );

	void getRotationAxis( RotationAxis &rot_axis ) const;
	void setRotationAxis( RotationAxis rot_axis );

	void getRotationAngle( double &a ) const;
	void setRotationAngle( const double &a );
	
	void getRotationSpeed( double &s ) const;
	void setRotationSpeed( const double &s );
	
	void getGrowFactor( double &grow_factor ) const;
	void setGrowFactor( const double &grow_factor );

	void getDiffractionPos( double &x, double &y ) const;
	void setDiffractionPos( const double &x, const double &y );

	void getDiffractionSpeed( double &sx, double &sy ) const;
	void setDiffractionSpeed( const double &sx, const double &sy );

	void getNextFrame( unsigned char *ptr ) throw (Exception);
	unsigned long getFrameNr();
	void resetFrameNr( int frame_nr=0 );

	void getMaxImageSize(Size& max_size);
	
  private:
	FrameDim m_frame_dim;                   /// Generated frame dimensions
	Bin m_bin;                              /// "Hardware" Bin
	Roi m_roi;                              /// "Hardware" RoI
	PeakList m_peaks;  /// Peaks to put in each frame
	double m_grow_factor;                   /// Peaks grow % with each frame
	FillType m_fill_type;
	RotationAxis m_rot_axis;
	double m_rot_angle;
	double m_rot_speed;
	std::vector<double> m_peak_angles;

	double m_diffract_x;
	double m_diffract_y;
	double m_diffract_sx;
	double m_diffract_sy;

	unsigned long m_frame_nr;

	void init(FrameDim &frame_dim, Bin &bin, Roi &roi,
		  const PeakList &peaks, double grow_factor );

	void checkValid( const FrameDim &frame_dim, const Bin &bin, 
	                 const Roi &roi ) throw(Exception);
	void checkPeaks( PeakList const &peaks );
	double dataXY( const PeakList &peaks, int x, int y );
	double dataDiffract( double x, double y );
	template <class depth> void fillData( unsigned char *ptr );

	PeakList getGaussPeaksFrom3d(double angle);
	static double gauss2D( double x, double y, double x0, double y0, 
			       double fwhm, double max );
	
};


}

}

#endif /* FRAMEBUILDER_H */
