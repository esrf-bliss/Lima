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
#ifndef HWBUFFERCTRLOBJ_H
#define HWBUFFERCTRLOBJ_H

#include "lima/LimaCompatibility.h"
#include "lima/HwFrameCallback.h"

namespace lima
{
/// This interface controls the image memory buffer allocation and management. Buffers are used:
/// - As temporary frame storage before saving, allowing disk / network speed fluctuations.
/// - To permanently hold images that can be read by the user after the acquisition is finished.
/// These buffer functionalities may be implemented by the hardware layer (kernel driver in the case of the Espia).
/// If not, an auxiliary buffer manager class will be provided to facilitate (and unify) its software implementation.
/// The buffer management parameters are :
class LIMACORE_API HwBufferCtrlObj
{
	DEB_CLASS(DebModHardware, "HwBufferCtrlObj");

public:
	HwBufferCtrlObj();
	virtual ~HwBufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim) = 0;
	virtual void getFrameDim(      FrameDim& frame_dim) = 0;

	virtual void setNbBuffers(int  nb_buffers) = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;

	virtual void setNbConcatFrames(int  nb_concat_frames) = 0;
	virtual void getNbConcatFrames(int& nb_concat_frames) = 0;

	virtual void getMaxNbBuffers(int& max_nb_buffers) = 0;

	/// Returns a pointer to the buffer at the specified location
	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0) = 0;
	/// Returns a pointer to the frame at the specified location
	virtual void *getFramePtr(int acq_frame_nb) = 0;

	/// Returns the start timestamp
	virtual void getStartTimestamp(Timestamp& start_ts) = 0;
	/// Returns some information for the specified frame number such as timestamp
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info) = 0;

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb) = 0;
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb) = 0;
	
	class LIMACORE_API Callback
	{
	public:
		virtual ~Callback();
		// returns a pointer to internal object that refers to address
		virtual void *map(void *address) = 0;
		// receives the pointer returned by map
		virtual void release(void *address_ref) = 0;
		virtual void releaseAll() = 0;
	};

	virtual Callback *getBufferCallback();
};
 
} // namespace lima

#endif // HWBUFFERCTRLOBJ_H
