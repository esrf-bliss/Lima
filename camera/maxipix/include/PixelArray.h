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
#ifndef _PRIAMARR_H
#define _PRIAMARR_H

#include "MaxipixDet.h"

namespace lima {
namespace Maxipix {

class PixelConfigArray {

  public:

    PixelConfigArray(MaxipixDet::Version);
    ~PixelConfigArray();

    void convert(std::string&);

    char* maskArray;
    char* testArray;
    char* lowArray;
    char* highArray;

  private:
    struct PixelArrayBitType {
	short mask;
	short test;
	short nbLow;
	short low[4];
	short nbHigh;
	short high[4];
    };

    MaxipixDet::Version m_version;
    PixelArrayBitType m_bit;

};

class PixelDataArray {
  public:
    PixelDataArray(MaxipixDet::Version);
    ~PixelDataArray();

    void convert(std::string, unsigned short*);

  private:
    MaxipixDet::Version m_version;
};

};
};

#endif // _PRIAMARR_H
