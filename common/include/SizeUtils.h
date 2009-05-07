#ifndef SIZEUTILS_H
#define SIZEUTILS_H

#include "Constants.h"
#include "Exceptions.h"

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
	return Point(p1) += p2;
}

inline Point operator -(const Point& p1, const Point& p2)
{
	return Point(p1) -= p2;
}

inline Point operator *(const Point& p1, const Point& p2)
{
	return Point(p1) *= p2;
}

inline Point operator /(const Point& p1, const Point& p2)
{
	return Point(p1) /= p2;
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
	Size()                 : m_xy()       {}
	Size(int w, int h)     : m_xy(w, h)   {}
	Size(const Point& p)   : m_xy(p)      {}
	Size(const Size& s)    : m_xy(s.m_xy) {}

	int getWidth() const
	{ return m_xy.x; }

	int getHeight() const
	{ return m_xy.y; }

	bool isEmpty() const
	{ return m_xy.getArea() == 0; }

	operator Point() const
	{ return m_xy; }

	Size& operator  =(const Point& p)
	{ m_xy  = p; return *this; }

	Size& operator +=(const Point& p)
	{ m_xy += p; return *this; }

	Size& operator -=(const Point& p)
	{ m_xy -= p; return *this; }

	Size& operator *=(const Point& p)
	{ m_xy *= p; return *this; }

	Size& operator /=(const Point& p)
	{ m_xy /= p; return *this; }


 private:
	Point m_xy;
};


/*******************************************************************
 * \class Size
 * \brief Basic rectangle size class
 *
 * This class helps managing the size of rectangular objects
 *******************************************************************/

class Bin
{
 public:
	Bin()		      : m_xy(1)                       {}
	Bin(int x, int y)     : m_xy(checkValid(Point(x, y))) {}
	Bin(const Point& p)   : m_xy(checkValid(p))           {}
	Bin(const Bin& b)     : m_xy(b.m_xy)                  {}

	int getX() const
	{ return m_xy.x; }

	int getY() const
	{ return m_xy.y; }

	operator Point() const
	{ return m_xy; }

	Bin& operator  =(const Point& p)
	{ m_xy = checkValid(p);        return *this; }

	Bin& operator *=(const Point& p)
	{ m_xy = checkValid(m_xy * p); return *this; }

	Bin& operator /=(const Point& p)
	{ m_xy = checkValid(m_xy / p); return *this; }

 private:
	static bool isValid(int i);
	static Point checkValid(const Point& p);

	Point m_xy;
};

inline bool Bin::isValid(int i)
{
	return (i > 0);
}

inline Point Bin::checkValid(const Point& p)
{
	if (!(isValid(p.x) && isValid(p.y)))
		throw LIMA_COM_EXC(InvalidValue, "Invalid binning");
	return p;
}


/*******************************************************************
 * \class FrameDim
 * \brief Class holding the Size and ImageType of a frame
 *
 * This class contains the X, Y and Z dimensions of a frame. For
 * practical reasons, it also contains the depth in bytes.
 *******************************************************************/

class FrameDim
{
 public:
	FrameDim(const Size& size, ImageType type);
	FrameDim(int width, int height, ImageType type);

	const Size& getSize() const;
	ImageType getImageType() const;
	int getDepth() const;
	int getMemSize() const;

	static int getImageTypeBpp(ImageType type);
	static int getImageTypeDepth(ImageType type);

 private:
	FrameDim();

	Size m_size;
	ImageType m_type;
	int m_depth;
};

inline const Size& FrameDim::getSize() const
{
	return m_size;
}

inline ImageType FrameDim::getImageType() const
{
	return m_type;
}

inline int FrameDim::getDepth() const
{
	return m_depth;
}

int FrameDim::getMemSize() const
{
	return Point(m_size).getArea() * m_depth;
}

inline bool operator ==(const FrameDim& f1, const FrameDim& f2)
{
	return ((f1.getSize()  == f2.getSize()) && 
		(f1.getDepth() == f2.getDepth()));
}

inline bool operator !=(const FrameDim& f1, const FrameDim& f2)
{	
	return !(f1 == f2);
}

 
} // namespace lima

#endif // SIZEUTILS_H
