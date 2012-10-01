#include "RayonixHsInterface.h"
#include "RayonixHsCamera.h"
#include "RayonixHsDetInfoCtrlObj.h"
#include "RayonixHsBufferCtrlObj.h"
#include "RayonixHsBinCtrlObj.h"
//#include "RayonixHsRoiCtrlObj.h"
#include "RayonixHsSyncCtrlObj.h"

using namespace lima;
using namespace lima::RayonixHs;

Interface::Interface(Camera *cam)
	: m_cam(cam) {

	DEB_CONSTRUCTOR();

	m_det_info = new DetInfoCtrlObj(cam);
	m_buffer = new BufferCtrlObj(cam);
	m_sync = new SyncCtrlObj(cam, m_buffer);
	cam->m_sync = m_sync;
	m_bin = new BinCtrlObj(cam);
	//m_roi = new RoiCtrlObj(cam);

	if (m_buffer)
		m_buffer->m_sync = m_sync;
}

Interface::~Interface() {
	DEB_DESTRUCTOR();

	delete m_buffer;
	delete m_det_info;
	delete m_sync;
	delete m_bin;
	//delete m_roi;
}

void Interface::getCapList(CapList &cap_list) const {
	cap_list.push_back(HwCap(m_sync));
	cap_list.push_back(HwCap(m_det_info));
	cap_list.push_back(HwCap(m_buffer));
	cap_list.push_back(HwCap(m_bin));
	//cap_list.push_back(HwCap(m_roi));
}

void Interface::reset(ResetLevel reset_level) {
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(reset_level);

	m_sync->stopAcq();
	m_cam->reset();
}

void Interface::prepareAcq() {
	DEB_MEMBER_FUNCT();
	if (m_buffer)
		m_buffer->prepareAcq();
}

void Interface::startAcq() {
	DEB_MEMBER_FUNCT();

	m_sync->startAcq();
}

void Interface::stopAcq() {
	DEB_MEMBER_FUNCT();

	m_sync->stopAcq();
}

void Interface::getStatus(StatusType& status) {
	m_sync->getStatus(status);
}

int Interface::getNbAcquiredFrames() {
	DEB_MEMBER_FUNCT();

	int aNbAcquiredFrames;
	if (m_buffer)
		aNbAcquiredFrames = m_buffer->getNbAcquiredFrames();
	else
		aNbAcquiredFrames = m_cam->getNbAcquiredFrames();

	DEB_RETURN() << DEB_VAR1(aNbAcquiredFrames);
	return aNbAcquiredFrames;
}

int Interface::getNbHwAcquiredFrames() {
	return getNbAcquiredFrames();
}

