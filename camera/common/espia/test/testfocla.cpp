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
/***************************************************************//**
 * @file testfocla.cpp
 * @brief This file is to test Espia::Focla::Dev class
 *
 * @author A.Kirov
 * @date 15/07/2009
 *******************************************************************/

#include <iostream>

#include "EspiaFocla.h"
#include "EspiaBufferMgr.h"
#include "HwBufferSave.h"
#include "Timestamp.h"

using namespace lima;
using namespace std;


int main(int argc, char *argv[])
{

  try {
	Espia::Dev espia_dev(0);

	Espia::Focla::Dev focla(espia_dev);

	Espia::Acq espia_acq(espia_dev);
	Espia::BufferMgr espia_buffer_mgr(espia_acq);
	BufferCtrlMgr buffer_mgr(espia_buffer_mgr);


	for( map<string, int>::iterator p = 
	            Espia::Focla::ParamName2IdxMap.begin();
	            p != Espia::Focla::ParamName2IdxMap.end(); ++p ) 
	{
		cout << p->first << " : " << p->second << endl;
	}


	int val;
	focla.getParam( Espia::Focla::TEST_IMAGE, val );
	cout << endl << "\"TEST_IMAGE\" = " << val << endl;

	focla.setParam( Espia::Focla::TEST_IMAGE, 0 );
	cout << endl << "\"TEST_IMAGE\" set to 0" << endl;

	focla.getParam( Espia::Focla::TEST_IMAGE, val );
	cout << endl << "TEST_IMAGE = " << val << endl;

	espia_dev.getDrvOption( "NO_FIFO_RESET", val );
	cout << endl << "NO_FIFO_RESET = " << val << endl;
	espia_dev.setDrvOption( "NO_FIFO_RESET", 1 );
	cout << "NO_FIFO_RESET set to 1" << endl;
	espia_dev.getDrvOption( "NO_FIFO_RESET", val );
	cout << endl << "NO_FIFO_RESET = " << val << endl;


	FrameDim frame_dim(1024, 1024, Bpp16);
	buffer_mgr.setFrameDim(frame_dim);
	int max_nb_buffers;
	buffer_mgr.getMaxNbBuffers(max_nb_buffers);
	cout << "MaxNbBuffers " << max_nb_buffers << endl;
	int nb_concat_frames = 1;
	buffer_mgr.setNbConcatFrames(nb_concat_frames);
	int nb_buffers = max_nb_buffers/2;
	buffer_mgr.setNbBuffers(nb_buffers);
	cout << "Allocated " << nb_buffers << " buffers" << endl << flush;

	buffer_mgr.getFrameDim(frame_dim);
	buffer_mgr.getNbBuffers(nb_buffers);
	buffer_mgr.getNbConcatFrames(nb_concat_frames);
	cout << "FrameDim " << frame_dim << ", "
	     << "NbBuffers " << nb_buffers << ", "
	     << "NbConcatFrames " << nb_concat_frames << endl << flush;


	HwBufferSave bs(HwBufferSave::EDF, "foclatest");
	bs.setTotFileFrames(1);

	espia_acq.setNbFrames(1000);
	espia_acq.start();
	cout << "Acquisition started" << endl << flush;

	focla.setParam( Espia::Focla::TEST_IMAGE, 1 );

	Espia::Acq::StatusType acq_status;
	Timestamp start = Timestamp::now();
	for( int i=0; i<10; i++ ) {
		Sleep(0.5);

		espia_acq.getStatus(acq_status);
		cout << "running " << acq_status.running << ", "
		     << "last_frame_nb " << acq_status.last_frame_nb << endl;

		Timestamp t = Timestamp::now() - start;
		int pixels = Point(frame_dim.getSize()).getArea();
		void *buffer = buffer_mgr.getBufferPtr(0);
		HwFrameInfoType finfo(i, buffer, &frame_dim, t, pixels,
				      HwFrameInfoType::Managed);
		bs.writeFrame(finfo);
		cout << "Written frame number " << i << endl << flush;
	}

	espia_acq.stop();
	cout << "Acquisition stopped" << endl << flush;

  } catch (Exception e) {
	cerr << "LIMA Exception: " << e << endl;
	return -1;
  }

	return 0;
}
