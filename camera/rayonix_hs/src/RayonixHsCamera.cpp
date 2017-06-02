
#include "craydl.h"

#include "RayonixHsCamera.h"

using namespace lima;
using namespace lima::RayonixHs;

Camera::Camera()
	: m_rx_detector(new craydl::RxDetector()),
	  m_acquiring(false) {

	init();

	m_frame_status_cb = new FrameStatusCb(this, m_acquiring);
	//m_frame_status_cb->registerCallbackAcqComplete(&lima::RayonixHs::Camera::acquisitionComplete);
	//m_frame_status_cb->registerCallbackAcqComplete(&acquisitionComplete);
}

void Camera::init() {
	m_rx_detector->Open();

	m_detector_status = DETECTOR_STATUS_IDLE;

	int fast, slow, depth;
	m_rx_detector->GetFrameSize(fast, slow, depth);
	m_max_image_size = Size(fast, slow);

	m_exp_time = 0;
	m_lat_time = 0;
	m_nb_frames = 0;
}

Camera::~Camera() {
	m_rx_detector->EndAcquisition(true);
	m_rx_detector->Close();
	delete m_rx_detector;
	delete m_frame_status_cb;
}

SoftBufferCtrlObj* Camera::getBufferCtrlObj() {
	(SoftBufferCtrlObj *)m_buffer_ctrl_obj;
}

void Camera::getDetectorType(std::string &type) {
	type = "RayonixHs";
}

void Camera::getImageType(ImageType &img_type) {
	img_type = Bpp16;
}

void Camera::setImageType(ImageType img_type) {
	switch (img_type) {
		case Bpp16:
			break;
		default:
			throw LIMA_HW_EXC(InvalidValue,"This image type is not supported.");
	}
}

bool Camera::checkTrigMode(TrigMode mode) {
	switch (mode) {
		case IntTrig:
		case ExtTrigSingle:
		case ExtTrigMult:
		case ExtGate:
			return true;
		default:
			return false;
	}
}

void Camera::getPixelSize(double &x, double &y) {
#warning Fix when library has pixel size method
	x = y = -1.;
}

void Camera::getMaxImageSize(Size& max_image_size) {
	max_image_size = m_max_image_size;
}

void Camera::setNbFrames(int nb_frames) {
	if (nb_frames < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb of frames");

	m_nb_frames = nb_frames;
}

void Camera::getNbFrames(int& nb_frames) {
	nb_frames = m_nb_frames;
}

void Camera::setExpTime(double exp_time) {
	if (exp_time < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid exposure time");

	m_exp_time = exp_time;
}

void Camera::getExpTime(double& exp_time) {
	exp_time = m_exp_time;
}

void Camera::setLatTime(double lat_time) {
	if (lat_time < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid latency time");

	m_lat_time = lat_time;
}

void Camera::getLatTime(double& lat_time) {
	lat_time = m_lat_time;
}

void Camera::setBin(const Bin& bin) {
	m_rx_detector->SetBinning(bin.getX(), bin.getY());
}

void Camera::getBin(Bin& bin) {
	int binFast, binSlow;
	m_rx_detector->GetBinning(binFast, binSlow);
	bin = Bin(binFast, binSlow);
}

void Camera::checkBin(Bin& bin) {
}

void Camera::setFrameDim(const FrameDim& frame_dim) {
	m_frame_dim = frame_dim;
}

void Camera::getFrameDim(FrameDim& frame_dim) {
	frame_dim = m_frame_dim;
}

void Camera::reset() {
	stopAcq();
	init();
}

HwInterface::StatusType::Basic Camera::getStatus() {
	switch (m_detector_status) {
		case DETECTOR_STATUS_IDLE:
			return HwInterface::StatusType::Ready;
		case DETECTOR_STATUS_INTEGRATING:
			return HwInterface::StatusType::Exposure;
		default:
			throw LIMA_HW_EXC(Error, "Invalid status");
	}
}

void Camera::prepareAcq() {

}

void Camera::startAcq() {
	m_rx_detector->SetupAcquisitionSequence(m_nb_frames, static_cast<craydl::VirtualFrameCallback *> (m_frame_status_cb), 1);

#warning Other frame types?
	m_rx_detector->StartAcquisition(craydl::ACQUIRE_LIGHT);
	//m_acquiring = true;
}

void Camera::stopAcq() {
	m_rx_detector->EndAcquisition(true);
	//m_acquiring = false;
}

void Camera::getTrigMode(TrigMode &mode) {
#warning Trig mode functionality to do
	mode = IntTrig;
}

void Camera::setTrigMode(TrigMode mode) {
#warning Trig mode functionality to do
}

int Camera::getNbAcquiredFrames() {
	return m_frame_status_cb->frameCountCorrected();
}

void Camera::getDetectorModel(std::string &model) {
	std::string junk;
   m_rx_detector->GetDetectorID(model, junk);
}

void Camera::setRoi(const Roi& roi) {

}

void Camera::getRoi(Roi& roi) {

}

void Camera::checkRoi(Roi& roi) {

}

//void Camera::acquisitionComplete() {
//   m_acquiring = false;
//}
