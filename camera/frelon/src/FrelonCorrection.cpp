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

#include "FrelonCorrection.h"

using namespace lima;
using namespace lima::Frelon;
using namespace std;

const int    E2VCorrection::FirstCol    = 1023;
const int    E2VCorrection::LastCol     = 1024;
const double E2VCorrection::ErrorFactor = 0.004;

E2VCorrection::E2VCorrection()
{
}

E2VCorrection::~E2VCorrection()
{
}

E2VCorrection::E2VCorrection(const E2VCorrection& o)
{
}

void E2VCorrection::setHwBin(const Bin& hw_bin)
{
	m_hw_bin = hw_bin;
}

void E2VCorrection::getHwBin(Bin& hw_bin)
{
	hw_bin = m_hw_bin;
}

void E2VCorrection::setHwRoi(const Roi& hw_roi)
{
	m_hw_roi = hw_roi;
}

void E2VCorrection::getHwRoi(Roi& hw_roi)
{
	hw_roi = m_hw_roi;
}

Data E2VCorrection::process(Data& data)
{
	Data ret = data;

	if (!_processingInPlaceFlag) {
		int size = data.size();
		Buffer *buffer = new Buffer(size);
		memcpy(buffer->data, data.data(), size);
		ret.setBuffer(buffer);
		buffer->unref();
	}

	int bin_x = m_hw_bin.getX();
	int corr_offset = FirstCol / bin_x - m_hw_roi.getTopLeft().x;
	int corr_width  = LastCol / bin_x - FirstCol / bin_x + 1;
	int roi_width   = m_hw_roi.getSize().getWidth();
	if ((corr_offset + corr_width <= 0) || (corr_offset >= roi_width))
		return ret;

	if (corr_offset + corr_width > roi_width)
		corr_width = roi_width - corr_offset;
	if (corr_offset < 0) {
		corr_width += corr_offset;
		corr_offset = 0;
	}
		
	typedef unsigned short T;
	T *ptr = (T *) ret.data();
	ptr += corr_offset;

	double corr_factor = 1 + ErrorFactor / bin_x;
	int roi_height = m_hw_roi.getSize().getHeight();
	for (int y = 0; y < roi_height; ++y, ptr += roi_width)
		for (int x = 0; x < corr_width; ++x)
			ptr[x] = T(ptr[x] * corr_factor);

	return ret;
}
