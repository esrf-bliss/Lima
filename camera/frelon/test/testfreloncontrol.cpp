#include "FrelonInterface.h"
#include "CtControl.h"
#include "CtAcquisition.h"
#include "CtImage.h"
#include "CtSaving.h"
#include "AcqState.h"

using namespace lima;
using namespace std;

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
	: m_ct(ct), m_acq_state(acq_state), m_nb_frames(0)
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

	if ((last_acq_frame_nb == m_nb_frames - 1) &&
	    (m_acq_state.get() == AcqState::Acquiring)) {
		DEB_ALWAYS() << "All frames acquired!";
		m_acq_state.set(AcqState::Saving);
	}

	if (last_saved_frame_nb == m_nb_frames - 1) {
		DEB_ALWAYS() << "All frames saved!";
		m_acq_state.set(AcqState::Finished);
	}
}

class FrelonAcq
{
	DEB_CLASS(DebModTest, "FrelonAcq");
public:
	FrelonAcq(int espia_dev_nb);
	~FrelonAcq();

	void initSaving(string dir, string prefix, string suffix, int idx, 
			CtSaving::FileFormat fmt, CtSaving::SavingMode mode,
			int frames_per_file);

	void setExpTime(double exp_time);
	void setNbAcqFrames(int nb_acq_frames);

	void start();
	void wait();
	void run();

	void setBin(Bin& bin);
	void setRoi(Roi& roi);

private:
	void printDefaults();

	Espia::Dev		m_edev;
	Espia::Acq		m_acq;
	Espia::BufferMgr	m_buffer_cb_mgr;
	Espia::SerialLine	m_eserline;
	Frelon::Camera		m_cam;
	BufferCtrlMgr		m_buffer_mgr;
	Frelon::Interface	m_hw_inter;
	AcqState		m_acq_state;

	CtControl		*m_ct;
	CtAcquisition		*m_ct_acq;
	CtSaving		*m_ct_saving;
	CtImage			*m_ct_image;
	CtBuffer		*m_ct_buffer;

	ImageStatusCallback	*m_img_status_cb;
};

FrelonAcq::FrelonAcq(int espia_dev_nb)
	: m_edev(espia_dev_nb), 
	  m_acq(m_edev), 
	  m_buffer_cb_mgr(m_acq), 
	  m_eserline(m_edev),
	  m_cam(m_eserline), 
	  m_buffer_mgr(m_buffer_cb_mgr),
	  m_hw_inter(m_acq, m_buffer_mgr, m_cam),
	  m_ct(NULL), m_img_status_cb(NULL)
{
	DEB_CONSTRUCTOR();

	AutoPtr<CtControl> ct = new CtControl(&m_hw_inter);

	m_ct_acq    = ct->acquisition();
	m_ct_saving = ct->saving();
	m_ct_image  = ct->image();
	m_ct_buffer = ct->buffer();

	printDefaults();

	AutoPtr<ImageStatusCallback> img_status_cb;
	img_status_cb = new ImageStatusCallback(*ct, m_acq_state);
	ct->registerImageStatusCallback(*img_status_cb);

	DEB_TRACE() << "All is OK!";
	m_ct = ct.forget();
	m_img_status_cb = img_status_cb.forget();
}

FrelonAcq::~FrelonAcq()
{
	DEB_DESTRUCTOR();

	delete m_img_status_cb;
	delete m_ct;
}

void FrelonAcq::printDefaults()
{
	DEB_MEMBER_FUNCT();

	AcqMode acq_mode;
	m_ct_acq->getAcqMode(acq_mode);
	DEB_TRACE() << "Default " << DEB_VAR1(acq_mode);

	ImageType image_type;
	m_ct_image->getImageType(image_type);
	DEB_TRACE() << "Default " << DEB_VAR1(image_type);

	Size max_size;
	m_ct_image->getMaxImageSize(max_size);
	DEB_TRACE() << "Default " << DEB_VAR1(max_size);

	CtImage::ImageOpMode img_op_mode;
	m_ct_image->getMode(img_op_mode);
	DEB_TRACE() << "Default " << DEB_VAR1(img_op_mode);

	Bin bin;
	m_ct_image->getBin(bin);
	DEB_TRACE() << "Default " << DEB_VAR1(bin);

	Roi roi;
	m_ct_image->getRoi(roi);
	DEB_TRACE() << "Default " << DEB_VAR1(roi);
	
	double exp_time;
	m_ct_acq->getAcqExpoTime(exp_time);
	DEB_TRACE() << "Default " << DEB_VAR1(exp_time);

	int nb_frames;
	m_ct_acq->getAcqNbFrames(nb_frames);
	DEB_TRACE() << "Default " << DEB_VAR1(nb_frames);

}

void FrelonAcq::start()
{
	DEB_MEMBER_FUNCT();

	m_ct->prepareAcq();
	m_acq_state.set(AcqState::Acquiring);
	DEB_TRACE() << "Starting acquisition";
   	m_ct->startAcq();
}

void FrelonAcq::wait()
{
	DEB_MEMBER_FUNCT();
	m_acq_state.waitNot(AcqState::Acquiring | AcqState::Saving);
	DEB_TRACE() << "Acquisition finished";
}

void FrelonAcq::run()
{
	DEB_MEMBER_FUNCT();
	start();
	wait();
}

void FrelonAcq::initSaving(string dir, string prefix, string suffix, int idx, 
			   CtSaving::FileFormat fmt, CtSaving::SavingMode mode,
			   int frames_per_file)
{
	DEB_MEMBER_FUNCT();

	m_ct_saving->setDirectory(dir);
 	m_ct_saving->setPrefix(prefix);
	m_ct_saving->setSuffix(suffix);
	m_ct_saving->setNextNumber(idx);
	m_ct_saving->setFormat(fmt);
	m_ct_saving->setSavingMode(mode);
	m_ct_saving->setFramesPerFile(frames_per_file);

}

void FrelonAcq::setExpTime(double exp_time)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(exp_time);

	m_ct_acq->setAcqExpoTime(exp_time);
}

void FrelonAcq::setNbAcqFrames(int nb_acq_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_acq_frames);

	m_ct_acq->setAcqNbFrames(nb_acq_frames);
}

void FrelonAcq::setBin(Bin& bin)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(bin);

	m_ct_image->setBin(bin);
}

void FrelonAcq::setRoi(Roi& roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

	m_ct_image->setRoi(roi);

}

void test_frelon_control(bool enable_debug = false)
{
	DEB_GLOBAL_FUNCT();

	if (!enable_debug) {
		DebParams::disableModuleFlags(DebParams::AllFlags);
	}

	DEB_ALWAYS() << "Creating FrelonAcq";
	int espia_dev_nb = 0;
	FrelonAcq acq(espia_dev_nb);
	DEB_ALWAYS() << "Done!";

	acq.initSaving("data", "img", ".edf", 0, CtSaving::EDF, 
		       CtSaving::AutoFrame, 1);

	DEB_ALWAYS() << "First run with default pars";
	acq.run();
	DEB_ALWAYS() << "Done!";

	double exp_time = 1e-6;
	acq.setExpTime(exp_time);

	int nb_acq_frames = 500;
	acq.setNbAcqFrames(nb_acq_frames);

	DEB_ALWAYS() << "Run " << DEB_VAR2(exp_time, nb_acq_frames);
	acq.run();
	DEB_ALWAYS() << "Done!";

	Bin bin(2);
	acq.setBin(bin);

	nb_acq_frames = 5;
	acq.setNbAcqFrames(nb_acq_frames);

	DEB_ALWAYS() << "Run " << DEB_VAR2(bin, nb_acq_frames);
	acq.run();
	DEB_ALWAYS() << "Done!";

	Roi roi = Roi(Point(256, 256), Size(512, 512));
	acq.setRoi(roi);

	DEB_ALWAYS() << "Run " << DEB_VAR1(roi);
	acq.run();
	DEB_ALWAYS() << "Done!";

	roi = Roi(Point(267, 267), Size(501, 501));
	acq.setRoi(roi);

	DEB_ALWAYS() << "Run " << DEB_VAR1(roi);
	acq.run();
	DEB_ALWAYS() << "Done!";

}

int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	string par;
	if (argc > 1)
		par = argv[1];
	bool enable_debug = (par == "debug");

	try {
		test_frelon_control(enable_debug);
	} catch (Exception& e) {
		DEB_ERROR() << "LIMA Exception: " << e;
	}
	return 0;
}
