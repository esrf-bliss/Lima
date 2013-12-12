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
bool IsPowerOf2(int x);

class Point
{
%TypeHeaderCode
#include <SizeUtils.h>
%End
 public:
	int x;
	int y;

	Point();
	Point(int i);
	Point(int x0, int y0);
	Point(const Point& p);

	Point& operator+=(const Point& p);

	Point& operator-=(const Point& p);

	Point& operator*=(const Point& p);

	Point& operator/=(const Point& p);

	Point& operator%=(const Point& p);

	int getArea() const;

	bool isNull() const;

	bool contains(const Point& p) const;

	void alignTo(const Point& p, AlignDir align_dir);
	void swap();
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
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
	void setX(XBorder xb);
	void setY(YBorder yb);

	XBorder getX() const;
	YBorder getY() const;

	Point getDir() const;

	bool operator ==(const Corner& c);
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
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

	void swapDimensions();

	operator Point() const;

	Size& operator +=(const Point& p);

	Size& operator -=(const Point& p);

	Size& operator *=(const Point& p);

	Size& operator /=(const Point& p);

	void alignTo(const Point& p, AlignDir align_dir);

	Point getCornerCoords(const Point& p, const Corner& c);
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
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
	void swap();
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
};

class Flip
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	bool x;
	bool y;

	Flip();
	Flip(bool b);
	Flip(bool x0, bool y0);
	Flip(const Flip& f);

	operator Point() const;

	Corner getRefCorner() const;

	Flip& operator &=(const Flip& f);
	void reset();
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End	
};

Flip operator-(const Flip &f,const Flip &s);
Flip operator+(const Flip &f,const Flip &s);

class Roi
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
 public:
	Roi();
	Roi(int x,int y,int width,int height);
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

	Roi getFlipped(const Flip& f,const Size& image_size) const;

	Roi getRotated(RotationMode r,const Size& image_size) const;
	Roi getUnrotated(RotationMode r,const Size& image_size) const;

	bool isActive() const;

	void alignSizeTo(const Point& p, AlignDir align_dir);
	void alignCornersTo(const Point& p, AlignDir align_dir);
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
};

bool operator ==(const Roi& r1, const Roi& r2);
bool operator !=(const Roi& r1, const Roi& r2);

%MappedType std::list<Roi>
{
%TypeHeaderCode
#include <list>
#include <SizeUtils.h>
using namespace lima;
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
 bool aReturnFlag = PyList_Check(sipPy);
 for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
  aReturnFlag = sipCanConvertToType(PyList_GET_ITEM(sipPy, i),
                                     sipType_Roi, SIP_NOT_NONE);
 return aReturnFlag;
}

std::list<Roi> *roiList = new std::list<Roi>();
for(int i = 0;i < PyList_Size(sipPy);++i)
{
 int state;
 Roi *aRoi = reinterpret_cast<Roi*>(sipConvertToType(PyList_GET_ITEM(sipPy,i),
					             sipType_Roi, 0,	
						     SIP_NOT_NONE,
						     &state, sipIsErr));
 // Deal with any errors.
  if (*sipIsErr)
  {
   sipReleaseType(aRoi, sipType_Roi, state);

   // Tidy up.
   delete roiList;

   // There is no temporary instance.
   return 0;
  }

 roiList->push_back(*aRoi);
 sipReleaseType(aRoi, sipType_Roi, state);
}
*sipCppPtr = roiList;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
 PyObject *l;

 // Create the Python list of the correct length.
 if (!(l = PyList_New(sipCpp->size())))
  return NULL;

 // Go through each element in the C++ instance and convert it to a
 // wrapped Roi
 int i = 0;
 sipTransferObj = NULL;
 for (std::list<Roi>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
  {
   Roi *aRoi = new Roi(*j);
   PyObject *wobj;

   // Get the Python wrapper for the QWidget instance, creating a new
   // one if necessary, and handle any ownership transfer.
   if (!(wobj = sipConvertFromNewType(aRoi, sipType_Roi, sipTransferObj)))
    {
     delete aRoi;
     // There was an error so garbage collect the Python list.
     Py_DECREF(l);
     return NULL;
    }

   // Add the wrapper to the list.
   PyList_SET_ITEM(l, i, wobj);
  }

 // Return the Python list.
 return l;
%End
};

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
	void swapDimensions();
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End
};

class ArcRoi
{
%TypeHeaderCode
#include <SizeUtils.h>
using namespace lima;
%End
public:
  ArcRoi();
  ArcRoi(double centerX,double centerY,
	 double rayon1,double rayon2,
	 double angle_start,double angle_end);

  bool isEmpty() const;
  void reset();

  void setCenter(double x,double y);
  void getCenter(double& x /Out/,double& y /Out/) const;

  void setRayons(double rayon1,double rayon2);
  void getRayons(double& rayon1 /Out/,double& rayon2 /Out/) const;

  void setAngles(double start,double end);
  void getAngles(double& start /Out/,double& end /Out/) const;

  const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End

};
