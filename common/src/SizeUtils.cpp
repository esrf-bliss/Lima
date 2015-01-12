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
#include "lima/SizeUtils.h"

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

void Point::swap()
{
	int tmp = x;
	x = y,y = tmp;
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
 * \brief Constant corners
 *******************************************************************/

const Corner lima::TopLeft    (Left,  Top);
const Corner lima::TopRight   (Right, Top);
const Corner lima::BottomLeft (Left,  Bottom);
const Corner lima::BottomRight(Right, Bottom);


/*******************************************************************
 * \brief X/YBorder and Corner ostream << operators
 *******************************************************************/

ostream& lima::operator <<(ostream& os, XBorder xb)
{
	return os << ((xb == Left) ? "Left" : "Right");
}

ostream& lima::operator <<(ostream& os, YBorder yb)
{
	return os << ((yb == Top)  ? "Top" : "Bottom");
}

ostream& lima::operator <<(ostream& os, const Corner& c)
{
	return os << c.getY() << c.getX();
}


/*******************************************************************
 * \brief Get coordinates relative to specified corner
 *******************************************************************/

Point Size::getCornerCoords(const Point& p, const Corner& c)
{
	Point rel = p;
	if (c.getX() == Right)
		rel.x = getWidth()  - 1 - rel.x;
	if (c.getY() == Bottom)
		rel.y = getHeight() - 1 - rel.y;
	return rel;
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

Roi Roi::subRoiRel2Abs(const Roi& rel_sub_roi) const
{
	Roi aux(0, getSize());
	if (!aux.containsRoi(rel_sub_roi))
		throw LIMA_COM_EXC(InvalidValue, "Given roi is not sub roi");
	const Point& tl = getTopLeft();
	return Roi(rel_sub_roi.getTopLeft() + tl, rel_sub_roi.getSize());
}

Roi Roi::subRoiAbs2Rel(const Roi& abs_sub_roi) const
{
	if (!containsRoi(abs_sub_roi))
		throw LIMA_COM_EXC(InvalidValue, "Given roi is not sub roi");
	const Point& tl = getTopLeft();
	return Roi(abs_sub_roi.getTopLeft() - tl, abs_sub_roi.getSize());
}

ostream& lima::operator <<(ostream& os, const Roi& roi)
{
	return os << roi.getTopLeft() << "-" << roi.getSize();
}

int FrameDim::getImageTypeBpp(ImageType type)
{
	switch (type) {
	case Bpp8S:
	case Bpp8:  return 8;
	case Bpp10S:
	case Bpp10: return 10;
	case Bpp12S:
	case Bpp12: return 12;
	case Bpp14S:
	case Bpp14: return 14;
	case Bpp16S:
	case Bpp16: return 16;
	case Bpp32S:
	case Bpp32F:
	case Bpp32: return 32;
	default:
		throw LIMA_COM_EXC(InvalidValue, "Invalid image type");
	}
}

int FrameDim::getImageTypeDepth(ImageType type)
{
	switch (type) {
	case Bpp8S:
	case Bpp8: 
		return 1;
	case Bpp10: 
	case Bpp10S: 
	case Bpp12: 
	case Bpp12S: 
	case Bpp14: 
	case Bpp14S: 
	case Bpp16: 
	case Bpp16S: 
		return 2;
	case Bpp32: 
	case Bpp32S: 
	case Bpp32F:
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

ostream& lima::operator <<(ostream& os,const ArcRoi& arc)
{
  double x,y;
  double start,end;
  double rayon1,rayon2;

  arc.getCenter(x,y);
  arc.getAngles(start,end);
  arc.getRayons(rayon1,rayon2);

  return os << "<center: (" << x << "," << y << ")"
	    << " angles: (" << start << "," << end << ")"
	    << " rayons: (" << rayon1 << "," << rayon2 << ")"
	    << ">";
}
