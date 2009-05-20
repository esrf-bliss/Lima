#ifndef FRAMEBUILDER_H
#define FRAMEBUILDER_H

#include <vector>
#include "SizeUtils.h"

namespace lima {


struct GaussPeak {
	int x0, y0;
	unsigned fwhm;
	double max;
};


class FrameBuilder {

  public:
	Bin m_bin;
	FrameDim m_frame_dim;


	FrameBuilder();
	FrameBuilder( Bin &bin, FrameDim &frame_dim,
	              std::vector<struct GaussPeak> &peaks, double grow_factor );
	~FrameBuilder();

	void getNextFrame( unsigned char *ptr );

	unsigned long getFrameNr();
	void resetFrameNr( int frame_nr=0 );

  private:
	std::vector<struct GaussPeak> m_peaks;
	double m_grow_factor;

	unsigned long m_frame_nr;


	int writeFrameData( unsigned char *ptr );
	double dataXY( int x, int y );
	template <class depth> void fillData( unsigned char *ptr );
};


}

#endif /* FRAMEBUILDER_H */
