#include "SizeUtils.h"

using namespace lima;

/*******************************************************************
 * \brief Aligns the Point to the nearest boundary another Point
 *
 * This method aligns the point to the nearest boundary that is
 * a multiple of another Point. The align_dir parameter controls 
 * if the alignment is done towards the smaller or equal boundary 
 * (Floor) or towards the greater boundary (Ceil)
 *******************************************************************/

void Point::alignTo(const Point& p, AlignDir align_dir)
{
	if (align_dir == Ceil)
		*this += p - 1; 
	*this /= p;
	*this *= p;
}


FrameDim::FrameDim(const Size& size, ImageType type)
{
	m_size = size;
	m_type = type;
	m_depth = getImageTypeDepth(type);
}


FrameDim::FrameDim(int width, int height, ImageType type)
{
	m_size = Size(width, height);
	m_type = type;
	m_depth = getImageTypeDepth(type);
}

const Size& FrameDim::getSize() const
{
	return m_size;
}

ImageType FrameDim::getImageType() const
{
	return m_type;
}

int FrameDim::getDepth() const
{
	return m_depth;
}

int FrameDim::getImageTypeBpp(ImageType type)
{
	switch (type) {
	case Bpp8:  return 8;
	case Bpp10: return 10;
	case Bpp12: return 12;
	case Bpp14: return 14;
	case Bpp16: return 16;
	case Bpp32: return 32;
	default:    return 0;
	}
}

int FrameDim::getImageTypeDepth(ImageType type)
{
	switch (type) {
	case Bpp8: 
		return 1;
	case Bpp10: 
	case Bpp12: 
	case Bpp14: 
	case Bpp16: 
		return 2;
	case Bpp32: 
		return 4;
	default:    
		return 0;
	}
}
