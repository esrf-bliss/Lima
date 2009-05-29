#ifndef FRAMEBUILDER_H
#define FRAMEBUILDER_H

#include <vector>
#include "SizeUtils.h"
#include "Exceptions.h"

namespace lima {


struct GaussPeak {
	int x0, y0;
	unsigned fwhm;
	double max;
};


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

	void getRoi( Roi &roi ) const;
	void setRoi( const Roi &roi );

	void getPeaks( std::vector<struct GaussPeak> &peaks ) const;
	void setPeaks( const std::vector<struct GaussPeak> &peaks );

	void getNextFrame( unsigned char *ptr ) throw (Exception);
	unsigned long getFrameNr();
	void resetFrameNr( int frame_nr=0 );

	void getMaxImageSize(Size& max_size);

  private:
	FrameDim m_frame_dim;
	Bin m_bin;
	Roi m_roi;
	std::vector<struct GaussPeak> m_peaks;
	double m_grow_factor;

	unsigned long m_frame_nr;

	void checkValid( const FrameDim &frame_dim, const Bin &bin, 
	                 const Roi &roi ) throw(Exception);
	void checkPeaks( std::vector<struct GaussPeak> const &peaks );
	double dataXY( int x, int y );
	template <class depth> void fillData( unsigned char *ptr );
};


}

#endif /* FRAMEBUILDER_H */
