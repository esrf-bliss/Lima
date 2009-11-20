#include "FrelonInterface.h"
#include "CtControl.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtSaving.h"
#include "AcqState.h"

using namespace lima;

DEB_GLOBAL(DebModTest);

class ImageStatusCallback : public CtControl::ImageStatusCallback
{
	DEB_CLASS(DebModTest, "ImageStatusCallback");

public:
	ImageStatusCallback(CtControl& ct, AcqState& acq_state);
	virtual ~ImageStatusCallback();

protected:
	virtual void imageStatusChanged(
				const CtControl::ImageStatus& img_status);

private:
	CtControl& m_ct;
	AcqState& m_acq_state;

	int m_nb_frames;
};

ImageStatusCallback::ImageStatusCallback(CtControl& ct, AcqState& acq_state)
	: m_ct(ct), m_acq_state(acq_state)
{
	DEB_CONSTRUCTOR();
}

ImageStatusCallback::~ImageStatusCallback()
{
	DEB_DESTRUCTOR();
}

void ImageStatusCallback::imageStatusChanged(
				const CtControl::ImageStatus& img_status)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(img_status);

	int last_acq_frame_nb = img_status.LastImageAcquired;
	int last_saved_frame_nb = img_status.LastImageSaved;

	if (last_acq_frame_nb == 0) {
		CtAcquisition *ct_acq = m_ct.acquisition();
		ct_acq->getAcqNbFrames(m_nb_frames);
	}

	if (last_saved_frame_nb == m_nb_frames - 1) {
		DEB_TRACE() << "Acquisition finished!";
		m_acq_state.set(AcqState::Finished);
	}
}

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

	AutoPtr<CtControl> ct = new CtControl(&hw_inter);
	CtAcquisition *ct_acq = ct->acquisition();
	CtSaving *ct_save = ct->saving();
	CtImage *ct_image = ct->image();
//	CtBuffer *ct_buffer = ct->buffer();

	AcqState acq_state;
	ImageStatusCallback img_status_cb(*ct, acq_state);
	ct->registerImageStatusCallback(img_status_cb);

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

	int nb_frames;
	ct_acq->getAcqNbFrames(nb_frames);
	DEB_TRACE() << "Default " << DEB_VAR1(nb_frames);

	ct_save->setDirectory(".");
 	ct_save->setPrefix("img");
	ct_save->setSuffix(".edf");
	ct_save->setNextNumber(0);
	ct_save->setFormat(CtSaving::EDF);
	ct_save->setSavingMode(CtSaving::AutoFrame);
	ct_save->setFramesPerFile(1);

	DEB_TRACE() << "Preparing acquisition";
	ct->prepareAcq();
	acq_state.set(AcqState::Running);
	DEB_TRACE() << "Starting acquisition";
   	ct->startAcq();
	acq_state.waitNot(AcqState::Running);
	DEB_TRACE() << "Acquisition finished";

	exp_time = 1e-6;
	DEB_TRACE() << "Setting " << DEB_VAR1(exp_time);
	ct_acq->setAcqExpoTime(exp_time);

	nb_frames = 200;
	DEB_TRACE() << "Setting " << DEB_VAR1(nb_frames);
	ct_acq->setAcqNbFrames(nb_frames);
	
	DEB_TRACE() << "Preparing acquisition";
	ct->prepareAcq();
	acq_state.set(AcqState::Running);
	DEB_TRACE() << "Starting acquisition";
   	ct->startAcq();
	acq_state.waitNot(AcqState::Running);
	DEB_TRACE() << "Acquisition finished";

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
