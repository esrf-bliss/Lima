#include "AcqFinished.h"

using namespace lima;

AcqFinished::AcqFinished()
	: m_finished(false) 
{
}

AutoMutex AcqFinished::lock()
{
	return AutoMutex(m_cond.mutex());
}

void AcqFinished::signalFinished()
{
	AutoMutex l = lock();
	m_finished = true;
	m_cond.signal();
}

bool AcqFinished::hasFinished()
{
	AutoMutex l = lock();
	return m_finished;
}

void AcqFinished::resetFinished()
{
	AutoMutex l = lock();
	m_finished = false;
}

void AcqFinished::waitFinished()
{
	AutoMutex l = lock();
	while (!m_finished)
		m_cond.wait();
}

