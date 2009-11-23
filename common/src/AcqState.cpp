#include "AcqState.h"

using namespace lima;

AcqState::AcqState()
	: m_state(Idle) 
{
}

AutoMutex AcqState::lock()
{
	return AutoMutex(m_cond.mutex());
}

void AcqState::set(State new_state)
{
	AutoMutex l = lock();
	m_state = new_state;
	m_cond.signal();
}

AcqState::State AcqState::get()
{
	AutoMutex l = lock();
	return m_state;
}

void AcqState::wait(State state)
{
	AutoMutex l = lock();
	while ((m_state & state) == 0)
		m_cond.wait();
}

void AcqState::waitNot(State state)
{
	AutoMutex l = lock();
	while ((m_state & state) != 0)
		m_cond.wait();
}

