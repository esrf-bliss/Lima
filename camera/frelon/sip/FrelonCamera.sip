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

namespace Frelon
{


class Camera
{

%TypeHeaderCode
#include "FrelonCamera.h"
%End

 public:
	Camera(Espia::SerialLine& espia_ser_line);
	~Camera();

	Frelon::SerialLine& getSerialLine();

	void writeRegister(Frelon::Reg reg, int  val);
	void readRegister (Frelon::Reg reg, int& val /Out/);

	void hardReset();
	void getVersionStr(std::string& ver /Out/);
	void getComplexSerialNb(int& complex_ser_nb /Out/);
	Frelon::Model& getModel();

	Frelon::TimingCtrl& getTimingCtrl();

	void getDefInputChan(Frelon::FrameTransferMode ftm, 
			     Frelon::InputChan& input_chan /Out/);
	void setInputChan(Frelon::InputChan  input_chan);
	void getInputChan(Frelon::InputChan& input_chan /Out/);

	void setFrameTransferMode(Frelon::FrameTransferMode  ftm);
	void getFrameTransferMode(Frelon::FrameTransferMode& ftm /Out/);

	static std::string getInputChanModeName(Frelon::FrameTransferMode ftm, 
						Frelon::InputChan input_chan);

	void getFrameDim(FrameDim& frame_dim /Out/);

	bool isChanActive(Frelon::InputChan curr, Frelon::InputChan chan);

	void checkFlip(Flip& flip /In,Out/);
	void setFlip(const Flip& flip);
	void getFlip(Flip& flip /Out/);

	void checkBin(Bin& bin /In,Out/);
	void setBin(const Bin& bin);
	void getBin(Bin& bin /Out/);

	void setRoiMode(Frelon::RoiMode  roi_mode);
	void getRoiMode(Frelon::RoiMode& roi_mode /Out/);

	void checkRoi(const Roi& set_roi, Roi& hw_roi /Out/);
	void setRoi(const Roi& set_roi);
	void getRoi(Roi& hw_roi /Out/);

	void setRoiBinOffset(const Point& roi_bin_offset);
	void getRoiBinOffset(Point& roi_bin_offset /Out/);

	void setTrigMode(TrigMode  trig_mode);
	void getTrigMode(TrigMode& trig_mode /Out/);
	
	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time /Out/);

	void setShutMode(Frelon::ShutMode  shut_mode);
	void getShutMode(Frelon::ShutMode& shut_mode /Out/);

	void setShutCloseTime(double  shut_time);
	void getShutCloseTime(double& shut_time /Out/);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time /Out/);

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames /Out/);

	void getStatus(Frelon::Status& status /Out/);
	bool waitStatus(Frelon::Status& status /In,Out/, Frelon::Status mask, 
			double timeout);

	void start();
	void stop();

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);

};


}; // namespace Frelon
