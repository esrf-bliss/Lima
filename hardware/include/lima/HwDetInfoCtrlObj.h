//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2020
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

#include "lima/LimaCompatibility.h"
#include "lima/HwMaxImageSizeCallback.h"
#include <string>

namespace lima
{

/// Provides static information about the detector and the current image dimension.
class LIMACORE_API HwDetInfoCtrlObj
{
	DEB_CLASS(DebModHardware, "HwDetInfoCtrlObj");

 public:
	HwDetInfoCtrlObj();
	virtual ~HwDetInfoCtrlObj();

	/// Return the maximum size of the image.
	virtual void getMaxImageSize(Size& max_image_size) = 0;
	/// Return the size of the detector image, it is always equal or greater than the MaxImageSize.
	virtual void getDetectorImageSize(Size& det_image_size) = 0;

  /// Returns the default data type of image (ushort, ulong, …)
	virtual void getDefImageType(ImageType& def_image_type) = 0;

  /// Returns the current data type of image (ushort, ulong, …).
	virtual void getCurrImageType(ImageType& curr_image_type) = 0;
	virtual void setCurrImageType(ImageType  curr_image_type) = 0;

	/// Physical size of pixels (in meter)
	virtual void getPixelSize(double& x_size, double& y_size) = 0;
	/// Returns the type of the detector (Frelon, Maxipix, …)
	virtual void getDetectorType(std::string& det_type) = 0;
	/// Returns the model of the detector.
	virtual void getDetectorModel(std::string& det_model) = 0;

  /// Register a callback called when the detector is reconfigured with a different geometry
	virtual void registerMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb) = 0;
	/// Unregister a callback previsouly registered with registerMaxImageSizeCallback
	virtual void unregisterMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb) = 0;

	/// Set a detector user name
	virtual void setUserDetectorName(const std::string &username){m_username = username;};
	/// Get a detector user name
	virtual void getUserDetectorName(std::string &username) {
	  if (!m_username.empty()) username = m_username;
	  else getDetectorType(username);
	};
	virtual void setInstrumentName(const std::string& name){m_instrument_name = name;};
	virtual void getInstrumentName(std::string& name){
	  if (!m_instrument_name.empty()) name = m_instrument_name;
	  else name = "instrument";
	}

private:
	std::string m_username;
	std::string m_instrument_name;
};


} // namespace lima


#endif // HWDETINFOCTRLOBJ_H
