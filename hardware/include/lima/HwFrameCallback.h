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
#ifndef HWFRAMECALLBACK_H
#define HWFRAMECALLBACK_H

#include "lima/SizeUtils.h"
#include "lima/Timestamp.h"
#include "lima/HwFrameInfo.h"
#include "lima/Debug.h"

namespace lima
{

class HwFrameCallback;


/*******************************************************************
 * \class HwFrameCallbackGen
 * \brief Abstract class with interface for frame callback generation
 *
 * This class specifies the basic functionality for objects generating
 * frame callbacks
 *******************************************************************/

class LIMACORE_API HwFrameCallbackGen
{
	DEB_CLASS(DebModHardware, "HwFrameCallbackGen");

 public:
	HwFrameCallbackGen();
	virtual ~HwFrameCallbackGen();

	void registerFrameCallback(HwFrameCallback& frame_cb);
	void unregisterFrameCallback(HwFrameCallback& frame_cb);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
	bool newFrameReady(const HwFrameInfoType& frame_info);
	
 private:
	HwFrameCallback *m_frame_cb;
};


/*******************************************************************
 * \class HwFrameCallback
 * \brief Base class for classes receiving frame callbacks
 *
 *******************************************************************/

class LIMACORE_API HwFrameCallback
{
	DEB_CLASS(DebModHardware, "HwFrameCallback");

 public:
	HwFrameCallback();
	virtual ~HwFrameCallback();

	HwFrameCallbackGen *getFrameCallbackGen() const;

 protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info) = 0;

 private:
	friend class HwFrameCallbackGen;
	void setFrameCallbackGen(HwFrameCallbackGen *frame_cb_gen);

	HwFrameCallbackGen *m_frame_cb_gen;

};

inline HwFrameCallbackGen *HwFrameCallback::getFrameCallbackGen() const
{
	return m_frame_cb_gen;
}

} // namespace lima

#endif // HWFRAMECALLBACK_H
