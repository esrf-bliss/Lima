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
namespace Simulator
{
class Camera
{
%TypeHeaderCode
#include "SimulatorCamera.h"
#include <string>
#include <time.h>
#include <cmath>
%End

 public:
	Camera();
	~Camera();

	HwBufferCtrlObj* getBufferCtrlObj();
	Simulator::FrameBuilder* getFrameBuilder();
	
	void startAcq();
	void stopAcq();

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames /Out/);

	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time /Out/);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time /Out/);

	void setBin(const Bin& bin);
	void getBin(Bin& bin /Out/);
	void checkBin(Bin& bin /In,Out/);

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(FrameDim& frame_dim /Out/);
	
	HwInterface::StatusType::Basic getStatus();
	int getNbAcquiredFrames();

	void getMaxImageSize(Size& max_image_size /Out/);

	void reset();

	SIP_PYOBJECT __repr__() const;
%MethodCode
	LIMA_REPR_CODE
%End
};
};

