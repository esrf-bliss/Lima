#include "FrelonInterface.h"
#include "CtControl.h"
#include "CtAcquisition.h"
#include "CtImage.h"

using namespace lima;

DEB_GLOBAL(DebModTest);


void test_frelon_control()
{
	DEB_GLOBAL_FUNCT();

//	DebParams::disableModuleFlags(DebParams::AllFlags);
	DebParams::enableModuleFlags(DebModTest);
	
	Espia::Dev dev(0);
	Espia::Acq acq(dev);
	Espia::BufferMgr buffer_cb_mgr(acq);
	Espia::SerialLine ser_line(dev);
	Frelon::Camera cam(ser_line);
	BufferCtrlMgr buffer_mgr(buffer_cb_mgr);

	DEB_ALWAYS() << "Creating the Hw Interface ...";
	Frelon::Interface hw_inter(acq, buffer_mgr, cam);
	DEB_TRACE() << "Done!";

	CtControl control(&hw_inter);
	CtAcquisition *ct_acq = control.acquisition();
	CtSaving *ct_saving = control.saving();
	CtImage *ct_image = control.image();
	CtBuffer *ct_buffer = control.buffer();

	AcqMode acq_mode;
	ct_acq->getAcqMode(acq_mode);
	DEB_TRACE() << "Default " << DEB_VAR1(acq_mode);

	ImageType image_type;
	ct_image->getImageType(image_type);
	DEB_TRACE() << "Default " << DEB_VAR1(image_type);

	Size max_size;
	ct_image->getMaxImageSize(max_size);
	DEB_TRACE() << "Default " << DEB_VAR1(max_size);

	Bin bin;
	ct_image->getBin(bin);
	DEB_TRACE() << "Default " << DEB_VAR1(bin);

	Roi roi;
	ct_image->getRoi(roi);
	DEB_TRACE() << "Default " << DEB_VAR1(roi);
	
	double exp_time;
	ct_acq->getAcqExpoTime(exp_time);
	DEB_TRACE() << "Default " << DEB_VAR1(exp_time);

	int nb_frame;
	ct_acq->getAcqNbFrames(nb_frame);
	DEB_TRACE() << "Default " << DEB_VAR1(nb_frame);

}

int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	try {
		test_frelon_control();
	} catch (Exception& e) {
		DEB_ERROR() << "LIMA Exception: " << e;
	}
	return 0;
}
