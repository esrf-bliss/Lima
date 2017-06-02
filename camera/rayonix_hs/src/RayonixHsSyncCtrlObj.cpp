#include <sstream>

#include "RayonixHsSyncCtrlObj.h"
#include "RayonixHsBufferCtrlObj.h"
#include "RayonixHsCamera.h"

using namespace lima;
using namespace lima::RayonixHs;

SyncCtrlObj::SyncCtrlObj(Camera *cam, BufferCtrlObj *buffer)
	: m_cam(cam),
	  m_trig_mode(IntTrig),
	  m_buffer(buffer),
	  m_nb_frames(1),
	  m_started(false) {

	DEB_CONSTRUCTOR();
}

SyncCtrlObj::~SyncCtrlObj() {
	DEB_DESTRUCTOR();
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode) {
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(trig_mode);

	return m_cam->checkTrigMode(trig_mode);
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode) {
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(trig_mode);
	bool error = true;

	if (checkTrigMode(trig_mode)) {
		m_cam->setTrigMode(trig_mode);
	}
	else
		throw LIMA_HW_EXC(NotSupported,"Trigger type not supported");
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode) {
	m_cam->getTrigMode(trig_mode);
}

void SyncCtrlObj::setExpTime(double exp_time) {
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(exp_time);

        m_cam->setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double &exp_time) {
	DEB_MEMBER_FUNCT();

	m_cam->getExpTime(exp_time);

	DEB_RETURN() << DEB_VAR1(exp_time);
}

void SyncCtrlObj::setLatTime(double lat_time) {
	m_cam->setLatTime(lat_time);
}

void SyncCtrlObj::getLatTime(double& lat_time) {
	m_cam->getLatTime(lat_time);
}

void SyncCtrlObj::setNbFrames(int nb_frames) {
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);

	m_cam->setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbFrames(int& nb_frames) {
	m_cam->getNbFrames(nb_frames);
}

void SyncCtrlObj::setNbHwFrames(int nb_frames) {
	setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames) {
	getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges) {
	valid_ranges.min_exp_time = 0;
	valid_ranges.max_exp_time = 86400;
	valid_ranges.min_lat_time = 0.;
	valid_ranges.max_lat_time = 0.;
}

void SyncCtrlObj::startAcq() {
	DEB_MEMBER_FUNCT();

	if (!m_cam->acquiring()) {
		m_cam->startAcq();
	}
}

void SyncCtrlObj::stopAcq(bool clearQueue) {
	DEB_MEMBER_FUNCT();

	if (m_cam->acquiring()) {
		DEB_TRACE() << "Try to stop Acq";
		m_cam->stopAcq();
	}
}

void SyncCtrlObj::getStatus(HwInterface::StatusType& status) {
	DEB_MEMBER_FUNCT();

	switch (m_cam->getStatus()) {
	   case HwInterface::StatusType::Exposure:
	       status.acq = AcqRunning;
	       status.det = DetExposure;
	       break;
	   case HwInterface::StatusType::Ready:
		   status.acq = AcqReady;
		   status.det = DetIdle;
		   break;
	}
	DEB_RETURN() << DEB_VAR1(status);
}
