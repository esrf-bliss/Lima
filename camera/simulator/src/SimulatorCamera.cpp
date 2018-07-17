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

#include <unistd.h>

#include "SimulatorCamera.h"
#include "SimulatorFrameGetter.h"
#include "SimulatorFrameBuilder.h"
#include "SimulatorFrameLoader.h"
#include "SimulatorFramePrefetcher.h"


using namespace lima;
using namespace lima::Simulator;


Camera::SimuThread::SimuThread(Camera& simu)
	: m_simu(&simu)
{
    DEB_CONSTRUCTOR();

	m_acq_frame_nb = 0;
}

Camera::SimuThread::~SimuThread()
{
	DEB_DESTRUCTOR();
	abort();
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

	switch (cmd) {
	case PrepareAcq:
		execPrepareAcq();
		break;
	case StartAcq:
		execStartAcq();
		break;
	case StopAcq:
		setStatus(Ready);
		break;
	}
}

void Camera::SimuThread::execPrepareAcq()
{
	DEB_MEMBER_FUNCT();

	m_acq_frame_nb = 0;

	// Delegate to the frame getter that may need some preparation
	m_simu->m_frame_getter->prepareAcq();

	setStatus(Prepared);
}

void Camera::SimuThread::execStartAcq()
{
    DEB_MEMBER_FUNCT();

	StdBufferCbMgr& buffer_mgr = m_simu->m_buffer_ctrl_obj.getBuffer();
	buffer_mgr.setStartTimestamp(Timestamp::now());

	FrameGetter* frame_getter = m_simu->m_frame_getter;
    frame_getter->resetFrameNr(m_acq_frame_nb);

	int nb_frames = m_simu->m_trig_mode == IntTrig ? m_simu->m_nb_frames : m_acq_frame_nb + 1;
	int& frame_nb = m_acq_frame_nb;
	for (; (nb_frames==0) || (frame_nb < nb_frames); frame_nb++) {
		if (getNextCmd() == StopAcq) {
			waitNextCmd();
			break;
		}
		double req_time;
		req_time = m_simu->m_exp_time;
		if (req_time > 1e-6) {
			setStatus(Exposure);
			usleep(long(req_time * 1e6));
		}

		setStatus(Readout);
        unsigned char *ptr = reinterpret_cast<unsigned char *>(buffer_mgr.getFrameBufferPtr(frame_nb));

		// Get the next frame
        bool res = frame_getter->getNextFrame(ptr);
        if (!res)
            throw LIMA_HW_EXC(InvalidValue, "Failed to get next frame");

		HwFrameInfoType frame_info;
		frame_info.acq_frame_nb = frame_nb;
		buffer_mgr.newFrameReady(frame_info);

		req_time = m_simu->m_lat_time;
		if (req_time > 1e-6) {
			setStatus(Latency);
			usleep(long(req_time * 1e6));
		}
	}
	setStatus(Ready);
}


const char* Camera::DetectorModel[] = {
    "Generator",
    "Generator Prefetched"
    "Loader",
    "Loader Prefetched"
};


Camera::Camera(const Mode& mode) :
    m_mode(mode),
    m_frame_getter(NULL),
    m_thread(*this)
{
    DEB_CONSTRUCTOR();

	setDefaultProperties();

    constructFrameGetter();

	m_thread.start();
	m_thread.waitStatus(SimuThread::Ready);
}

void Camera::setDefaultProperties()
{
    DEB_MEMBER_FUNCT();

	m_exp_time = 1.0;
	m_lat_time = 0.0;
	m_nb_frames = 1;
}


void Camera::constructFrameGetter()
{
    switch (m_mode)
    {
    case Mode::MODE_GENERATOR:
        m_frame_getter = new FrameBuilder();
        break;

    case Mode::MODE_GENERATOR_PREFETCH:
        m_frame_getter = new FramePrefetcher<FrameBuilder>();
        break;

    case Mode::MODE_LOADER:
        m_frame_getter = new FrameLoader();
        break;

    case Mode::MODE_LOADER_PREFETCH:
        m_frame_getter = new FramePrefetcher<FrameLoader>();
        break;
    }
}


Camera::~Camera()
{
    DEB_DESTRUCTOR();

    delete m_frame_getter;
}


void Camera::setMode(const Mode& mode)
{
    if (mode != m_mode)
    {
        delete m_frame_getter;

        m_mode = mode;
        constructFrameGetter();
    }
}

void Camera::setFrameDim(const FrameDim& frame_dim)
{
    m_frame_getter->setFrameDim(frame_dim);
}

void Camera::getFrameDim(FrameDim& frame_dim)
{
    m_frame_getter->getFrameDim(frame_dim);
}

void Camera::getMaxImageSize(Size& max_image_size) const
{
    m_frame_getter->getMaxImageSize(max_image_size);
}

FrameBuilder* Camera::getFrameBuilder()
{
    return dynamic_cast<FrameBuilder *>(m_frame_getter);
}

FramePrefetcher<FrameBuilder>* Camera::getFrameBuilderPrefetched()
{
    return dynamic_cast<FramePrefetcher<FrameBuilder> *>(m_frame_getter);
}

FrameLoader* Camera::getFrameLoader()
{
    return dynamic_cast<FrameLoader *>(m_frame_getter);
}

FramePrefetcher<FrameLoader>* Camera::getFrameLoaderPrefetched()
{
    return dynamic_cast<FramePrefetcher<FrameLoader> *>(m_frame_getter);
}

void Camera::getDetectorModel(std::string& det_model) const
{
    det_model = DetectorModel[m_frame_getter->getMode()];
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

void Camera::reset()
{
	stopAcq();

	setDefaultProperties();
}

HwInterface::StatusType::Basic Camera::getStatus()
{
	DEB_MEMBER_FUNCT();

	int thread_status = m_thread.getStatus();
	switch (thread_status) {
	case SimuThread::Ready:
	case SimuThread::Prepared:
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

	if (m_thread.getStatus() == SimuThread::Prepared)
		return;
	if (m_thread.getStatus() != SimuThread::Ready)
		THROW_HW_ERROR(Error) << "Camera not Ready";
	m_thread.sendCmd(SimuThread::PrepareAcq);
	m_thread.waitStatus(SimuThread::Prepared);
}

void Camera::startAcq()
{
	DEB_MEMBER_FUNCT();

	int status = m_thread.getStatus();
	if (status != SimuThread::Prepared && status != SimuThread::Ready)
		THROW_HW_ERROR(Error) << "Camera not Prepared and not Ready neither";

	m_buffer_ctrl_obj.getBuffer().setStartTimestamp(Timestamp::now());

	m_thread.sendCmd(SimuThread::StartAcq);
	
	m_thread.waitStatus(SimuThread::Exposure);
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

std::ostream& lima::Simulator::operator <<(std::ostream& os, Camera& simu)
{
	std::string status;
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
