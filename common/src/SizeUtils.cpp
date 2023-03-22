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

#include <algorithm>

using namespace lima;
using namespace std;

template <class T, class BinaryFunction>
static Point decode_pair(istream& is, string sep, BinaryFunction stov)
{
	string o;
	is >> o;
	string s = o;
	string::size_type l = s.size(), sl = sep.size();
	if (!l)
		throw LIMA_COM_EXC(InvalidValue, "Invalid pair: ") << o;
	if (s[0] == '<') {
		if ((l <= 2) || (s[l - 1] != '>'))
			throw LIMA_COM_EXC(InvalidValue, "Invalid pair: ") << o;
		l -= 2;
		s = s.substr(1, l);
	}
	string::size_type p;
	T x = stov(s, &p);
	if ((p == 0) || ((s = s.substr(p)).find(sep) != 0) || (s.size() == sl))
		throw LIMA_COM_EXC(InvalidValue, "Invalid pair: ") << o;
	s = s.substr(sl);
	T y = stov(s, &p);
	if (p != s.size())
		throw LIMA_COM_EXC(InvalidValue, "Invalid pair: ") << o;
	return Point(x, y);
}

static Point decode_int_pair(istream& is, string sep = ",")
{
	auto f = [](const std::string& str, std::size_t* pos) {
		return stoi(str, pos);
	};
	return decode_pair<int>(is, sep, f);
}

static bool stob(const string& s, string::size_type* pos)
{
	string aux = s;
	std::transform(aux.begin(), aux.end(), aux.begin(),::tolower);
	auto f = [&](const string& o) {
		if (s.find(o) != 0)
			return false;
		if (pos)
			*pos = o.size();
		return true;
	};
	if (f("true") || f("false"))
		return true;
	if (pos)
		*pos = 0;
	return false;
}

static Point decode_bool_pair(istream& is, string sep = ",")
{
	return decode_pair<bool>(is, sep, stob);
}


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

istream& lima::operator >>(istream& is, Point& p)
{
	p = decode_int_pair(is);
	return is;
}

ostream& lima::operator <<(ostream& os, const Size& s)
{
	return os << "<" << s.getWidth() << "x" << s.getHeight() << ">";
}

istream& lima::operator >>(istream& is, Size& s)
{
	s = decode_int_pair(is, "x");
	return is;
}

ostream& lima::operator <<(ostream& os, const Bin& bin)
{
	return os << "<" << bin.getX() << "x" << bin.getY() << ">";
}

istream& lima::operator >>(istream& is, Bin& bin)
{
	bin = decode_int_pair(is);
	return is;
}

istream& lima::operator >>(istream& is, Flip& flip)
{
	Point p = decode_bool_pair(is);
	flip = Flip(p.x, p.y);
	return is;
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

istream& lima::operator >>(istream& is, XBorder& xb)
{
	string s;
	is >> s;
	if (s == "Left")
		xb = Left;
	else if (s == "Right")
		xb = Right;
	else
		throw LIMA_COM_EXC(InvalidValue, "Invalid XBorder: ") << s;
	return is;
}

istream& lima::operator >>(istream& is, YBorder& yb)
{
	string s;
	is >> s;
	if (s == "Top")
		yb = Top;
	else if (s == "Bottom")
		yb = Bottom;
	else
		throw LIMA_COM_EXC(InvalidValue, "Invalid YBorder: ") << s;
	return is;
}

ostream& lima::operator <<(ostream& os, const Corner& c)
{
	return os << c.getY() << c.getX();
}

istream& lima::operator >>(istream& is, Corner& c)
{
	XBorder xb;
	YBorder yb;
	is >> yb >> xb;
	c.set(xb, yb);
	return is;
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

istream& lima::operator >>(istream& is, Roi& roi)
{
	string s;
	is >> s;
	string::size_type p = s.find('-');
	if ((p == 0) || (p == s.size()) || (p == string::npos))
		throw LIMA_COM_EXC(InvalidValue, "Invalid Roi: ") << s;
	istringstream tl_is(s.substr(0, p)), size_is(s.substr(p + 1));
	Point tl;
	tl_is >> tl;
	Size size;
	size_is >> size;
	roi = Roi(tl, size);
	return is;
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

	case Bpp1:
	case Bpp4:
	case Bpp6:
		return 8;
	case Bpp24:
	case Bpp24S:
		return 32;
    case Bpp64S:
    case Bpp64: return 64;
	default:
		throw LIMA_COM_EXC(InvalidValue, "Invalid image type");
	}
}

bool FrameDim::isImageTypeSigned(ImageType type)
{
	switch (type) {
	case Bpp8S:
	case Bpp10S:	
	case Bpp12S:	
	case Bpp14S:
	case Bpp16S:
	case Bpp24S:
	case Bpp32S:
	case Bpp32F:
		return true;

	case Bpp1:
	case Bpp4:
	case Bpp6:
	case Bpp8:
	case Bpp10:
	case Bpp12:
	case Bpp14:
	case Bpp16:
	case Bpp24:
	case Bpp32:
	case Bpp64:
		return false;
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

	case Bpp1:
	case Bpp4:
	case Bpp6:
		return 1;
	case Bpp24:
	case Bpp24S:
		return 4;
	case Bpp64:
	case Bpp64S:
		return 8;
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

istream& lima::operator >>(std::istream& is, FrameDim& fdim)
{
	// Decode format: 1024x512x1-Bpp8
	string s;
	is >> s;
	string::size_type sep1 = 0;
	sep1 = s.find('x', sep1);
	if ((sep1 == 0) || (sep1 == s.size()) || (sep1 == string::npos))
		throw LIMA_COM_EXC(InvalidValue, "Invalid FrameDim: ") << s;
	sep1 = s.find('x', sep1 + 1);
	if ((sep1 == 0) || (sep1 == s.size()) || (sep1 == string::npos))
		throw LIMA_COM_EXC(InvalidValue, "Invalid FrameDim: ") << s;
	string::size_type next1 = sep1 + 1;
	string::size_type sep2 = s.find('-', next1);
	if ((sep2 == next1) || (sep2 == s.size()) || (sep2 == string::npos))
		throw LIMA_COM_EXC(InvalidValue, "Invalid FrameDim: ") << s;
	string::size_type next2 = sep2 + 1;
	Size size;
	istringstream(s.substr(0, sep1)) >> size;
	int bpp;
	istringstream(s.substr(next1, sep2)) >> bpp;
	ImageType type;
	istringstream(s.substr(next2)) >> type;
	if (FrameDim::getImageTypeDepth(type) != bpp)
		throw LIMA_COM_EXC(InvalidValue,
				   "FrameDim Bpp/ImageType mismatch: ") << s;
	fdim = FrameDim(size, type);
	return is;
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
