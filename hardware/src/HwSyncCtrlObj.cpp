#include "HwSyncCtrlObj.h"

using namespace lima;

HwSyncCtrlObj::HwSyncCtrlObj(HwBufferCtrlObj& buffer_ctrl)
	: m_buffer_ctrl(buffer_ctrl)
{
}

HwSyncCtrlObj::~HwSyncCtrlObj()
{
}

void HwSyncCtrlObj::setNbFrames(int nb_frames)
{
	int nb_acc_frames;
	m_buffer_ctrl.getNbAccFrames(nb_acc_frames);
	int real_nb_frames = nb_frames * nb_acc_frames;
	setNbHwFrames(real_nb_frames);
}

void HwSyncCtrlObj::getNbFrames(int& nb_frames)
{
	getNbHwFrames(nb_frames);
	int nb_acc_frames;
	m_buffer_ctrl.getNbAccFrames(nb_acc_frames);
	nb_frames /= nb_acc_frames;
}

