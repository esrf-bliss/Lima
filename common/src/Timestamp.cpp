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
#include "lima/Timestamp.h"
#include "lima/Exceptions.h"

#ifdef __unix
#include <sys/time.h>
#else
#include <time_compat.h>
#endif
#include <unistd.h>
#include <iostream>

using namespace lima;

static const Timestamp Unset = -12345678901234567890.1234567890;

Timestamp::Timestamp() : m_ts(Unset) {}

bool Timestamp::isSet() const
{
  return *this != Unset;
}

Timestamp Timestamp::now()
{
	struct timeval t;
	if (gettimeofday(&t, NULL) < 0)
		throw LIMA_HW_EXC(Error, "Error calling gettimeofday");

	return t.tv_sec + t.tv_usec * 1e-6;
}

double lima::Sleep(double sec)
{
	Timestamp t0 = Timestamp::now();
	if (sec > 0)
		usleep((unsigned long) (sec * 1e6 + 0.1));
	return Timestamp::now() - t0;
}
