#include "Simulator.h"

#include <string>
#include <time.h>
#include <cmath>

using namespace lima;
using namespace std;

Simulator::SimuThread::SimuThread(Simulator& simu)
	: m_simu(simu)
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
	StdBufferCbMgr& buffer_mgr = m_simu.m_buffer_cb_mgr;
	buffer_mgr.setStartTimestamp(Timestamp::now());

	struct timespec treq, trem;
	double exp_time = m_simu.m_exp_time;
	treq.tv_sec = int(floor(exp_time));
	treq.tv_nsec = int((exp_time - treq.tv_sec) * 1e9);
	
	int nb_frames = m_simu.m_nb_frames;
	int& frame_nb = m_acq_frame_nb;
	for (frame_nb = 0; frame_nb < nb_frames; frame_nb++) {
		setStatus(Exposure);
		nanosleep(&treq, &trem);
		setStatus(Readout);
	}
	setStatus(Ready);
}

Simulator::Simulator()
	: m_buffer_cb_mgr(m_buffer_alloc_mgr),
	  m_buffer_ctrl_mgr(m_buffer_cb_mgr),
	  m_thread(*this)
{
	m_exp_time = 1.0;
	m_nb_frames = 1;

	m_thread.start();
}

Simulator::~Simulator()
{
}

BufferCtrlMgr& Simulator::getBufferMgr()
{
	return m_buffer_ctrl_mgr;
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

void Simulator::setBin(Bin bin)
{
	m_frame_builder.m_bin = bin;
}

void Simulator::getBin(Bin& bin)
{
	bin = m_frame_builder.m_bin;
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
	default:
		throw LIMA_HW_EXC(Error, "Invalid thread status");
	}
}

void Simulator::startAcq()
{
	m_thread.sendCmd(SimuThread::StartAcq);
	m_thread.waitNotStatus(SimuThread::Ready);
}

void Simulator::stopAcq()
{
	m_thread.sendCmd(SimuThread::StopAcq);
	m_thread.waitStatus(SimuThread::Ready);
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
	default:
		status = "Unknown";
	}
	os << "<status=" << status << ">";
	return os;
}
