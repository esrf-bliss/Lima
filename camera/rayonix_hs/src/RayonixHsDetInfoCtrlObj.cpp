#include <cstdlib>

#include "RayonixHsDetInfoCtrlObj.h"
#include "RayonixHsCamera.h"

using namespace lima;
using namespace lima::RayonixHs;

DetInfoCtrlObj::DetInfoCtrlObj(Camera *cam)
	: m_cam(cam) {
}

DetInfoCtrlObj::~DetInfoCtrlObj() {
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size) {
	m_cam->getMaxImageSize(max_image_size);
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size) {
	getMaxImageSize(det_image_size);
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type) {
	m_cam->getImageType(def_image_type);
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type) {
	m_cam->getImageType(curr_image_type);
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type) {
	m_cam->setImageType(curr_image_type);
}

void DetInfoCtrlObj::getPixelSize(double& x_size, double& y_size) {
	m_cam->getPixelSize(x_size, y_size);
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type) {
	m_cam->getDetectorType(det_type);
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model) {
	m_cam->getDetectorModel(det_model);
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb) {
	m_cam->registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb) {
	m_cam->unregisterMaxImageSizeCallback(cb);
}

