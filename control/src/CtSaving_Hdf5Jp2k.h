//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2026
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9
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

#ifndef CTSAVING_HDF5JP2K_H
#define CTSAVING_HDF5JP2K_H

#include "hdf5.h"
#include "H5Zpublic.h"
#include "processlib/Data.h"

#include <cstddef>
#include <vector>

#ifndef LIMA_HDF5_JP2K_FILTER
#define LIMA_HDF5_JP2K_FILTER 32033
#endif

namespace lima {
namespace hdf5_jp2k {

enum DataType {
	JP2K_UINT8 = 1,
	JP2K_INT8 = 2,
	JP2K_UINT16 = 3,
	JP2K_INT16 = 4,
};

enum Codec {
	OpenJPEG = 0,
	Kakadu = 1,
};

enum FilterOption {
	FilterVersion = 0,
	FilterWidth = 1,
	FilterHeight = 2,
	FilterDataType = 3,
	FilterOptionCount = 4,
};

bool isSupported(Data::TYPE type);
DataType toJp2kType(Data::TYPE type);
std::size_t rawSize(std::size_t width, std::size_t height, DataType type);
std::size_t compressedBufferBound(std::size_t raw_size);

void encode(Data& data, std::vector<unsigned char>& out);
void encode(Data& data, double compression_ratio,
	    std::vector<unsigned char>& out);
void encode(Data& data, double compression_ratio, Codec codec,
	    std::vector<unsigned char>& out);

int register_filter();
H5Z_filter_t filter_id();
const H5Z_class2_t* filter_class();

} // namespace hdf5_jp2k
} // namespace lima

#endif // CTSAVING_HDF5JP2K_H
