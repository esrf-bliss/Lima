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
#ifndef MAXIPIXDET_H
#define MAXIPIXDET_H

#include "HwMaxImageSizeCallback.h"
#include "MaxipixReconstruction.h"
#include "SizeUtils.h"
#include "Debug.h"

namespace lima {
namespace Maxipix {

class MaxipixDet : public HwMaxImageSizeCallbackGen
{
  DEB_CLASS_NAMESPC(DebModCamera, "Camera", "MaxipixDet");

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

    static const int ChipSize= 256*256;
    static const double PixelSize= 55.0;
    static const int MaxChips= 5;

    MaxipixDet();
    ~MaxipixDet();

    // -- version
    void setVersion(Version version);

    // -- geometry
    void setNbChip(int xchip, int ychip);
    void setPixelGap(int xgap, int ygap);

    // -- detector info
    void getImageSize(Size& size);
    void getPixelSize(double& size);
    void getImageType(ImageType& type);

    void getDetectorType(std::string& type);
    void getDetectorModel(std::string& model);

    // -- reconstruction
    bool needReconstruction();
    void getReconstruction(MaxipixReconstruction::Model& model);

  protected:
    virtual void setMaxImageSizeCallbackActive(bool cb_active);

  private:
    void _updateSize();

    int m_xchip, m_ychip, m_nchip;
    int m_xgap, m_ygap;
    Size m_size;
    ImageType m_type;
    Version m_version;
    bool m_mis_cb_act;
};

} // namespace Maxipix
} // namespace lima

#endif // MAXIPIXDET_H

