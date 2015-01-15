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
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "lima/LimaCompatibility.h"
#include "lima/Exceptions.h"

namespace lima
{

class LIMACORE_API Timestamp
{
 public:
	Timestamp();
	Timestamp(double ts) : m_ts(ts) {}
	Timestamp(const Timestamp& ts) : m_ts(ts.m_ts) {}

	bool isSet() const;
	
	operator double() const
	{ return m_ts; }

	Timestamp& operator  =(const Timestamp& ts)
	{ m_ts = ts.m_ts; return *this; }

	Timestamp& operator +=(const Timestamp& ts)
	{ validValue() += ts.validValue(); return *this; }

	Timestamp& operator -=(const Timestamp& ts)
	{ validValue() -= ts.validValue(); return *this; }

	Timestamp& operator *=(double factor)
	{ validValue() *= factor; return *this; }

	Timestamp& operator /=(double factor)
	{ validValue() /= factor; return *this; }
	
	static Timestamp now();

 private:
	double& validValue();
	const double& validValue() const;

	double m_ts;
};

inline double& Timestamp::validValue()
{
	if (!isSet())
		throw LIMA_COM_EXC(InvalidValue, "Timestamp not set");
	return m_ts;
}

inline const double& Timestamp::validValue() const
{
	if (!isSet())
		throw LIMA_COM_EXC(InvalidValue, "Timestamp not set");
	return m_ts;
}


inline Timestamp operator +(Timestamp t1, Timestamp t2)
{
	return t1 += t2;
}

inline Timestamp operator -(Timestamp t1, Timestamp t2)
{
	return t1 -= t2;
}

inline Timestamp operator *(Timestamp t, double factor)
{
	return t *= factor;
}

inline Timestamp operator /(Timestamp t, double factor)
{
	return t /= factor;
}

LIMACORE_API double Sleep(double sec);

}

#endif // TIMESTAMP_H
