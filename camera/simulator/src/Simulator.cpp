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
#include "Simulator.h"

#include <string>
#include <unistd.h>
#include <cmath>

using namespace lima;
using namespace std;

Simulator::SimuThread::SimuThread(Simulator& simu)
	: m_simu(&simu)
{
	m_acq_frame_nb = 0;
}

void Simulator::SimuThread::start()
{
	CmdThread::start();
	waitStatus(Ready);
}

void Simulator::SimuThread::init()
{
	setStatus(Ready);
}

void Simulator::SimuThread::execCmd(int cmd)
{
	int status = getStatus();
	switch (cmd) {
	case StartAcq:
		if (status != Ready)
			throw LIMA_HW_EXC(InvalidValue, 
					  "Not Ready to StartAcq");
		execStartAcq();
		break;
	}
}

void Simulator::SimuThread::execStartAcq()
{
	StdBufferCbMgr& buffer_mgr = m_simu->m_buffer_cb_mgr;
	buffer_mgr.setStartTimestamp(Timestamp::now());

	FrameBuilder& frame_builder = m_simu->m_frame_builder;
	frame_builder.resetFrameNr();

	int nb_frames = m_simu->m_nb_frames;
	int& frame_nb = m_acq_frame_nb;
	for (frame_nb = 0; frame_nb < nb_frames; frame_nb++) {
		double req_time;

		req_time = m_simu->m_exp_time;
		if (req_time > 0) {	
			setStatus(Exposure);
			usleep(long(req_time * 1e6));
		}

		setStatus(Readout);
		int buffer_nb, concat_frame_nb;
		buffer_mgr.acqFrameNb2BufferNb(frame_nb, buffer_nb, 
					       concat_frame_nb);
		void *ptr = buffer_mgr.getBufferPtr(buffer_nb,
						    concat_frame_nb);
		typedef unsigned char *BufferPtr;
		frame_builder.getNextFrame(BufferPtr(ptr));

		HwFrameInfoType frame_info;
		frame_info.acq_frame_nb = frame_nb;
		buffer_mgr.newFrameReady(frame_info);

		req_time = m_simu->m_lat_time;
		if (req_time > 0) {
			setStatus(Latency);
			usleep(long(req_time * 1e6));
		}
	}
	setStatus(Ready);
}

int Simulator::SimuThread::getNbAcquiredFrames()
{
	return m_acq_frame_nb;
}

Simulator::Simulator()
	: m_buffer_cb_mgr(m_buffer_alloc_mgr),
	  m_buffer_ctrl_mgr(m_buffer_cb_mgr),
	  m_thread(*this)
{
	init();

	m_thread.start();
}

void Simulator::init()
{
	m_exp_time = 1.0;
	m_lat_time = 0.0;
	m_nb_frames = 1;
}

Simulator::~Simulator()
{
}

BufferCtrlMgr& Simulator::getBufferMgr()
{
	return m_buffer_ctrl_mgr;
}

void Simulator::getMaxImageSize(Size& max_image_size)
{
	m_frame_builder.getMaxImageSize(max_image_size);
}

void Simulator::setNbFrames(int nb_frames)
{
	if (nb_frames < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb of frames");

	m_nb_frames = nb_frames;
}

void Simulator::getNbFrames(int& nb_frames)
{
	nb_frames = m_nb_frames;
}

void Simulator::setExpTime(double exp_time)
{
	if (exp_time <= 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid exposure time");
		
	m_exp_time = exp_time;
}

void Simulator::getExpTime(double& exp_time)
{
	exp_time = m_exp_time;
}

void Simulator::setLatTime(double lat_time)
{
	if (lat_time < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid latency time");
		
	m_lat_time = lat_time;
}

void Simulator::getLatTime(double& lat_time)
{
	lat_time = m_lat_time;
}

void Simulator::setBin(const Bin& bin)
{
	m_frame_builder.setBin(bin);
}

void Simulator::getBin(Bin& bin)
{
	m_frame_builder.getBin(bin);
}

void Simulator::checkBin(Bin& bin)
{
	m_frame_builder.checkBin(bin);
}

void Simulator::setFrameDim(const FrameDim& frame_dim)
{
	m_frame_builder.setFrameDim(frame_dim);
}

void Simulator::getFrameDim(FrameDim& frame_dim)
{
	m_frame_builder.getFrameDim(frame_dim);
}

void Simulator::reset()
{
	stopAcq();

	init();
}

Simulator::Status Simulator::getStatus()
{
	int thread_status = m_thread.getStatus();
	switch (thread_status) {
	case SimuThread::Ready:
		return Simulator::Ready;
	case SimuThread::Exposure:
		return Simulator::Exposure;
	case SimuThread::Readout:
		return Simulator::Readout;
	case SimuThread::Latency:
		return Simulator::Latency;
	default:
		throw LIMA_HW_EXC(Error, "Invalid thread status");
	}
}

void Simulator::startAcq()
{
	m_buffer_ctrl_mgr.setStartTimestamp(Timestamp::now());

	m_thread.sendCmd(SimuThread::StartAcq);
	m_thread.waitNotStatus(SimuThread::Ready);
}

void Simulator::stopAcq()
{
	m_thread.sendCmd(SimuThread::StopAcq);
	m_thread.waitStatus(SimuThread::Ready);
}

int Simulator::getNbAcquiredFrames()
{
	return m_thread.getNbAcquiredFrames();
}

ostream& lima::operator <<(ostream& os, Simulator& simu)
{
	string status;
	switch (simu.getStatus()) {
	case Simulator::Ready:
		status = "Ready"; break;
	case Simulator::Exposure:
		status = "Exposure"; break;
	case Simulator::Readout:
		status = "Readout"; break;
	case Simulator::Latency:
		status = "Latency"; break;
	default:
		status = "Unknown";
	}
	os << "<status=" << status << ">";
	return os;
}
