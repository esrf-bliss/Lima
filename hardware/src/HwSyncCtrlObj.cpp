#include "HwSyncCtrlObj.h"

using namespace lima;

HwSyncCtrlObj::HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl)
	: m_buffer_ctrl(buffer_ctrl)
{
	DEB_CONSTRUCTOR();
}

HwSyncCtrlObj::~HwSyncCtrlObj()
{
	DEB_DESTRUCTOR();
}

void HwSyncCtrlObj::setNbFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);

	int nb_acc_frames;
	m_buffer_ctrl.getNbAccFrames(nb_acc_frames);
	int real_nb_frames = nb_frames * nb_acc_frames;
	setNbHwFrames(real_nb_frames);
}

void HwSyncCtrlObj::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	getNbHwFrames(nb_frames);
	int nb_acc_frames;
	m_buffer_ctrl.getNbAccFrames(nb_acc_frames);
	nb_frames /= nb_acc_frames;
	DEB_RETURN() << DEB_VAR1(nb_frames);
}

