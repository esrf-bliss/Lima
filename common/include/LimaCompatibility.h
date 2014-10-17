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
#ifndef LIMACOMPATIBILITY_H
#define LIMACOMPATIBILITY_H

#ifdef WIN32
	#include <WTypes.h> // Include this first on Win (bug #35683 aka bug #73144)
	#ifdef LIMACORE_EXPORTS
		#define LIMACORE_API __declspec(dllexport)
	#else
		#define LIMACORE_API __declspec(dllimport)
	#endif
#else  /* Unix */
		#define LIMACORE_API
#endif

#endif //- LIMACOMPATIBILITY_H
