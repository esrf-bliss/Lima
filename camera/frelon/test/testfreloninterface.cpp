#include "FrelonInterface.h"
#include "PoolThreadMgr.h"
#include "BufferSave.h"
#include "Data.h"
#include "TaskMgr.h"
#include "TaskEventCallback.h"
#include "SoftRoi.h"
#include "AcqFinished.h"

#include <iostream>

using namespace lima;
using namespace std;
using namespace Tasks;

class SoftRoiCallback : public TaskEventCallback
{
public:
	SoftRoiCallback(Frelon::Interface& hw_inter, BufferSave& buffer_save,
			AcqFinished& acq_finished)
		: m_hw_inter(hw_inter), m_buffer_save(buffer_save), 
		  m_acq_finished(acq_finished) {}

	virtual void finished(Data& data);
private:
	void data2FrameInfo(Data& data, HwFrameInfoType& finfo, 
			    FrameDim& fdim);

	Frelon::Interface& m_hw_inter;
	BufferSave& m_buffer_save;
	AcqFinished& m_acq_finished;
};

void SoftRoiCallback::data2FrameInfo(Data& data, HwFrameInfoType& finfo,
				     FrameDim& fdim)
{
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
		throw LIMA_HW_EXC(InvalidValue, "Unknown data type");
	}

	HwBufferCtrlObj *buffer_ctrl;
	m_hw_inter.getHwCtrlObj(buffer_ctrl);
	Timestamp start_ts;
	buffer_ctrl->getStartTimestamp(start_ts);

	fdim = FrameDim(data.width, data.height, image_type);
	int valid_pixels = Point(fdim.getSize()).getArea();

	finfo = HwFrameInfoType(data.frameNumber, data.data(), &fdim,
				start_ts - Timestamp::now(), valid_pixels);
}

void SoftRoiCallback::finished(Data& data)
{
	HwFrameInfoType finfo;
	FrameDim fdim;
	data2FrameInfo(data, finfo, fdim);
	m_buffer_save.writeFrame(finfo);

	HwSyncCtrlObj *hw_sync;
	m_hw_inter.getHwCtrlObj(hw_sync);
	int nb_frames;
	hw_sync->getNbFrames(nb_frames);
	if (data.frameNumber == nb_frames - 1)
		m_acq_finished.signalFinished();

}


class TestFrameCallback : public HwFrameCallback
{
public:
	TestFrameCallback(Frelon::Interface& hw_inter, Roi& soft_roi,
			  BufferSave& buffer_save, AcqFinished& acq_finished) 
		: m_hw_inter(hw_inter), m_soft_roi(soft_roi)
	{
		m_roi_cb = new SoftRoiCallback(hw_inter, buffer_save, 
					       acq_finished);
		m_roi_task = new SoftRoi();
	}

	~TestFrameCallback()
	{
		m_roi_task->unref();
		m_roi_cb->unref();
	}

protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info);

private:
	void frameInfo2Data(const HwFrameInfoType& frame_info, Data& data);

	Frelon::Interface& m_hw_inter;
	Roi& m_soft_roi;
	SoftRoi *m_roi_task;
	SoftRoiCallback *m_roi_cb;
};

void TestFrameCallback::frameInfo2Data(const HwFrameInfoType& frame_info, 
				       Data& data)
{
	data.frameNumber = frame_info.acq_frame_nb;
	const Size &aSize = frame_info.frame_dim->getSize();
	data.width = aSize.getWidth();
	data.height = aSize.getHeight();

	ImageType image_type = frame_info.frame_dim->getImageType();
	switch (image_type) {
	case Bpp8:
		data.type = Data::UINT8; break;
	case Bpp16:
		data.type = Data::UINT16; break;
	case Bpp32:
		data.type = Data::UINT32; break;
	default:
		throw LIMA_HW_EXC(InvalidValue, "Invalid frame image type");
	}
	
	Buffer *buffer = new Buffer();
	buffer->owner = Buffer::MAPPED;
	buffer->data = (void*)frame_info.frame_ptr;
	data.setBuffer(buffer);
	buffer->unref();
}



bool TestFrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	int nb_acq_frames = m_hw_inter.getNbAcquiredFrames();
	HwInterface::Status status;
	m_hw_inter.getStatus(status);

	cout << "In callback:" << endl
	     << "  frame_info=" << frame_info << endl
	     << "  nb_acq_frames=" << nb_acq_frames << endl
	     << "  status=" << status << endl;

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

		cout << "  adding SoftRoi task!" << endl;
		PoolThreadMgr::get().addProcess(task_mgr);
	} else {
		m_roi_cb->finished(data);
	}

	return true;
}

void print_status(Frelon::Interface& hw_inter)
{
	HwInterface::Status status;

	hw_inter.getStatus(status);
	cout << "status=" << status << endl;
}

void set_hw_roi(HwRoiCtrlObj *hw_roi, const Roi& set_roi, Roi& real_roi,
		Roi& soft_roi)
{
	hw_roi->setRoi(set_roi);
	hw_roi->getRoi(real_roi);

	if (real_roi == set_roi) {
		soft_roi.reset();
	} else if (real_roi.isActive() && !set_roi.isActive()) {
		soft_roi = real_roi;
	} else {
		soft_roi = real_roi.subRoiAbs2Rel(set_roi);
	}

	cout << "set_roi=" << set_roi << ", real_roi=" << real_roi << ", "
	     << "soft_roi=" << soft_roi << endl;
}

void print_deb_flags()
{
	DebParams::Flags deb_flags;
	cout << hex << showbase;
	deb_flags = DebParams::getTypeFlags();
	cout << "TypeFlags=" << deb_flags << endl;
	deb_flags = DebParams::getFormatFlags();
	cout << "FormatFlags=" << deb_flags << endl;
	deb_flags = DebParams::getModuleFlags();
	cout << "ModuleFlags=" << deb_flags << endl;
	cout << dec << noshowbase;

	DebParams::NameList name_list;
	name_list = DebParams::getTypeFlagsNameList();
	cout << "TypeFlagsNameList=" << name_list << endl;
	name_list = DebParams::getFormatFlagsNameList();
	cout << "FormatFlagsNameList=" << name_list << endl;
	name_list = DebParams::getModuleFlagsNameList();
	cout << "ModuleFlagsNameList=" << name_list << endl;
}

void test_frelon_hw_inter(bool do_reset)
{
	print_deb_flags();
	
	Espia::Dev dev(0);
	Espia::Acq acq(dev);
	Espia::BufferMgr buffer_cb_mgr(acq);
	Espia::SerialLine ser_line(dev);
	Frelon::Camera cam(ser_line);
	BufferCtrlMgr buffer_mgr(buffer_cb_mgr);

	DebParams::disableModuleFlags(DebModEspiaSerial);
	print_deb_flags();

	cout << "Creating the Hw Interface ... " << endl;
	Frelon::Interface hw_inter(acq, buffer_mgr, cam);
	cout << " Done!" << endl;

	BufferSave buffer_save(BufferSave::EDF, "img", 0, ".edf", true, 1);

	Roi soft_roi;
	AcqFinished acq_finished;
	TestFrameCallback cb(hw_inter, soft_roi, buffer_save, acq_finished);

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
		cout << "Reseting the hardware ... " << endl;
		hw_inter.reset(HwInterface::HardReset);
		cout << "  Done!" << endl;
	}

	Size size;
	hw_det_info->getMaxImageSize(size);
	ImageType image_type;
	hw_det_info->getCurrImageType(image_type);
	FrameDim frame_dim(size, image_type);

	Bin bin(1);
	hw_bin->setBin(bin);

	Roi set_roi, real_roi;
	set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);
	
	FrameDim effect_frame_dim = frame_dim / bin;
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);
	hw_buffer->registerFrameCallback(cb);

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_sync->setExpTime(5);

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_sync->setExpTime(2);
	hw_sync->setNbFrames(3);

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
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
	if (raised_exception)
		cout << "Exception was raised OK!" << endl;
	else
		throw LIMA_HW_EXC(Error, "Did not raise bad bin exception");

	bin = Bin(2, 2);
	hw_bin->setBin(bin);
	effect_frame_dim = frame_dim / bin;
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	set_roi = Roi(Point(256, 256), Size(512, 512));
	set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);
	effect_frame_dim.setSize(real_roi.getSize());
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
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

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbAccFrames(5);
	hw_buffer->setNbBuffers(10);

	hw_sync->setNbFrames(3);

	DebParams::disableModuleFlags(DebModEspia);
	print_deb_flags();

	print_status(hw_inter);
	acq_finished.resetFinished();
	hw_inter.startAcq();
	acq_finished.waitFinished();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	DebParams::enableTypeFlags  (DebParams::AllFlags);
	DebParams::enableFormatFlags(DebParams::AllFlags);
	DebParams::enableModuleFlags(DebParams::AllFlags);
	print_deb_flags();
}

int main(int argc, char *argv[])
{
	try {
		bool do_reset = (argc > 1) && (string(argv[1]) == "reset");
		test_frelon_hw_inter(do_reset);
	} catch (Exception e) {
		cerr << "LIMA Exception:" << e << endl;
	}

	return 0;
}
