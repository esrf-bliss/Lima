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

#include "LimaCompatibility.h"
#include "HwFrameCallback.h"

namespace lima
{

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

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0) = 0;
	virtual void *getFramePtr(int acq_frame_nb) = 0;

	virtual void getStartTimestamp(Timestamp& start_ts) = 0;
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info) = 0;

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb) = 0;
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb) = 0;
	
	class LIMACORE_API Callback
	{
	public:
		virtual ~Callback();
		virtual void map(void *address) = 0;
		virtual void release(void *address) = 0;
		virtual void releaseAll() = 0;
	};

	virtual Callback *getBufferCallback();
};
 
} // namespace lima

#endif // HWBUFFERCTRLOBJ_H
