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
#ifndef ACQSTATE_H
#define ACQSTATE_H

#include "ThreadUtils.h"

#include <ostream>

namespace lima
{

class AcqState
{
public:
	enum State {
		Idle		= 1 << 0, 
		Acquiring	= 1 << 1, 
		Saving		= 1 << 2,
		Finished	= 1 << 3, 
		Aborted		= 1 << 4,
	};

	AcqState();

	void set(State new_state);
	State get();
	void wait(State state);
	void waitNot(State state);

private:
	AutoMutex lock();
	Cond m_cond;
	State m_state;
};

inline AcqState::State operator |(AcqState::State s1, AcqState::State s2)
{
	return AcqState::State(int(s1) | int(s2));
}

inline AcqState::State operator &(AcqState::State s1, AcqState::State s2)
{
	return AcqState::State(int(s1) & int(s2));
}

std::ostream& operator <<(std::ostream& os, AcqState::State state);

} // namespace lima

#endif // ACQSTATE_H
