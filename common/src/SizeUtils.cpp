#include "SizeUtils.h"

using namespace lima;
using namespace std;


/*******************************************************************
 * \brief Aligns the Point to the nearest boundary of another Point
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

ostream& lima::operator <<(ostream& os, const Point& p)
{
	return os << "<" << p.x << "," << p.y << ">";
}


ostream& lima::operator <<(ostream& os, const Size& s)
{
	return os << "<" << s.getWidth() << "x" << s.getHeight() << ">";
}

ostream& lima::operator <<(ostream& os, const Bin& bin)
{
	return os << "<" << bin.getX() << "x" << bin.getY() << ">";
}


/*******************************************************************
 * \brief Set the roi by giving to arbitrary (diagonal) corners
 *
 * This method works by finding the top-left and bottom-right corners
 *******************************************************************/

void Roi::setCorners(const Point& p1, const Point& p2)
{
	int x1 = min(p1.x, p2.x);
	int x2 = max(p1.x, p2.x);
	int y1 = min(p1.y, p2.y);
	int y2 = max(p1.y, p2.y);
	m_top_left = checkCorner(Point(x1, y1));
	Point bottom_right = Point(x2, y2);
	m_size = bottom_right + 1 - m_top_left;
}

ostream& lima::operator <<(ostream& os, const Roi& roi)
{
	return os << roi.getTopLeft() << "-" << roi.getSize();
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
	default:
		throw LIMA_COM_EXC(InvalidValue, "Invalid image type");
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
		throw LIMA_COM_EXC(InvalidValue, "Invalid image type");
	}
}

ostream& lima::operator <<(ostream& os, const FrameDim& fdim)
{
	const Size& size = fdim.getSize();
	return os << "<" << size.getWidth() << "x" << size.getHeight() << "x"
		  << fdim.getDepth() << "-" << fdim.getImageType() << ">";
}
