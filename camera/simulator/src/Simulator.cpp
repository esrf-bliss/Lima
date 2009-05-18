#include "Simulator.h"

using namespace lima;

Simulator::SimuThread::SimuThread(Simulator& simu)
	: m_simu(simu)
{
	start();
	waitStatus(Ready);
}

void Simulator::SimuThread::init()
{
	setStatus(Ready);
}

void Simulator::SimuThread::execCmd(int cmd)
{
	
}

Simulator::Simulator()
	: m_thread(*this)
{
	m_exp_time = 1.0;
	m_nb_frames = 1;
	
}

Simulator::~Simulator()
{
}

BufferCtrlMgr& Simulator::getBufferMgr()
{
	return m_buffer_mgr;
}

