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
#include "HwBufferSave.h"
#include "PoolThreadMgr.h"

#include <iostream>
#include <cstdlib>

#include <stdlib.h>


using namespace lima;
using namespace std;

class TestFrameCallback : public HwFrameCallback
{
public:
	TestFrameCallback(Simulator& simu, HwBufferSave& buffer_save) 
		: m_simu(simu), m_buffer_save(buffer_save) {}
protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info);
private:
	Simulator& m_simu;
	HwBufferSave& m_buffer_save;
};

bool TestFrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	cout << "acq_frame_nb=" << frame_info.acq_frame_nb  << ", "
	     << "ts=" << frame_info.frame_timestamp << ", "
	     << "simu=" << m_simu  << endl;
	m_buffer_save.writeFrame(frame_info);
	return true;
}


int main(int argc, char *argv[])
{
	Simulator simu;
	HwBufferSave buffer_save(HwBufferSave::EDF);
	TestFrameCallback cb(simu, buffer_save);

	FrameDim frame_dim;
	simu.getFrameDim(frame_dim);
	Size size = frame_dim.getSize();

	BufferCtrlMgr& buffer_mgr = simu.getBufferMgr();
	buffer_mgr.setFrameDim(frame_dim);
	buffer_mgr.setNbBuffers(10);
	buffer_mgr.registerFrameCallback(cb);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	simu.setExpTime(5);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	simu.setExpTime(2);
	simu.setNbFrames(3);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	PoolThreadMgr &pMgr = PoolThreadMgr::get();
	pMgr.quit();
	exit(0);
}
