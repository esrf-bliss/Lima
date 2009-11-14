#include "FrelonInterface.h"
#include <iostream>

using namespace lima;
using namespace std;

DEB_GLOBAL(DebModTest);

void test_frelon_spectroscopy()
{
	DEB_GLOBAL_FUNCT();

//	DebParams::disableModuleFlags(DebModEspiaSerial | DebModCameraCom);

	Espia::Dev dev(0);
	Espia::Acq acq(dev);
	Espia::BufferMgr buffer_cb_mgr(acq);
	Espia::SerialLine ser_line(dev);
	Frelon::Camera cam(ser_line);
	BufferCtrlMgr buffer_mgr(buffer_cb_mgr);

	DEB_TRACE() << "Creating the Hw Interface ...";
	Frelon::Interface hw_inter(acq, buffer_mgr, cam);
	DEB_TRACE() << "Done!";

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

	DEB_TRACE() << "Setting Chans 3 & 4";
	cam.setInputChan(Frelon::Chan34);

	Bin bin(1, 64);
	DEB_TRACE() << "Setting binning to " << bin;
	hw_bin->setBin(bin);

	Roi roi(Point(0, 30), Size(2048, 1));
	DEB_TRACE() << "Checking " << DEB_VAR1(roi);
	Roi real_roi;
	hw_roi->checkRoi(roi, real_roi);
	DEB_TRACE() << "Got " << DEB_VAR1(real_roi);

	DEB_TRACE() << "Setting " << DEB_VAR1(roi);
	hw_roi->setRoi(roi);
	hw_roi->getRoi(real_roi);
	DEB_TRACE() << "Got " << DEB_VAR1(real_roi);


}

int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	try {
		test_frelon_spectroscopy();
	} catch (Exception& e) {
		DEB_ERROR() << "LIMA Exception: " << e;
	}
	return 0;
}
