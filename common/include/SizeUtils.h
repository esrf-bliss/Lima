#ifndef SIZEUTILS_H
#define SIZEUTILS_H

#include "Constants.h"

namespace lima
{

/*******************************************************************
 * \class Point
 * \brief Basic two-dimension arithmetic class
 *
 * This utility class provides basic arithmetic in two dimensions
 *******************************************************************/

class Point
{
 public:
	Point()               : x(0),   y(0)   {}
	Point(int i)          : x(i),   y(i)   {}
	Point(int x0, int y0) : x(x0),  y(y0)  {}
	Point(const Point& p) : x(p.x), y(p.y) {}

	Point& operator  =(const Point& p)
	{ x  = p.x, y  = p.y; return *this; }

	Point& operator +=(const Point& p)
	{ x += p.x, y += p.y; return *this; }

	Point& operator -=(const Point& p)
	{ x -= p.x, y -= p.y; return *this; }

	Point& operator *=(const Point& p)
	{ x *= p.x, y *= p.y; return *this; }

	Point& operator /=(const Point& p)
	{ x /= p.x, y /= p.y; return *this; }

	int getArea() const
	{ return x * y; }

	bool isNull() const
	{ return (x == 0) && (y == 0); }

	bool contains(const Point& p) const
	{ return (p.x <= x) && (p.y <= y); }

	void alignTo(const Point& p, AlignDir align_dir);

	int x, y;
};

inline Point operator +(const Point& p1, const Point& p2)
{
	Point p = p1;
	return p += p2;
}

inline Point operator -(const Point& p1, const Point& p2)
{
	Point p = p1;
	return p -= p2;
}

inline Point operator *(const Point& p1, const Point& p2)
{
	Point p = p1;
	return p *= p2;
}

inline Point operator /(const Point& p1, const Point& p2)
{
	Point p = p1;
	return p /= p2;
}

inline bool operator ==(const Point& p1, const Point& p2)
{
	return (p1.x == p2.x) && (p1.y == p2.y);
}

inline bool operator !=(const Point& p1, const Point& p2)
{
	return !(p1 == p2);
}


/*******************************************************************
 * \class Size
 * \brief Basic rectangle size class
 *
 * This class helps managing the size of rectangular objects
 *******************************************************************/

class Size
{
 public:
	Size() {}
	Size(int w, int h)     : m_xy(w, h)   {}
	Size(const Point& w_h) : m_xy(w_h)    {}
	Size(const Size& s)    : m_xy(s.m_xy) {}

	int getWidth() const
	{ return m_xy.x; }

	int getHeight() const
	{ return m_xy.y; }

	operator Point() const
	{ return m_xy; }

 private:
	Point m_xy;
};



class FrameDim
{
 public:
	FrameDim(const Size& size, ImageType type);
	FrameDim(int width, int height, ImageType type);

	const Size& getSize() const;
	ImageType getImageType() const;
	int getDepth() const;

	static int getImageTypeBpp(ImageType type);
	static int getImageTypeDepth(ImageType type);

 private:
	FrameDim();

	Size m_size;
	ImageType m_type;
	int m_depth;
};



} // lima

#endif // SIZEUTILS_H
