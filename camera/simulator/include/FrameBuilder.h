#ifndef FRAMEBUILDER_H
#define FRAMEBUILDER_H

#include <vector>

namespace lima {


struct GaussPeak {
	int x0, y0;
	unsigned fwhm;
	double max;
};


class FrameBuilder {

  public:
	int bin_X, bin_Y;
	int width, height, depth;
	std::vector<struct GaussPeak> peaks;
	double inc;
	

	FrameBuilder( int bin_X, int bin_Y, int width, int height, int depth,
	              std::vector<struct GaussPeak> &peaks );
	~FrameBuilder();

	void getNextFrame( unsigned char *ptr );
	void resetFrameNr( int frame_nr=0 );
	unsigned long getCurrFrameNr();

  private:
	unsigned long _frame_nr;

	int writeFrameData( unsigned char *ptr );
	double dataXY( int x, int y );
};


}

#endif /* FRAMEBUILDER_H */
