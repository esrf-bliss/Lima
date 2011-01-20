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


//class Interface;


class DetInfoCtrlObj : HwDetInfoCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	DetInfoCtrlObj(Frelon::Camera& cam);
	virtual ~DetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size /Out/);
	virtual void getDetectorImageSize(Size& det_image_size /Out/);

	virtual void getDefImageType(ImageType& def_image_type /Out/);
	virtual void getCurrImageType(ImageType& curr_image_type /Out/);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& pixel_size /Out/);
	virtual void getDetectorType(std::string& det_type /Out/);
	virtual void getDetectorModel(std::string& det_model /Out/);

	virtual void registerMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb);
	virtual void unregisterMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb);
};


class BufferCtrlObj : HwBufferCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	BufferCtrlObj(BufferCtrlMgr& buffer_mgr);
	virtual ~BufferCtrlObj();

	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFrameDim(      FrameDim& frame_dim /Out/);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers /Out/);

	virtual void setNbConcatFrames(int  nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames /Out/);

	virtual void getMaxNbBuffers(int& max_nb_buffers /Out/);

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts /Out/);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

};


class SyncCtrlObj : HwSyncCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	SyncCtrlObj(Espia::Acq& acq, Frelon::Camera& cam, 
	            Frelon::BufferCtrlObj& buffer_ctrl);
	virtual ~SyncCtrlObj();

	virtual bool checkTrigMode(TrigMode trig_mode);
	virtual void setTrigMode(TrigMode  trig_mode);
	virtual void getTrigMode(TrigMode& trig_mode /Out/);

	virtual void setExpTime(double  exp_time);
	virtual void getExpTime(double& exp_time /Out/);

	virtual void setLatTime(double  lat_time);
	virtual void getLatTime(double& lat_time /Out/);

	virtual void setNbHwFrames(int  nb_frames);
	virtual void getNbHwFrames(int& nb_frames /Out/);

	virtual void getValidRanges(HwSyncCtrlObj::ValidRangesType& valid_ranges /Out/);

};


class BinCtrlObj : HwBinCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	BinCtrlObj(Frelon::Camera& cam);
	virtual ~BinCtrlObj();

	virtual void setBin(const Bin& bin);
	virtual void getBin(Bin& bin /Out/);
	virtual void checkBin(Bin& bin /In,Out/);
};


class RoiCtrlObj : HwRoiCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	RoiCtrlObj(Frelon::Camera& cam);
	virtual ~RoiCtrlObj();

	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi /Out/);
	virtual void setRoi(const Roi& set_roi);
	virtual void getRoi(Roi& hw_roi /Out/);
};


class FlipCtrlObj : HwFlipCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	FlipCtrlObj(Frelon::Camera& cam);
	virtual ~FlipCtrlObj();

	virtual void setFlip(const Flip& flip);
	virtual void getFlip(Flip& flip /Out/);
	virtual void checkFlip(Flip& flip /In,Out/);
};


class ShutterCtrlObj : HwShutterCtrlObj
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

public:
	ShutterCtrlObj(Frelon::Camera& cam);
	virtual ~ShutterCtrlObj();

	virtual bool checkMode(ShutterMode shut_mode);
	virtual void getModeList(ShutterModeList&  mode_list /Out/);
	virtual void setMode(ShutterMode  shut_mode);
	virtual void getMode(ShutterMode& shut_mode /Out/);

	virtual void setState(bool  shut_open);
	virtual void getState(bool& shut_open /Out/);

	virtual void setOpenTime (double  shut_open_time);
	virtual void getOpenTime (double& shut_open_time /Out/);
	virtual void setCloseTime(double  shut_close_time);
	virtual void getCloseTime(double& shut_close_time /Out/);

};


class Interface : HwInterface
{

%TypeHeaderCode
#include "FrelonInterface.h"
#include <algorithm>
%End

 public:
	Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr, 
		  Frelon::Camera& cam);
	virtual ~Interface();

//	virtual const HwInterface::CapList& getCapList() const;
	virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const;

	virtual void reset(HwInterface::ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(HwInterface::StatusType& status /Out/);
	virtual int getNbHwAcquiredFrames();
};


}; // namespace Frelon
