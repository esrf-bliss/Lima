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
class Timestamp
{
%TypeHeaderCode
#include <Timestamp.h>
using namespace lima;
%End
 public:
	Timestamp();
	Timestamp(double ts);
	Timestamp(const Timestamp& ts);

	bool isSet() const;

	operator double() const;

	Timestamp& operator +=(const Timestamp& ts);

	Timestamp& operator -=(const Timestamp& ts);

	Timestamp& operator *=(double factor);

	Timestamp& operator /=(double factor);
	
	static Timestamp now();
};
