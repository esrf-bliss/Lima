bool IsPowerOf2(int x);

class Point
{
%TypeHeaderCode
#include <SizeUtils.h>
%End
 public:
	Point();
	Point(int i);
	Point(int x0, int y0);
	Point(const Point& p);

	Point& operator+=(const Point& p);

	Point& operator-=(const Point& p);

	Point& operator*=(const Point& p);

	Point& operator/=(const Point& p);

	int getArea() const;

	bool isNull() const;

	bool contains(const Point& p) const;

	void alignTo(const Point& p, AlignDir align_dir);

	int x;
	int y;
};

enum XBorder {
	Left        = 0,
	Right       = 1,
};

enum YBorder {
	Top         = 0,
	Bottom      = 1,
};

class Corner
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	Corner();
	Corner(XBorder xb, YBorder yb);
	
	void set(XBorder xb, YBorder yb);

	XBorder getX() const;
	YBorder getY() const;

	bool operator ==(const Corner& c);
};

class Size
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	Size();
	Size(int w, int h);
	Size(const Point& p);
	Size(const Size& s);

	int getWidth() const;

	int getHeight() const;

	bool isEmpty() const;

	operator Point() const;

	Size& operator +=(const Point& p);

	Size& operator -=(const Point& p);

	Size& operator *=(const Point& p);

	Size& operator /=(const Point& p);

	void alignTo(const Point& p, AlignDir align_dir);

	Point getCornerCoords(const Point& p, const Corner& c);
};

class Bin
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	Bin();
	Bin(int x, int y);
	Bin(const Point& p);
	Bin(const Bin& b);

	int getX() const;

	int getY() const;

	bool isOne() const;

	void reset();

	operator Point() const;

	Bin& operator *=(const Point& p);

	Bin& operator /=(const Point& p);
};

class Roi
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	Roi();
	Roi(const Point& top_left, const Size& size);
        Roi(const Point& p1, const Point& p2);
	Roi(const Roi& r);

	const Point& getTopLeft() const;
	const Size& getSize() const;

	bool isEmpty() const;
	void reset();

	Point getTopRight() const;
	Point getBottomLeft() const;
	Point getBottomRight() const;

	void setTopLeft(const Point& top_left);
	void setSize(const Size& size);
	void setCorners(const Point& p1, const Point& p2);

	bool containsPoint(const Point& p) const;
	bool containsRoi(const Roi& r) const;

	Roi getBinned(const Bin& b) const;
	Roi getUnbinned(const Bin& b) const;

	bool isActive() const;

	void alignSizeTo(const Point& p, AlignDir align_dir);
	void alignCornersTo(const Point& p, AlignDir align_dir);
};

bool operator ==(const Roi& r1, const Roi& r2);
bool operator !=(const Roi& r1, const Roi& r2);

class FrameDim
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	FrameDim();
	FrameDim(const Size& size, ImageType type);
	FrameDim(int width, int height, ImageType type);

	bool isValid() const;

	void setSize(const Size& size);
	const Size& getSize() const;

	void setImageType(ImageType image_type);
	ImageType getImageType() const;

	int getDepth() const;
	int getMemSize() const;

	static int getImageTypeBpp(ImageType type);
	static int getImageTypeDepth(ImageType type);

	FrameDim& operator *=(const Point& point);
	FrameDim& operator /=(const Point& point);

	void checkValidPoint(const Point& point, bool for_div);
	void checkValidRoi(const Roi& roi);
};

