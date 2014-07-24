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
class DetInfoCtrlObj : HwDetInfoCtrlObj
{
%TypeHeaderCode
#include "SimulatorInterface.h"
%End

 public:
	DetInfoCtrlObj(Simulator::Camera& simu);
	virtual ~DetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size /Out/);
	virtual void getDetectorImageSize(Size& det_image_size /Out/);

	virtual void getDefImageType(ImageType& def_image_type /Out/);
	virtual void getCurrImageType(ImageType& curr_image_type /Out/);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& x_size /Out/,double &y_size /Out/);
	virtual void getDetectorType(std::string& det_type /Out/);
	virtual void getDetectorModel(std::string& det_model /Out/);

	virtual void registerMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb);
	virtual void unregisterMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb);
};

class SyncCtrlObj : HwSyncCtrlObj
{
%TypeHeaderCode
#include "SimulatorInterface.h"
%End

 public:
	SyncCtrlObj(Simulator::Camera& simu);
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

	virtual void getValidRanges(HwSyncCtrlObj::ValidRangesType& 
	                                                   valid_ranges /Out/);

};


class BinCtrlObj : HwBinCtrlObj
{
%TypeHeaderCode
#include "SimulatorInterface.h"
%End

 public:
	BinCtrlObj(Simulator::Camera& simu);
	virtual ~BinCtrlObj();

	virtual void setBin(const Bin& bin);
	virtual void getBin(Bin& bin /Out/);
	virtual void checkBin(Bin& bin /In,Out/);

};


class Interface : HwInterface
{
%TypeHeaderCode
#include "SimulatorInterface.h"
%End

 public:
	Interface(Simulator::Camera& simu /KeepReference/);
	virtual ~Interface();

//	virtual const CapList& getCapList() const;
	virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(HwInterface::StatusType& status /Out/);
	virtual int getNbHwAcquiredFrames();

};

};