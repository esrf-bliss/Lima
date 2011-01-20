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


namespace Maxipix {

%TypeHeaderCode

#include "HwMaxImageSizeCallback.h"
#include "MaxipixReconstruction.h"
#include "Constants.h"

using namespace lima;
%End

class MaxipixDet
{

  public:

    enum Version {
      DUMMY,
      MPX2,
      MXR2,
      TPX1
    };

    enum Polarity {
      NEGATIVE,
      POSITIVE
    };

    static const int ChipSize;
    static const double PixelSize;
    static const int MaxChips;

    MaxipixDet();
    ~MaxipixDet();

    // -- version
    void setVersion(Version version);

    // -- geometry
    void setNbChip(int xchip, int ychip);
    void setPixelGap(int xgap, int ygap);

    // -- detector info
    void getImageSize(Size& size /Out/);
    void getPixelSize(double& size /Out/);
    void getImageType(ImageType& type /Out/);

    void getDetectorType(std::string& type /Out/);
    void getDetectorModel(std::string& model /Out/);

    // -- reconstruction
    bool needReconstruction();
    void getReconstruction(Maxipix::MaxipixReconstruction::Model& model /Out/);

};

};

