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
#include "lima/AcqState.h"

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

