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
#ifndef METADETINFOCTRLOBJ_H
#define METADETINFOCTRLOBJ_H

#include "lima/HwDetInfoCtrlObj.h"
#include "lima/Debug.h"

namespace lima
{
  class HwInterface;
  namespace Meta
  {
    class Interface;
    class DetInfoCtrlObj : public HwDetInfoCtrlObj, public HwMaxImageSizeCallbackGen
    {
      DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj","Meta");
      class _MaxImageSizeCallback;
      friend class _MaxImageSizeCallback;
      friend class Interface;
    public:
      DetInfoCtrlObj(Interface&);
      virtual ~DetInfoCtrlObj();

      virtual void getMaxImageSize(Size& max_image_size);
      virtual void getDetectorImageSize(Size& det_image_size);

      virtual void getDefImageType(ImageType& def_image_type);
      virtual void getCurrImageType(ImageType& curr_image_type);
      virtual void setCurrImageType(ImageType  curr_image_type);

      virtual void getPixelSize(double& x_size,double &y_size);
      virtual void getDetectorType(std::string& det_type);
      virtual void getDetectorModel(std::string& det_model);

      virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
      virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
    private:
      Interface&		m_interface;
      std::list<_MaxImageSizeCallback*> m_cbk;

      void _maxImageSizeChanged();
      void _addInterface(HwInterface*);
    };
  } // namespace Meta
} // namespace lima

#endif // METADETINFOCTRLOBJ_H
