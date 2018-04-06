//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include "SimulatorCamera.h"

#include <string>
#include <unistd.h>
#include <cmath>

using namespace lima;
using namespace lima::Simulator;
using namespace std;

Camera::SimuThread::SimuThread(Camera& simu)
	: m_simu(&simu)
{
	DEB_CONSTRUCTOR();

	m_acq_frame_nb = 0;
}

void Camera::SimuThread::start()
{
	DEB_MEMBER_FUNCT();

	CmdThread::start();
	waitStatus(Ready);
}

void Camera::SimuThread::init()
{
	DEB_MEMBER_FUNCT();

	setStatus(Ready);
}

void Camera::SimuThread::execCmd(int cmd)
{
	DEB_MEMBER_FUNCT();

	int status = getStatus();
	switch (cmd) {
	case PrepareAcq:
		m_acq_frame_nb = 0;
		setStatus(Prepare);
		break;
	case StartAcq:
		execStartAcq();
		break;
	case StopAcq:
		setStatus(Ready);
		break;
	}
}

void Camera::SimuThread::execStartAcq()
{
	DEB_MEMBER_FUNCT();

	StdBufferCbMgr& buffer_mgr = m_simu->m_buffer_ctrl_obj.getBuffer();
	buffer_mgr.setStartTimestamp(Timestamp::now());

	FrameBuilder& frame_builder = m_simu->m_frame_builder;
	frame_builder.resetFrameNr(m_acq_frame_nb);

	bool int_trig = m_simu->m_trig_mode == IntTrig;
	int nb_frames = int_trig ? m_simu->m_nb_frames : m_acq_frame_nb + 1;
	int& frame_nb = m_acq_frame_nb;
	for (; (nb_frames==0) || (frame_nb < nb_frames); frame_nb++) {
		if (getNextCmd() == StopAcq) {
			waitNextCmd();
			break;
		}
		double req_time;
		req_time = m_simu->m_exp_time;
		if (req_time > 0) {	
			setStatus(Exposure);
			Sleep(req_time);
		}

		setStatus(Readout);
		void *ptr = buffer_mgr.getFrameBufferPtr(frame_nb);
		typedef unsigned char *BufferPtr;
		frame_builder.getNextFrame(BufferPtr(ptr));

		HwFrameInfoType frame_info;
		frame_info.acq_frame_nb = frame_nb;
		buffer_mgr.newFrameReady(frame_info);

		req_time = m_simu->m_lat_time;
		if (req_time > 0) {
			setStatus(Latency);
			Sleep(req_time);
		}
	}
	setStatus(Ready);
}

Camera::Camera() : 
	m_thread(*this)
{
	DEB_CONSTRUCTOR();

	init();

	m_thread.start();
	m_thread.waitStatus(SimuThread::Ready);
}

void Camera::init()
{
	DEB_MEMBER_FUNCT();

	m_exp_time = 1.0;
	m_lat_time = 0.0;
	m_nb_frames = 1;
}

Camera::~Camera()
{
	DEB_DESTRUCTOR();
	stopAcq();
}

HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
	return &m_buffer_ctrl_obj;
}

FrameBuilder* Camera::getFrameBuilder()
{
	return &m_frame_builder;
}

void Camera::getMaxImageSize(Size& max_image_size)
{
	m_frame_builder.getMaxImageSize(max_image_size);
}

void Camera::setNbFrames(int nb_frames)
{
	if (nb_frames < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb of frames");

	m_nb_frames = nb_frames;
}

void Camera::getNbFrames(int& nb_frames)
{
	nb_frames = m_nb_frames;
}

void Camera::setExpTime(double exp_time)
{
	if (exp_time <= 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid exposure time");
		
	m_exp_time = exp_time;
}

void Camera::getExpTime(double& exp_time)
{
	exp_time = m_exp_time;
}

void Camera::setLatTime(double lat_time)
{
	if (lat_time < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid latency time");
		
	m_lat_time = lat_time;
}

void Camera::getLatTime(double& lat_time)
{
	lat_time = m_lat_time;
}

void Camera::setBin(const Bin& bin)
{
	m_frame_builder.setBin(bin);
}

void Camera::getBin(Bin& bin)
{
	m_frame_builder.getBin(bin);
}

void Camera::checkBin(Bin& bin)
{
	m_frame_builder.checkBin(bin);
}

void Camera::setFrameDim(const FrameDim& frame_dim)
{
	m_frame_builder.setFrameDim(frame_dim);
}

void Camera::getFrameDim(FrameDim& frame_dim)
{
	m_frame_builder.getFrameDim(frame_dim);
}

void Camera::reset()
{
	stopAcq();

	init();
}

HwInterface::StatusType::Basic Camera::getStatus()
{
	DEB_MEMBER_FUNCT();

	int thread_status = m_thread.getStatus();
	switch (thread_status) {
	case SimuThread::Ready:
	case SimuThread::Prepare:
		return HwInterface::StatusType::Ready;
	case SimuThread::Exposure:
		return HwInterface::StatusType::Exposure;
	case SimuThread::Readout:
		return HwInterface::StatusType::Readout;
	case SimuThread::Latency:
		return HwInterface::StatusType::Latency;
	default:
		THROW_HW_ERROR(Error) <<  "Invalid thread status";
	}
}

void Camera::prepareAcq()
{
	DEB_MEMBER_FUNCT();

	if (m_thread.getStatus() == SimuThread::Prepare)
		return;
	if (m_thread.getStatus() != SimuThread::Ready)
		THROW_HW_ERROR(Error) << "Camera not Ready";
	m_thread.sendCmd(SimuThread::PrepareAcq);
	m_thread.waitStatus(SimuThread::Prepare);
}

void Camera::startAcq()
{
	DEB_MEMBER_FUNCT();

	if (m_thread.getStatus() != SimuThread::Prepare)
		THROW_HW_ERROR(Error) << "Camera not Prepared";

	m_buffer_ctrl_obj.getBuffer().setStartTimestamp(Timestamp::now());

	m_thread.sendCmd(SimuThread::StartAcq);
	m_thread.waitNotStatus(SimuThread::Ready);
}

void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();

	if (m_thread.getStatus() != SimuThread::Ready) {
		m_thread.sendCmd(SimuThread::StopAcq);
		m_thread.waitStatus(SimuThread::Ready);
	}
}

int Camera::getNbAcquiredFrames()
{
	return m_thread.m_acq_frame_nb;
}

ostream& lima::Simulator::operator <<(ostream& os, Camera& simu)
{
	string status;
	switch (simu.getStatus()) {
	case HwInterface::StatusType::Ready:
		status = "Ready"; break;
	case HwInterface::StatusType::Exposure:
		status = "Exposure"; break;
	case HwInterface::StatusType::Readout:
		status = "Readout"; break;
	case HwInterface::StatusType::Latency:
		status = "Latency"; break;
	default:
		status = "Unknown";
	}
	os << "<status=" << status << ">";
	return os;
}
