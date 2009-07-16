/*******************************************************************
 * @file FrameBuilder.h
 * @brief This file contains the FrameBuilder class for the Simulator
 *
 * @author A.Kirov
 *
 * @date 03/06/2009
 *******************************************************************/

#ifndef FRAMEBUILDER_H
#define FRAMEBUILDER_H

#include <vector>
#include "SizeUtils.h"
#include "Exceptions.h"

namespace lima {


struct GaussPeak {
	int x0, y0;     /// The center of the peak
	unsigned fwhm;  /// Full Width at Half Maximum
	double max;     /// The maximum
};


/*******************************************************************
 * @class FrameBuilder
 *
 *******************************************************************/
class FrameBuilder {

  public:
	FrameBuilder();
	FrameBuilder( FrameDim &frame_dim, Bin &bin, Roi &roi,
	              std::vector<struct GaussPeak> &peaks, double grow_factor );
	~FrameBuilder();

	void getFrameDim( FrameDim &dim ) const;
	void setFrameDim( const FrameDim &dim );

	void getBin( Bin &bin ) const;
	void setBin( const Bin &bin );
	void checkBin( Bin &bin ) const;

	void getRoi( Roi &roi ) const;
	void setRoi( const Roi &roi );
	void checkRoi( Roi &roi ) const;

	void getPeaks( std::vector<struct GaussPeak> &peaks ) const;
	void setPeaks( const std::vector<struct GaussPeak> &peaks );

	void getGrowFactor( double &grow_factor ) const;
	void setGrowFactor( const double &grow_factor );

	void getNextFrame( unsigned char *ptr ) throw (Exception);
	unsigned long getFrameNr();
	void resetFrameNr( int frame_nr=0 );

	void getMaxImageSize(Size& max_size);

  private:
	FrameDim m_frame_dim;                   /// Generated frame dimensions
	Bin m_bin;                              /// "Hardware" Bin
	Roi m_roi;                              /// "Hardware" RoI
	std::vector<struct GaussPeak> m_peaks;  /// Peaks to put in each frame
	double m_grow_factor;                   /// Peaks grow % with each frame

	unsigned long m_frame_nr;

	void checkValid( const FrameDim &frame_dim, const Bin &bin, 
	                 const Roi &roi ) throw(Exception);
	void checkPeaks( std::vector<struct GaussPeak> const &peaks );
	double dataXY( int x, int y );
	template <class depth> void fillData( unsigned char *ptr );
};


}

#endif /* FRAMEBUILDER_H */
