#include "SimuHwInterface.h"
#include "BufferSave.h"

#include <iostream>

using namespace lima;
using namespace std;

class TestFrameCallback : public HwFrameCallback
{
public:
	TestFrameCallback(SimuHwInterface& simu_hw, BufferSave& buffer_save) 
		: m_simu_hw(simu_hw), m_buffer_save(buffer_save) {}
protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info);
private:
	SimuHwInterface& m_simu_hw;
	BufferSave& m_buffer_save;
};

bool TestFrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	HwInterface::Status status;
	m_simu_hw.getStatus(status);

	cout << "In callback:" << endl
	     << "  frame_info=" << frame_info << endl
	     << "  status=" << status << endl;
	m_buffer_save.writeFrame(frame_info);
	return true;
}

void print_status(SimuHwInterface& simu_hw)
{
	HwInterface::Status status;

	simu_hw.getStatus(status);
	cout << "status=" << status << endl;
}

void test_simu_hw_interface()
{
	Simulator simu;
	SimuHwInterface simu_hw(simu);
	BufferSave buffer_save(BufferSave::EDF);
	TestFrameCallback cb(simu_hw, buffer_save);

	HwDetInfoCtrlObj *hw_det_info;
	simu_hw.getHwCtrlObj(hw_det_info);

	HwBufferCtrlObj *hw_buffer;
	simu_hw.getHwCtrlObj(hw_buffer);

	HwSyncCtrlObj *hw_sync;
	simu_hw.getHwCtrlObj(hw_sync);

	HwBinCtrlObj *hw_bin;
	simu_hw.getHwCtrlObj(hw_bin);

	Size size;
	hw_det_info->getMaxImageSize(size);
	ImageType image_type;
	hw_det_info->getCurrImageType(image_type);
	FrameDim frame_dim(size, image_type);

	Bin bin(1);
	hw_bin->setBin(bin);

	FrameDim effect_frame_dim = frame_dim / bin;
	hw_buffer->setFrameDim(effect_frame_dim);
	hw_buffer->setNbBuffers(10);
	hw_buffer->registerFrameCallback(cb);

	print_status(simu_hw);
	simu_hw.startAcq();
	print_status(simu_hw);
	simu_hw.stopAcq();
	print_status(simu_hw);

	hw_sync->setExpTime(5);

	print_status(simu_hw);
	simu_hw.startAcq();
	print_status(simu_hw);
	simu_hw.stopAcq();
	print_status(simu_hw);

	hw_sync->setExpTime(2);
	hw_sync->setNbFrames(3);

	print_status(simu_hw);
	simu_hw.startAcq();
	print_status(simu_hw);
	simu_hw.stopAcq();
	print_status(simu_hw);

	bool raised_exception = false;
	try {
		bin = Bin(2, 1);
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

	print_status(simu_hw);
	simu_hw.startAcq();
	print_status(simu_hw);
	simu_hw.stopAcq();
	print_status(simu_hw);
}

int main(int argc, char *argv[])
{
	try {
		test_simu_hw_interface();
	} catch (Exception e) {
		cerr << "LIMA Exception:" << e << endl;
	}

	return 0;
}
