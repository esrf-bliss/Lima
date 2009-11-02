#include "FrelonInterface.h"
#include <iostream>

using namespace lima;
using namespace std;

void test_frelon_spectroscopy()
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

	cam.setInputChan(Frelon::Chan34);

	Bin bin(1, 64);
	hw_bin->setBin(bin);

	Roi roi(Point(0, 30), Size(2048, 1));
	Roi real_roi;
	hw_roi->checkRoi(roi, real_roi);

}

int main(int argc, char *argv[])
{
	try {
		test_frelon_spectroscopy();
	} catch (Exception& e) {
		cerr << "LIMA Exception: " << e << endl;
	}
	return 0;
}
