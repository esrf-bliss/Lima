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
#ifndef RAYONIXHSBINCTRLOBJ_H
#define RAYONIXHSBINCTRLOBJ_H

#include "LimaCompatibility.h"
#include "SizeUtils.h"
#include "RayonixHsCamera.h"
//#include "Debug.h"

#include "HwBinCtrlObj.h"

namespace lima {
namespace RayonixHs {

class BinCtrlObj : public HwBinCtrlObj {
   public:
      BinCtrlObj(Camera *cam);
      ~BinCtrlObj();

      virtual void setBin(const Bin& bin);
      virtual void getBin(Bin& bin);
      virtual void checkBin(Bin& bin);
   private:
      Camera *m_cam;
};

} // namespace lima
} // namespace RayonixHs

#endif // RAYONIXHSBINCTRLOBJ_H
