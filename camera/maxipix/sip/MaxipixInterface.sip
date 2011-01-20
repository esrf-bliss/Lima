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

namespace Maxipix
{

%TypeHeaderCode

#include "HwInterface.h"
#include "EspiaBufferMgr.h"
#include "MaxipixInterface.h"
#include "MaxipixDet.h"
#include "PriamAcq.h"

using namespace lima;
%End


class DetInfoCtrlObj : HwDetInfoCtrlObj
{
  public:
    DetInfoCtrlObj(Maxipix::MaxipixDet& det);
    virtual ~DetInfoCtrlObj();

    virtual void getMaxImageSize(Size& size /Out/);
    virtual void getDetectorImageSize(Size& size /Out/);

    virtual void getDefImageType(ImageType& image_type /Out/);
    virtual void getCurrImageType(ImageType& image_type /Out/);
    virtual void setCurrImageType(ImageType image_type);

    virtual void getPixelSize(double& size /Out/);
    virtual void getDetectorType(std::string& type /Out/);
    virtual void getDetectorModel(std::string& model /Out/);

    virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
    virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

};

class BufferCtrlObj : HwBufferCtrlObj
{
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
  public:
    SyncCtrlObj(Espia::Acq& acq, Maxipix::PriamAcq& priam, 
		Maxipix::BufferCtrlObj& buffer_mgr);
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

    virtual void getValidRanges(ValidRangesType& valid_ranges /Out/);

};

class ShutterCtrlObj : HwShutterCtrlObj
{
  public:
	ShutterCtrlObj(Maxipix::PriamAcq& priam);
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
 public:
	Interface(Espia::Acq& acq, BufferCtrlMgr& buffer_mgr, 
		  Maxipix::PriamAcq& acq, Maxipix::MaxipixDet& det);
	virtual ~Interface();

//	virtual void getCapList(CapList&) const;
        virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const;

	virtual void reset(HwInterface::ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(HwInterface::StatusType& status /Out/);
	virtual int getNbHwAcquiredFrames();
	void updateValidRanges();	
};

}; // namespace Maxipix

