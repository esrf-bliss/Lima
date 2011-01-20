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
#include "FrelonInterface.h"
#include "PoolThreadMgr.h"
#include "HwBufferSave.h"
#include "Data.h"
#include "TaskMgr.h"
#include "TaskEventCallback.h"
#include "SoftRoi.h"
#ifdef WITH_SPS_IMAGE
#include "CtSpsImage.h"
#endif
#include "AcqState.h"

#include <iostream>

using namespace lima;
using namespace std;
using namespace Tasks;

DEB_GLOBAL(DebModTest);


//*********************************************************************
// SoftRoiCallback
//*********************************************************************

class SoftRoiCallback : public TaskEventCallback
{
	DEB_CLASS(DebModTest, "SoftRoiCallback");

public:
#ifdef WITH_SPS_IMAGE
	SoftRoiCallback(Frelon::Interface& hw_inter, CtSpsImage& m_sps_image, 
			HwBufferSave& buffer_save, AcqState& acq_state);
#else
	SoftRoiCallback(Frelon::Interface& hw_inter,HwBufferSave& buffer_save,
			AcqState& acq_state);
#endif
	~SoftRoiCallback();

	virtual void finished(Data& data);
private:
	void data2FrameInfo(Data& data, HwFrameInfoType& finfo, 
			    FrameDim& fdim);

	Frelon::Interface& m_hw_inter;
#ifdef WITH_SPS_IMAGE
	CtSpsImage& m_sps_image;
#endif
	HwBufferSave& m_buffer_save;
	AcqState& m_acq_state;
};
#ifdef WITH_SPS_IMAGE
SoftRoiCallback::SoftRoiCallback(Frelon::Interface& hw_inter, 
				 CtSpsImage& sps_image,
				 HwBufferSave& buffer_save,
				 AcqState& acq_state)
	: m_hw_inter(hw_inter), m_sps_image(sps_image), 
	  m_buffer_save(buffer_save), m_acq_state(acq_state) 
{
	DEB_CONSTRUCTOR();
}
#else
SoftRoiCallback::SoftRoiCallback(Frelon::Interface& hw_inter, 
				 HwBufferSave& buffer_save,
				 AcqState& acq_state)
	: m_hw_inter(hw_inter),
	  m_buffer_save(buffer_save), m_acq_state(acq_state) 
{
	DEB_CONSTRUCTOR();
}
#endif
SoftRoiCallback::~SoftRoiCallback()
{
	DEB_DESTRUCTOR();
}

void SoftRoiCallback::data2FrameInfo(Data& data, HwFrameInfoType& finfo,
				     FrameDim& fdim)
{
	DEB_MEMBER_FUNCT();

	ImageType image_type;
	switch (data.type) {
	case Data::INT8:
	case Data::UINT8:
		image_type = Bpp8; break;
	case Data::INT16:
	case Data::UINT16:
		image_type = Bpp16; break;
	case Data::INT32:
	case Data::UINT32:
		image_type = Bpp32; break;
	default:
		THROW_HW_ERROR(InvalidValue) << "Unknown " 
					     << DEB_VAR1(data.type);
	}

	HwBufferCtrlObj *buffer_ctrl;
	m_hw_inter.getHwCtrlObj(buffer_ctrl);
	Timestamp start_ts;
	buffer_ctrl->getStartTimestamp(start_ts);

	fdim = FrameDim(data.width, data.height, image_type);
	int valid_pixels = Point(fdim.getSize()).getArea();

	finfo = HwFrameInfoType(data.frameNumber, data.data(), &fdim,
				start_ts - Timestamp::now(), valid_pixels,
				HwFrameInfoType::Managed);
}

void SoftRoiCallback::finished(Data& data)
{
	DEB_MEMBER_FUNCT();
#ifdef WITH_SPS_IMAGE
	m_sps_image.frameReady(data);
#endif
	HwFrameInfoType finfo;
	FrameDim fdim;
	data2FrameInfo(data, finfo, fdim);
	m_buffer_save.writeFrame(finfo);

	HwSyncCtrlObj *hw_sync;
	m_hw_inter.getHwCtrlObj(hw_sync);
	int nb_frames;
	hw_sync->getNbFrames(nb_frames);
	if (data.frameNumber == nb_frames - 1)
		m_acq_state.set(AcqState::Finished);

}


//*********************************************************************
// TestFrameCallback
//*********************************************************************

class TestFrameCallback : public HwFrameCallback
{
	DEB_CLASS(DebModTest, "TestFrameCallback");

public:
#ifdef WITH_SPS_IMAGE
	TestFrameCallback(Frelon::Interface& hw_inter, Roi& soft_roi,
			  CtSpsImage& sps_image, HwBufferSave& buffer_save, 
			  AcqState& acq_state);
#else
	TestFrameCallback(Frelon::Interface& hw_inter, Roi& soft_roi,
			  HwBufferSave& buffer_save, 
			  AcqState& acq_state);
#endif
	~TestFrameCallback();

protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info);

private:
	void frameInfo2Data(const HwFrameInfoType& frame_info, Data& data);

	Frelon::Interface& m_hw_inter;
	Roi& m_soft_roi;
	SoftRoi *m_roi_task;
	SoftRoiCallback *m_roi_cb;
};

TestFrameCallback::TestFrameCallback(Frelon::Interface& hw_inter, 
				     Roi& soft_roi,
#ifdef WITH_SPS_IMAGE
				     CtSpsImage& sps_image,
#endif
				     HwBufferSave& buffer_save, 
				     AcqState& acq_state) 
	: m_hw_inter(hw_inter), m_soft_roi(soft_roi)
{
	DEB_CONSTRUCTOR();
	m_roi_cb = new SoftRoiCallback(hw_inter,
#ifdef WITH_SPS_IMAGE
				       sps_image,
#endif
				       buffer_save, 
				       acq_state);
	m_roi_task = new SoftRoi();
}

TestFrameCallback::~TestFrameCallback()
{
	DEB_DESTRUCTOR();
}

void TestFrameCallback::frameInfo2Data(const HwFrameInfoType& frame_info, 
				       Data& data)
{
	DEB_MEMBER_FUNCT();

	data.frameNumber = frame_info.acq_frame_nb;
	const Size &aSize = frame_info.frame_dim.getSize();
	data.width = aSize.getWidth();
	data.height = aSize.getHeight();

	ImageType image_type = frame_info.frame_dim.getImageType();
	switch (image_type) {
	case Bpp8:
		data.type = Data::UINT8; break;
	case Bpp16:
		data.type = Data::UINT16; break;
	case Bpp32:
		data.type = Data::UINT32; break;
	default:
		THROW_HW_ERROR(InvalidValue) << "Invalid "
					     << DEB_VAR1(image_type);
	}
	
	Buffer *buffer = new Buffer();
	buffer->owner = Buffer::MAPPED;
	buffer->data = (void*)frame_info.frame_ptr;
	data.setBuffer(buffer);
	buffer->unref();
}



bool TestFrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();

	int nb_acq_frames = m_hw_inter.getNbAcquiredFrames();
	HwInterface::Status status;
	m_hw_inter.getStatus(status);

	DEB_TRACE() << DEB_VAR3(frame_info, nb_acq_frames, status);

	Data data;
	frameInfo2Data(frame_info, data);

	if (m_soft_roi.isActive()) {
		Point tl = m_soft_roi.getTopLeft();
		Point br = m_soft_roi.getBottomRight();
		m_roi_task->setRoi(tl.x, br.x, tl.y, br.y);

		m_roi_task->setEventCallback(m_roi_cb);

		TaskMgr *task_mgr = new TaskMgr();
		task_mgr->setLinkTask(0, m_roi_task);
		task_mgr->setInputData(data);

		DEB_TRACE() << "adding SoftRoi task!";
		PoolThreadMgr::get().addProcess(task_mgr);
	} else {
		m_roi_cb->finished(data);
	}

	return true;
}


//*********************************************************************
// MaxImageSizeCallback
//*********************************************************************

class MaxImageSizeCallback : public HwMaxImageSizeCallback
{
	DEB_CLASS(DebModTest, "MaxImageSizeCallback");

public:
	MaxImageSizeCallback();
	virtual ~MaxImageSizeCallback();

protected:
	virtual void maxImageSizeChanged(const Size& size,
					 ImageType image_type);
};

MaxImageSizeCallback::MaxImageSizeCallback()
{
	DEB_CONSTRUCTOR();
}

MaxImageSizeCallback::~MaxImageSizeCallback()
{
	DEB_DESTRUCTOR();
}

void MaxImageSizeCallback::maxImageSizeChanged(const Size& size,
					       ImageType image_type)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(size, image_type);
}


//*********************************************************************
// print_status
//*********************************************************************

void print_status(Frelon::Interface& hw_inter)
{
	DEB_GLOBAL_FUNCT();

	HwInterface::Status status;
	hw_inter.getStatus(status);

	DEB_PARAM() << DEB_VAR1(status);
}

//*********************************************************************
// set_hw_roi
//*********************************************************************

void set_hw_roi(HwRoiCtrlObj *hw_roi, const Roi& set_roi, Roi& real_roi,
		Roi& soft_roi)
{
	DEB_GLOBAL_FUNCT();

	hw_roi->setRoi(set_roi);
	hw_roi->getRoi(real_roi);

	if (real_roi == set_roi) {
		soft_roi.reset();
	} else if (real_roi.isActive() && !set_roi.isActive()) {
		soft_roi = real_roi;
	} else {
		soft_roi = real_roi.subRoiAbs2Rel(set_roi);
	}

	DEB_TRACE() << DEB_VAR3(set_roi, real_roi, soft_roi);
}


//*********************************************************************
// check_sps_image_frame_dim
//*********************************************************************

void check_sps_image_frame_dim(HwInterface& hw_inter, 
			       Bin& bin, Roi& set_roi
#ifdef WITH_SPS_IMAGE
			       ,CtSpsImage& sps_image)
#else
  )
#endif
{
	DEB_GLOBAL_FUNCT();

	Size size;
	if (set_roi.isEmpty()) {
		HwDetInfoCtrlObj *hw_det_info;
		hw_inter.getHwCtrlObj(hw_det_info);
		hw_det_info->getMaxImageSize(size);
		size /= bin;
	} else {
		size = set_roi.getSize();
	}

	HwBufferCtrlObj *hw_buffer;
	hw_inter.getHwCtrlObj(hw_buffer);
	FrameDim frame_dim;
	hw_buffer->getFrameDim(frame_dim);
	frame_dim.setSize(size);
	DEB_TRACE() << DEB_VAR1(frame_dim);
#ifdef WITH_SPS_IMAGE
	sps_image.prepare(frame_dim);
#endif
}


//*********************************************************************
// print_deb_flags
//*********************************************************************

void print_deb_flags()
{
	DEB_GLOBAL_FUNCT();

	DebParams::Flags TypeFlags   = DebParams::getTypeFlags();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(TypeFlags));
	DebParams::Flags FormatFlags = DebParams::getFormatFlags();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(FormatFlags));
	DebParams::Flags ModuleFlags = DebParams::getModuleFlags();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(ModuleFlags));

	typedef DebParams::NameList NameList;
	NameList TypeFlagsNameList   = DebParams::getTypeFlagsNameList();
	DEB_PARAM() << DEB_VAR1(TypeFlagsNameList);
	NameList FormatFlagsNameList = DebParams::getFormatFlagsNameList();
	DEB_PARAM() << DEB_VAR1(FormatFlagsNameList);
	NameList ModuleFlagsNameList = DebParams::getModuleFlagsNameList();
	DEB_PARAM() << DEB_VAR1(ModuleFlagsNameList);
}


//*********************************************************************
// test_frelon_hw_inter
//*********************************************************************

void test_frelon_hw_inter(bool do_reset)
{
	DEB_GLOBAL_FUNCT();

	print_deb_flags();
	
	Espia::Dev dev(0);
	Espia::Acq acq(dev);
	Espia::BufferMgr buffer_cb_mgr(acq);
	Espia::SerialLine ser_line(dev);
	Frelon::Camera cam(ser_line);
	BufferCtrlMgr buffer_mgr(buffer_cb_mgr);

	DebParams::disableModuleFlags(DebModEspiaSerial);
	print_deb_flags();

	DEB_TRACE() << "Creating the Hw Interface ... ";
	Frelon::Interface hw_inter(acq, buffer_mgr, cam);
	DEB_TRACE() << "Done!";

	HwBufferSave buffer_save(HwBufferSave::EDF, "img", 0, ".edf", true, 1);

#ifdef WITH_SPS_IMAGE
	CtSpsImage sps_image;
	sps_image.setNames("_ccd_ds_", "frelon_live");
#endif
	MaxImageSizeCallback mis_cb;

	Roi soft_roi;
	AcqState acq_state;
	TestFrameCallback cb(hw_inter, soft_roi, 
#ifdef WITH_SPS_IMAGE
			     sps_image,
#endif
			     buffer_save, 
			     acq_state);
	
	HwDetInfoCtrlObj *hw_det_info;
	hw_inter.getHwCtrlObj(hw_det_info);

	HwBufferCtrlObj *hw_buffer;
	hw_inter.getHwCtrlObj(hw_buffer);

	HwSyncCtrlObj *hw_sync;
	hw_inter.getHwCtrlObj(hw_sync);

	HwBinCtrlObj *hw_bin;
	hw_inter.getHwCtrlObj(hw_bin);

	HwRoiCtrlObj *hw_roi;
	hw_inter.getHwCtrlObj(hw_roi);

	if (do_reset) {
		DEB_TRACE() << "Reseting the hardware ... ";
		hw_inter.reset(HwInterface::HardReset);
		DEB_TRACE() << "Done!";
	}

	Size size;
	hw_det_info->getMaxImageSize(size);
	ImageType image_type;
	hw_det_info->getCurrImageType(image_type);
	FrameDim frame_dim(size, image_type);
	hw_det_info->registerMaxImageSizeCallback(mis_cb);

	DEB_TRACE() << "Setting FTM";
	cam.setFrameTransferMode(Frelon::FTM);
	DEB_TRACE() << "Setting FFM";
	cam.setFrameTransferMode(Frelon::FFM);
	
	Bin bin(1);
	hw_bin->setBin(bin);

	Roi set_roi, real_roi;
	set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);
	
	FrameDim effect_frame_dim = frame_dim / bin;
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);
	hw_buffer->registerFrameCallback(cb);
	check_sps_image_frame_dim(hw_inter, bin, set_roi
#ifdef WITH_SPS_IMAGE
, sps_image);
#else
	);
#endif

	print_status(hw_inter);
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_sync->setExpTime(5);

	print_status(hw_inter);
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_sync->setExpTime(2);
	hw_sync->setNbFrames(3);

	print_status(hw_inter);
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	bool raised_exception = false;
	try {
		bin = Bin(16, 16);
		hw_bin->setBin(bin);
	} catch (Exception) {
		raised_exception = true;
	}
	if (raised_exception) {
		DEB_TRACE() << "Exception was raised OK!";
	} else {
		THROW_HW_ERROR(Error) << "Did not raise bad bin exception";
	}

	bin = Bin(2, 2);
	hw_bin->setBin(bin);
	effect_frame_dim = frame_dim / bin;
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);
	check_sps_image_frame_dim(hw_inter, bin, set_roi
#ifdef WITH_SPS_IMAGE
, sps_image);
#else
	);
#endif

	print_status(hw_inter);
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	set_roi = Roi(Point(256, 256), Size(512, 512));
	set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);
	effect_frame_dim.setSize(real_roi.getSize());
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);
	check_sps_image_frame_dim(hw_inter, bin, set_roi
#ifdef WITH_SPS_IMAGE
, sps_image);
#else
	);
#endif


	print_status(hw_inter);
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	DebParams::disableTypeFlags(DebTypeFunct);
	print_deb_flags();

	set_roi = Roi(Point(267, 267), Size(501, 501));
	set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);
	effect_frame_dim.setSize(real_roi.getSize());
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);
	check_sps_image_frame_dim(hw_inter, bin, set_roi
#ifdef WITH_SPS_IMAGE
, sps_image);
#else
	);
#endif

	print_status(hw_inter);
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	DebParams::enableTypeFlags  (DebParams::AllFlags);
	DebParams::enableFormatFlags(DebParams::AllFlags);
	DebParams::enableModuleFlags(DebParams::AllFlags);
	print_deb_flags();
}


//*********************************************************************
// main
//*********************************************************************

int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	try {
		bool do_reset = (argc > 1) && (string(argv[1]) == "reset");
		test_frelon_hw_inter(do_reset);
	} catch (Exception e) {
		DEB_ERROR() << "LIMA Exception:" << e;
	}

	return 0;
}
