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
#ifndef HWFRAMEINFO_H
#define HWFRAMEINFO_H

#include "processlib/Container.h"

#include "lima/LimaCompatibility.h"
#include "lima/SizeUtils.h"
#include "lima/Timestamp.h"
#include <ostream>

namespace lima
{

/*******************************************************************
 * \typedef HwFrameInfoType
 * \brief Structure containing information about acquired frame
 *
 *
 *******************************************************************/

struct LIMACORE_API HwFrameInfo {
	enum OwnerShip {Managed,Transfer,Shared};

	typedef Container<sideband::DataPtr> SidebandContainer;

	int acq_frame_nb;
	void *frame_ptr;
	FrameDim frame_dim;
	Timestamp frame_timestamp;
	int valid_pixels;
	OwnerShip buffer_owner_ship;
	SidebandContainer sideband_data;

	HwFrameInfo() 
		: acq_frame_nb(-1), frame_ptr(NULL), frame_dim(),
		  frame_timestamp(), valid_pixels(0), buffer_owner_ship(Managed) {}

	HwFrameInfo(int frame_nb, void *ptr, const FrameDim *dim, 
		    Timestamp timestamp, int pixels, OwnerShip owner,
		    const SidebandContainer& sideband = {});
  
	HwFrameInfo(const HwFrameInfo &anInfo);

	bool isValid() const;
};

typedef HwFrameInfo HwFrameInfoType; // For backward compatibility

LIMACORE_API std::ostream& operator <<(std::ostream& os, const HwFrameInfo& info);

// HwFrameInfo API
template <typename T>
bool HwHasData(const std::string& key, HwFrameInfo& info)
{
	return info.sideband_data.contains(key);
}

template <typename T>
bool HwAddData(const std::string& key, HwFrameInfo& info,
	       std::shared_ptr<T> sb_data)
{
	return info.sideband_data.insert(key, sb_data);
}

template <typename T>
void HwSetData(const std::string& key, HwFrameInfo& info,
	       std::shared_ptr<T> sb_data)
{
	return info.sideband_data.insert(key, sb_data);
}

template <typename T>
std::shared_ptr<T> HwGetData(const std::string& key, HwFrameInfo& info)
{
	return sideband::DataCast<T>(info.sideband_data.get(key));
}

inline void HwRemoveData(const std::string& key, HwFrameInfo& info)
{
	info.sideband_data.erase(key);
}

} //namespace lima

#endif // HWFRAMEINFO_H
