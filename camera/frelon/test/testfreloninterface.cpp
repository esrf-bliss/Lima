#include "FrelonInterface.h"
#include "PoolThreadMgr.h"
#include "CtSaving.h"
#include "Data.h"
#include "TaskMgr.h"
#include "TaskEventCallback.h"
#include "SoftRoi.h"

#include <iostream>

using namespace lima;
using namespace std;
using namespace Tasks;

class SoftRoiCallback : public TaskEventCallback
{
public:
	SoftRoiCallback(Frelon::Interface& hw_inter, CtSaving& buffer_save,
			Cond& acq_finished)
		: m_hw_inter(hw_inter), m_buffer_save(buffer_save), 
		  m_acq_finished(acq_finished) {}

	virtual void finished(Data& data);
private:
	Frelon::Interface& m_hw_inter;
	CtSaving& m_buffer_save;
	Cond& m_acq_finished;
};

void SoftRoiCallback::finished(Data& data)
{
	m_buffer_save.frameReady(data);

	HwSyncCtrlObj *hw_sync;
	m_hw_inter.getHwCtrlObj(hw_sync);
	int nb_frames;
	hw_sync->getNbFrames(nb_frames);
	if (data.frameNumber == nb_frames - 1) {
		m_acq_finished.signal();
		m_buffer_save.resetLastFrameNb();
	}

}


class TestFrameCallback : public HwFrameCallback
{
public:
	TestFrameCallback(Frelon::Interface& hw_inter, Roi& soft_roi,
			  CtSaving& buffer_save, Cond& acq_finished) 
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
	Frelon::Interface& m_hw_inter;
	Roi& m_soft_roi;
	SoftRoi *m_roi_task;
	SoftRoiCallback *m_roi_cb;
};

bool TestFrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	int nb_acq_frames = m_hw_inter.getNbAcquiredFrames();
	HwInterface::Status status;
	m_hw_inter.getStatus(status);

	cout << "In callback:" << endl
	     << "  frame_info=" << frame_info << endl
	     << "  nb_acq_frames=" << nb_acq_frames << endl
	     << "  status=" << status << endl;

	Data data = Data();
	data.frameNumber = frame_info.acq_frame_nb;
	const Size &aSize = frame_info.frame_dim->getSize();
	data.width = aSize.getWidth();
	data.height = aSize.getHeight();
	data.type = Data::UINT16;
	
	Buffer *buffer = new Buffer();
	buffer->owner = Buffer::MAPPED;
	buffer->data = (void*)frame_info.frame_ptr;
	data.setBuffer(buffer);
	buffer->unref();

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

void test_frelon_hw_inter(bool do_reset)
{
	Espia::Dev dev(0);
	Espia::Acq acq(dev);
	Espia::BufferMgr buffer_cb_mgr(acq);
	Espia::SerialLine ser_line(dev);
	Frelon::Camera cam(ser_line);
	BufferCtrlMgr buffer_mgr(buffer_cb_mgr);

	cout << "Creating the Hw Interface ... " << endl;
	Frelon::Interface hw_inter(acq, buffer_mgr, cam);
	cout << " Done!" << endl;

	CtControl aControl(NULL);
	CtSaving buffer_save(aControl);
	CtSaving::Parameters saving_par;
	saving_par.directory = ".";
	saving_par.prefix = "img";
	saving_par.suffix = ".edf";
	saving_par.nextNumber = 0;
	saving_par.fileFormat = CtSaving::EDF;
	saving_par.savingMode = CtSaving::AutoFrame;
	saving_par.overwritePolicy = CtSaving::Overwrite;
	saving_par.framesPerFile = 1;
	buffer_save.setParameters(saving_par);

	Roi soft_roi;
	Cond acq_finished;
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
	hw_inter.startAcq();
	acq_finished.wait();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_sync->setExpTime(5);

	print_status(hw_inter);
	hw_inter.startAcq();
	acq_finished.wait();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	hw_sync->setExpTime(2);
	hw_sync->setNbFrames(3);

	print_status(hw_inter);
	hw_inter.startAcq();
	acq_finished.wait();
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
	hw_inter.startAcq();
	acq_finished.wait();
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
	hw_inter.startAcq();
	acq_finished.wait();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

	set_roi = Roi(Point(267, 267), Size(501, 501));
	set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);
	effect_frame_dim.setSize(real_roi.getSize());
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);

	print_status(hw_inter);
	hw_inter.startAcq();
	acq_finished.wait();
	PoolThreadMgr::get().wait();
	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);
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
