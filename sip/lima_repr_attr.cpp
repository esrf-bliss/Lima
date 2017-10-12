//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2017
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9 
// FRANCE
//
// Contact: lima@esrf.fr
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

#include <Python.h>
// __repr__ attr for both python 2 (string) and 3 (unicode)
// to be included in sip class definition using %include 

//	SIP_PYOBJECT __repr__() const /TypeHint="str"/;

#ifndef LIMA_REPR_CODE
#if PY_VERSION_HEX >= 0x03000000
#define LIMA_REPR_CODE \
	std::ostringstream str; \
	str << *sipCpp;	\
	const std::string& tmpString = str.str(); \
	sipRes = PyUnicode_FromString(tmpString.c_str());
#else
#define LIMA_REPR_CODE \ 
	std::ostringstream str; \
	str << *sipCpp;	\
	const std::string& tmpString = str.str(); \
	sipRes = PyString_FromString(tmpString.c_str());
#endif

#endif
