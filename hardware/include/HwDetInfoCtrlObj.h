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
#ifndef HWDETINFOCTRLOBJ_H
#define HWDETINFOCTRLOBJ_H

#include "LimaCompatibility.h"
#include "HwMaxImageSizeCallback.h"
#include <string>

namespace lima
{

class LIMACORE_API HwDetInfoCtrlObj
{
	DEB_CLASS(DebModHardware, "HwDetInfoCtrlObj");

 public:
	HwDetInfoCtrlObj();
	virtual ~HwDetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size) = 0;
	virtual void getDetectorImageSize(Size& det_image_size) = 0;

	virtual void getDefImageType(ImageType& def_image_type) = 0;
	virtual void getCurrImageType(ImageType& curr_image_type) = 0;
	virtual void setCurrImageType(ImageType  curr_image_type) = 0;

	virtual void getPixelSize(double& x_size, double& y_size) = 0;
	virtual void getDetectorType(std::string& det_type) = 0;
	virtual void getDetectorModel(std::string& det_model) = 0;

	virtual void registerMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb) = 0;
	virtual void unregisterMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb) = 0;
	virtual void setUserDetectorName(const std::string &username){m_username = username;};
	virtual void getUserDetectorName(std::string &username) {
	  if (!m_username.empty()) username = m_username;
	  else getDetectorType(username);
	};

private:
	std::string m_username;
};


} // namespace lima


#endif // HWDETINFOCTRLOBJ_H
